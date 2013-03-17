#include <QDebug>
#include <QList>
#include <QCheckBox>
#include <QThread>
#include "main_window.h"
#include "dist.h"
#include "dist_tree_model.h"

MainWindow::MainWindow()
{
  ui.setupUi(this);
  QObject::connect(ui.pushButton, SIGNAL(clicked()),
      this, SLOT(update_resources(void)));
}

MainWindow::~MainWindow()
{
}

void MainWindow::DistModelUpdate(QAbstractItemModel *model)
{
  _model = model;
  ui.treeView->setModel(model);
  ui.treeView->show();
}

void MainWindow::update_resources()
{
  qDebug() << "Clicked!";
}
