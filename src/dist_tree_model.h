#ifndef _DIST_TREE_MODEL_H
#define _DIST_TREE_MODEL_H
#include <iostream>
#include <QAbstractItemModel>

using std::ostream;

/* Opaque definition. */
class Dist;

class DistTreeModel: public QAbstractItemModel
{
public:
  DistTreeModel(QObject *parent = NULL);
  ~DistTreeModel();

  /* Model specific functions. */
  QVariant data(const QModelIndex &index, int role) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation,
      int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int col,
      const QModelIndex &parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex &index) const;
  int rowCount(const QModelIndex &parent_count = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  bool setData(const QModelIndex &index,
      const QVariant &val, int role = Qt::EditRole);

  /* Custom functions */
  void update_caches();
  void Merge(Dist *dist);

public slots:
  void resource_ready(Dist *dist);
  
private:
  struct ResourceItem {
    ResourceItem(const Dist *data, ResourceItem *parent = NULL);
    ~ResourceItem();
    const ResourceItem *child(int row) const;
    void appendChild(ResourceItem *item);
    int childCount() const;
    int columnCount() const;
    const Dist *data(int col) const;
    int row() const;
    ResourceItem *parent() const;
  private:
    QList<const ResourceItem *> _children;
    const Dist *_data;
    ResourceItem *_parent;
  };
    
  QVariant column_update(Dist *dist, int col) const;
  QVariant column_align(int col) const;
  void update_cache(int row, int col, Dist *dist);
  Dist *_data_tree;
  QList<Dist *> _orphaned;
  QList<ResourceItem *> _children;
};
#endif
