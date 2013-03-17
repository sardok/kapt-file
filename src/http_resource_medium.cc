#include <sys/types.h>
#include <QDebug>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QThread>
#include <QRegExp>
#include <QMap>
#include <QString>
#include "http_resource_medium.h"

using std::nothrow;

HttpResourceMedium::HttpResourceMedium(const QString &addr, const QString &dist):
  ResourceMedium(DIST_SOURCE_HTTP, addr, dist,
      QString("%1/%2").arg(dist, "dists")), _mng(NULL)
{
    _root = new Dist;
    _root->set_name("Root");
    _root->set_dist_type(_dist);
}

HttpResourceMedium::~HttpResourceMedium()
{
    delete _mng;
    _mng = NULL;
}

void HttpResourceMedium::run(void)
{

    if (!_mng) {
	_mng = new QNetworkAccessManager();
	connect(_mng, SIGNAL(finished(QNetworkReply *)),
		this, SLOT(ReplyCallback(QNetworkReply *)));
    }

    _mng->get(QNetworkRequest(QUrl(
		QString("http://%1/%2/").arg(get_addr(), get_path()))));
    exec();
}

void HttpResourceMedium::ErrorCallback(QNetworkReply::NetworkError err)
{
    qDebug() << "Error is occured[" << err;
}

void HttpResourceMedium::ReplyCallback(QNetworkReply *reply)
{
  QRegExp pattern("^<tr>.*<img .*alt=\"\\[DIR\\]\".*>.*"\
		  "<a href=\"(/|)([\\x21-\\x7E]*)/\">.*$");

  while (!reply->atEnd()) {
    QByteArray line = reply->readLine();
    if (!pattern.exactMatch(line))
      continue;
	
    QString link = pattern.cap(2);
    qDebug() << "incoming link: " << link << ", getdist: " << get_dist();
    if (link == get_dist())
      /* Catch parent directory link. */
      continue;

    Dist *dst = new (nothrow) Dist();
    if (!dst) {
      qCritical() << "Cannot allocate memory!";
      continue;
    }
    
    /* Check if this is a major distro. */
    QRegExp minor_dist_pattern("(\\w*)-(\\w*)");
    if (!minor_dist_pattern.exactMatch(link))
      dst->SetMajor(true);
    
    dst->set_name(link);
    dst->AddEntry(get_addr(), get_path(), DIST_SOURCE_HTTP);
    emit DistReady(dst);
  }

  if (!reply->isFinished())
    qDebug() << "Reply is not finished ?!?";

  /* Terminate thread. */
  exit();
}
