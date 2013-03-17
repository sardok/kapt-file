#include <QDebug>
#include <QDir>
#include <QCheckBox>
#include "dist.h"
#include "apt_pkg.h"

Dist::Dist(Dist *parent):
  _entry(NULL), children(), _name(), _parent(parent),
  _major(false), _enabled(false)
{
}

Dist::~Dist()
{
  if (_entry)
    delete _entry;
  
  qDeleteAll(children);
}

void Dist::set_name(const QString &name)
{
  _name = name;
}

const QString &Dist::get_name(void) const
{
  return _name;
}

void Dist::SetMajor(bool set)
{
  _major = set;
}

bool Dist::is_major() const
{
  return _major;
}

void Dist::set_dist_type(const QString &type)
{
  _type = type;
}

void Dist::set_dist_type(const string &type)
{
  set_dist_type(QString::fromStdString(type));
}

const QString &Dist::get_dist_type(void)
{
  if (_type != "")
    return _type;

  if (_parent)
    _type = _parent->get_dist_type();

  return _type;
}

void Dist::set_arch(const QString &arch)
{
  _arch = arch;
}

void Dist::set_arch(const string &arch)
{
  set_arch(QString::fromStdString(arch));
}

const QString &Dist::get_arch(void) const
{
  return _arch;
}

bool Dist::get_cached() const
{
  return _entry->cached;
}

/*
 * This is the function called by Search signal.
 */
bool Dist::Search(const QString &name)
{
  return false;
}

QString Dist::get_cache_dir(void)
{
  const char *home = getenv("HOME");

  /* Show respect to apt-file */
  QString apt_file_dir = QString("%1/.cache/%2").arg(
      QString(home), QString("apt-file"));
  if (QDir(apt_file_dir).exists())
    return apt_file_dir;

  QString kapt_file_dir = QString("%1/%2").arg(
      QString(home), QString("kapt-file"));
  if (QDir(kapt_file_dir).exists())
    return kapt_file_dir;

  if (QDir().mkdir(kapt_file_dir))
    return kapt_file_dir;

  return QString();
}

void Dist::AddEntry(const QString &host, const QString &path,
		    dist_source_t proto)
{
  if (_entry) {
    qCritical() << "Overriding entry!";
    delete _entry;
  }

  source_entry_t *entry = new source_entry_t();
  entry->cached         = false;
  entry->source_type	= proto;
  entry->host		= host;
  entry->path		= path;

  string arch		= apt_pkg_find("APT::Architecture");
  if (arch.empty()) {
    qCritical() << "Could not determine architecture";
    return;
  }

  QString cache_dir = get_cache_dir();
  if (cache_dir.isEmpty()) {
    qCritical() << "Could not create: " << cache_dir;
    return;
  }
  
  entry->local_content_file = QString("%1/%2_%3_dists_%4_Contents-%5.gz").arg(
      cache_dir, host, get_dist_type(),
      get_name(), QString::fromStdString(arch));
  if(QFile(entry->local_content_file).exists())
    entry->cached = true;
  
  entry->remote_content_file = "undefined";
  qDebug() << get_name() << ":local content file: " << entry->local_content_file;
  _entry = entry;
  //  entries.append(entry);
}

void Dist::Merge(Dist *dst, const Dist *src)
{
  Dist *dptr = NULL;		/* Destination Dst pointer. */
  Dist *sptr = NULL;		/* Source Dst pointer. */

  if (dst->children.size() <= 0 || src->children.size() <= 0) {
    //    Dist::MergeEntries(&dst->entries, &src->entries);
    dst->children += src->children;
  }

  foreach(dptr, dst->children) {
    foreach(sptr, src->children) {
      if (dptr->get_name() == sptr->get_name()) {
	Dist::Merge(dptr, sptr);
        //	Dist::MergeEntries(&dptr->entries, &sptr->entries);
	break;
      }
    }
  }
}

bool Dist::IsChecked() const
{
  return _enabled;
}

void Dist::SetChecked(bool set)
{
  _enabled = set;
}

/*
 * Functions implemented for 'model' begins from here.
 */
void Dist::appendChild(Dist *cnt)
{
  children.append(cnt);
}

Dist *Dist::child(int row)
{
  return children.value(row);
}

int Dist::childCount() const
{
  return children.count();
}

int Dist::row() const
{
  if (_parent)
    return _parent->children.indexOf(const_cast<Dist *> (this));

  return 0;
}

int Dist::columnCount() const 
{
  return 1;
}

Dist *Dist::parent()
{
  return _parent;
}

QString Dist::GetSourceProto(dist_source_t src) const
{
  switch(src) {
  case DIST_SOURCE_INVALID:
    return QString("Invalid");
  case DIST_SOURCE_HTTP:
    return QString("Http");
  case DIST_SOURCE_CDROM:
    return QString("CDRom");
  case DIST_SOURCE_SSH:
    return QString("Ssh");
  case DIST_SOURCE_RSH:
    return QString("Rsh");
  default:
    return QString();
  }
}

void Dist::MergeEntries(QList<source_entry_t> *dst_entries,
			const QList<source_entry_t> *src_entries) 
{
  bool add = true;
  foreach(source_entry_t entry, *src_entries) {
    foreach(source_entry_t check, *dst_entries) {
      if (check.path == entry.path) {
	add = false;
	break;
      }
    }

    if (add)
      dst_entries->append(entry);
  }
}

void Dist::do_cache(source_entry_t *entry)
{
  
}

void Dist::cache(Dist *dist)
{
  if (dist->_enabled)
    dist->do_cache(dist->_entry);
}

void Dist::cache()
{
  do_cache(_entry);

  Dist *child;
  foreach(child, children)
    cache(child);
}

