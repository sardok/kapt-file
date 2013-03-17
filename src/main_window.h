#ifndef _MAIN_WINDOW_H
#define _MAIN_WINDOW_H
#include <QMainWindow>
#include <QAbstractItemModel>
#include "ui_mainwindow.h"

class Dist;

class MainWindow: public QMainWindow
{
    Q_OBJECT;

public:
  MainWindow();
  ~MainWindow();

public slots:
  void DistModelUpdate(QAbstractItemModel *);
  void update_resources();
    
private:
  Ui::MainWindow ui;
  QAbstractItemModel *_model;
};
#endif
