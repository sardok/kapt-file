#ifndef _RESOURCE_MANAGER_H
#define _RESOURCE_MANAGER_H
#include <QString>
#include <QList>
#include <QFile>

#include "dist.h"

/* Opaque definitions. */
class Dist;
class QAbstractItemModel;
class ResourceMedium;

class ResourceManager: public QObject
{
  Q_OBJECT;
    
public:
  ResourceManager();
  ~ResourceManager();
  void ParseResources(void);
    
signals:
  void ModelUpdate(QAbstractItemModel *);
  void resource_ready(Dist *new_dist);
  
private slots:
  void DistBuilt(Dist *);

private:
  void AptHTTPSourceParser(QString &srv_path);
  void AptCDROMSourceParser(QString &path);
  bool CheckEntry(dist_source_t type, const QString &addr);
  QFile _sourcelist;
  QList<ResourceMedium *> _src_db;
  QAbstractItemModel *_model;
};
#endif /* _RESOURCE_MANAGER_H */
