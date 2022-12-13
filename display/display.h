#pragma once

#include <QMainWindow>
#include <QGraphicsScene>

namespace Ui {
class Display;
}

class Display : public QMainWindow
{
  Q_OBJECT

public:
  explicit Display(QWidget *parent = nullptr);
  ~Display();

  QGraphicsScene* scene();
  void fit();

private:
  Ui::Display *ui;
};
