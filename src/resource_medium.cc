#include <QString>
#include <QThread>
#include "resource_medium.h"

ResourceMedium::ResourceMedium(dist_source_t proto, const QString &addr,
			       const QString &dist, const QString &path):
  QThread(), _proto(proto), _addr(addr), _dist(dist), _path(path)
{ }

ResourceMedium::~ResourceMedium()
{ }

const QString &ResourceMedium::get_addr(void) const
{
  return _addr;
}

const QString &ResourceMedium::get_dist(void) const
{
  return _dist;
}

const QString &ResourceMedium::get_path(void) const
{
  return _path;
}

dist_source_t ResourceMedium::get_proto(void) const
{
  return _proto;
}
