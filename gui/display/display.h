#pragma once

#include <QGraphicsScene>
#include <QMainWindow>

struct DisplayData;

namespace Ui {
class Display;
}

class Display : public QMainWindow {
  Q_OBJECT

public:
  explicit Display(QWidget *parent = nullptr);
  ~Display();

  QGraphicsScene *scene();
  void fit();

  void updateData(const DisplayData &data);

private:
  Ui::Display *ui;
};
