#include <QObject>
#include <QApplication>
#include <QDebug>
#include <QMetaType>
#include <QThread>
#include "exceptions.h"
#include "resource_manager.h"
#include "dist.h"
#include "main_window.h"
#include "dist_tree_model.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  qRegisterMetaType<Dist>("Dist");
  ResourceManager res;
  DistTreeModel model;
  MainWindow window;

  // QObject::connect(&res, SIGNAL(ModelUpdate(QAbstractItemModel *)),
  // 		   &window, SLOT(DistModelUpdate(QAbstractItemModel *)));

  QObject::connect(&res, SIGNAL(resource_ready(Dist *)),
                   &model, SLOT(resource_ready(Dist *)));

  res.ParseResources();
  window.show();
  return app.exec();
}
