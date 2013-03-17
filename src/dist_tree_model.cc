#include <QDebug>
#include <QCheckBox>
#include "dist_tree_model.h"
#include "dist.h"

DistTreeModel::DistTreeModel(QObject *parent):
  QAbstractItemModel(parent), _orphaned()
{ }

DistTreeModel::~DistTreeModel()
{ }


QVariant DistTreeModel::data(const QModelIndex &index, int role) const 
{
  if (!index.isValid())
    return QVariant();

  Dist *dist = static_cast<Dist *>(index.internalPointer());
  if (role == Qt::DisplayRole) {
    return column_update(dist, index.column());
  } else if (role == Qt::ToolTipRole) {
    return QVariant();
  } else if (role == Qt::CheckStateRole) {
    if (index.column() != 0)
      /* Only first column is checkable. */
      return QVariant();
	
    if (dist->IsChecked())
      return Qt::Checked;

    return Qt::Unchecked;
  } else if (role == Qt::TextAlignmentRole)
    return column_align(index.column());
    
  return QVariant();
}

Qt::ItemFlags DistTreeModel::flags(const QModelIndex &index) const 
{
  if (!index.isValid())
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}

QVariant DistTreeModel::headerData(int section,
    Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal)
    return QVariant();

  if (role == Qt::DisplayRole) {
    switch(section) {
      case 0:
        return QString("Distribution(s)");
      case 1:
        return QString("Cached");
      default:
        return QVariant();
    }
  }

  if (role == Qt::TextAlignmentRole) {
    switch(section) {
      case 1:
        return Qt::AlignRight;
      default:
        return QVariant();
    }
  }
  return QVariant();
}

QModelIndex DistTreeModel::index(int row, int col,
    const QModelIndex &parent) const
{
  if (!hasIndex(col, row, parent))
    return QModelIndex();

  ResourceItem *parent_item;

  if (!parent.isValid()) {
    parent_item = _children.value(row);
    qDebug() << "parent is not valid, parent_item: " << parent_item;
  } else
    parent_item = static_cast<ResourceItem *> (parent.internalPointer());

  const ResourceItem *item = parent_item->child(row);
  if (item)
    return createIndex(col, row, (void *)item);

  return QModelIndex();
}

QModelIndex DistTreeModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  ResourceItem *child = static_cast<ResourceItem *> (index.internalPointer());
  ResourceItem *parent = child->parent();

  if (child == parent)
    return QModelIndex();
    
  if (!parent)
    return QModelIndex();

  return createIndex(parent->row(), index.column(), parent);
}

int DistTreeModel::rowCount(const QModelIndex &parent_index) const
{
  if (!parent_index.isValid())
    return this->rowCount();

  ResourceItem *parent = static_cast<ResourceItem *>
    (parent_index.internalPointer());
  return parent->childCount();
}

int DistTreeModel::columnCount(const QModelIndex &parent) const
{
  return 2;
}

bool DistTreeModel::setData(const QModelIndex &index,
                            const QVariant &value, int role)
{
  if (role != Qt::CheckStateRole)
    return false;

  if (!index.isValid())
    return false;

  if (index.column() != 0)
    return false;

  bool val = value.toBool();
  Dist *dist = static_cast<Dist *>(index.internalPointer());
  dist->SetChecked(val);
  emit dataChanged(index, index);
  for (int childRow = 0; childRow < rowCount(index); childRow++) {
    QModelIndex child = index.child(childRow, 0);
    setData(child, value, role);
  }

  return true;
}

void DistTreeModel::update_cache(int row, int col, Dist *dist)
{
  dist->cache();
  
}

void DistTreeModel::update_caches()
{
  if (!_data_tree)
    return;

  _data_tree->cache();
}

void DistTreeModel::Merge(Dist *dist)
{
  emit layoutAboutToBeChanged();
  Dist::Merge(_data_tree, dist);
  emit layoutChanged();
}

void DistTreeModel::resource_ready(Dist *dist)
{
  bool happy = false;
  if (!dist->is_major()) {
    /* this is minor distro (such as backports, security etc) */
    QModelIndex tmp = QModelIndex();
    for(int row = 0; row < rowCount(); row++) {
      QModelIndex _parent_index = index(row, 0, tmp);
      if (!_parent_index.isValid())
	continue;
      ResourceItem *_parent = static_cast<ResourceItem *>(_parent_index.internalPointer());
      if (!_parent) {
	qCritical() << "Invalid pointer in model index";
	continue;
      }
      if (_parent->data(0)->get_name().startsWith(dist->get_name())) {
	ResourceItem *item = new ResourceItem(dist, _parent);
        _parent->appendChild(item);
	happy = true;
	break;
      }
    }
  } else {
    
  }
  if (!happy)
    _orphaned += dist;
}

// TODO: Add checkbox to column - 1
QVariant DistTreeModel::column_update(Dist *dist, int column) const
{
  switch(column) {
    case 0:
      return dist->get_name();
    case 1:
      return dist->get_cached() ? Qt::Checked:Qt::Unchecked;
    default:
      return QVariant();
  }
}

QVariant DistTreeModel::column_align(int column) const
{
  switch(column) {
    case 1:
      return Qt::AlignRight;
    default:
      return QVariant();
  }
}

DistTreeModel::ResourceItem::ResourceItem(const Dist *data,
					  ResourceItem *parent)
  : _data(data), _parent(parent)
{
}

DistTreeModel::ResourceItem::~ResourceItem()
{
  if (_data)
    delete _data;
}

const DistTreeModel::ResourceItem *
DistTreeModel::ResourceItem::child(int row) const
{
  return _children.value(row);
}

void DistTreeModel::ResourceItem::appendChild(ResourceItem *item)
{
  _children += item;
}

int DistTreeModel::ResourceItem::childCount() const
{
  return _children.count();
}

int DistTreeModel::ResourceItem::columnCount() const
{
  return 1;
}

const Dist *DistTreeModel::ResourceItem::data(int col) const
{
  return _data;
}

int DistTreeModel::ResourceItem::row() const
{
  if (_parent)
    return _parent->_children.indexOf(const_cast<ResourceItem *>(this));

  return 0;
}

DistTreeModel::ResourceItem *DistTreeModel::ResourceItem::parent() const
{
  return _parent;
}
