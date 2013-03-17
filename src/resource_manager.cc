#include <sys/types.h>
#include <iostream>
#include <memory>
#include <QString>
#include <QRegExp>
#include <QDebug>
#include <QThread>
#include <QFile>
#include <QStringList>
#include <QUrl>

#include "exceptions.h"
#include "http_resource_medium.h"
#include "resource_manager.h"
#include "dist.h"
#include "dist_tree_model.h"
#include "apt_pkg.h"

using std::nothrow;

ResourceManager::ResourceManager(): _sourcelist(), _src_db(), _model(NULL) {
    QFile deb_sys_file("/etc/debian_version");
    if (!deb_sys_file.exists()) {
	qCritical() << "This is system is not using apt";
	Exception InvApt("Not a apt based system");
	throw InvApt;
    }

    QString sourcelist_path = QString("/%1%2").arg(
	QString::fromStdString(apt_pkg_find("Dir::Etc")),
	QString::fromStdString(apt_pkg_find("Dir::Etc::sourcelist")));
    _sourcelist.setFileName(sourcelist_path);
 }

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::CheckEntry(dist_source_t type, const QString &addr)
{
    /* Iterate over db and check against existing sources. */
    for (auto iter = _src_db.begin(); iter != _src_db.end(); iter++) {
	ResourceMedium *entry = *iter;
	if (entry->get_proto() == type && entry->get_addr() == addr) {
	    return true;
	}
    }

    return false;
}

void ResourceManager::ParseResources(void)
{
    if (!_sourcelist.open(QIODevice::ReadOnly | QIODevice::Text)) {
	qCritical() << "Cannot open APT source file: '" << _sourcelist.fileName() << "'";
	return;
    }

    QRegExp pattern("^deb\\s*(http|cdrom|ssh|rsh):(.*)$");
    QTextStream stream(&_sourcelist);
    QString line;

    while (!stream.atEnd()) {
	line = stream.readLine();
	if (!pattern.exactMatch(line))
	    continue;
		
	pattern.indexIn(line);
	QString proto		= pattern.cap(1);
	QString addr		= pattern.cap(2);
	
	if (!proto.compare("http"))
	    AptHTTPSourceParser(addr);
	else if (!proto.compare("cdrom"))
	    AptCDROMSourceParser(addr);
	else
	    qDebug() << "Non implemented protocol: " << proto;
    }
    _sourcelist.close();
}

/*
 * Callback when dist is ready from ResourceMedium.
 */
void ResourceManager::DistBuilt(Dist *dist)
{
  // if (!_model) {
  //   _model = new DistTreeModel(dist);
  //   emit ModelUpdate(_model);
  //   return;
  // }

  // ((DistTreeModel *)_model)->Merge(dist);
  qDebug() << "Resource is ready from resource_manager" << QThread::currentThread();
  emit resource_ready(dist);
}

/*
 * Generate Source Objects from given cdrom medium.
 */
void ResourceManager::AptCDROMSourceParser(QString &path)
{
    return;
}

/*
 * Generate Source Objects from given http server address.
 */
void ResourceManager::AptHTTPSourceParser(QString &srv_path)
{
    QRegExp pattern("^//([a-z0-9A-Z/\\.]+)\\s+([a-z0-9A-Z\\-]+).*$");
    if (!pattern.exactMatch(srv_path)) {
	qWarning() << "Invalid address in HTTP content: '" << srv_path << "'";
	return;
    }
    
    pattern.indexIn(srv_path);
    QString addr = pattern.cap(1);
    QString dist = pattern.cap(2);

    qDebug() << "Host: " << addr << ", dist: " << dist;

    QStringList splitted = addr.split('/', QString::SkipEmptyParts);
    QString host	 = splitted.at(0);
    QString dir		 = splitted.at(1);
    
    if (CheckEntry(DIST_SOURCE_HTTP, host))
	return;

    ResourceMedium *cb = new (nothrow) HttpResourceMedium(host, dir);
    if (!cb) {
	qCritical() << "Cannot allocate new content builder";
	return;
    }

    QObject::connect(cb, SIGNAL(DistReady(Dist *)),
		     this, SLOT(DistBuilt(Dist *)), Qt::QueuedConnection);
    _src_db.append(cb);
    cb->start();
}
