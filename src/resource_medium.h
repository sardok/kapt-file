#ifndef _RESOURCE_MEDIUM_H
#define _RESOURCE_MEDIUM_H
#include <QThread>
#include "dist.h"

class ResourceMedium: public QThread
{
public:
  ResourceMedium(dist_source_t proto, const QString &addr,
		 const QString &dist, const QString &path);
  virtual ~ResourceMedium();
  const QString &get_addr(void) const;
  const QString &get_dist(void) const;
  const QString &get_path(void) const;
  dist_source_t get_proto(void) const;

protected:
  dist_source_t _proto;
  QString _addr;
  QString _dist;
  QString _path;
};
#endif
