#include "ui_display.h"
#include <gui/display/display.h>

#include <solvers/solvers.h>

Display::Display(QWidget *parent) : QMainWindow(parent), ui(new Ui::Display) {
  ui->setupUi(this);
}

Display::~Display() { delete ui; }

QGraphicsScene *Display::scene() { return ui->m_view->scene(); }

void Display::fit() { ui->m_view->fitToItems(); }

void Display::updateData(const DisplayData &data) {
  ui->m_view->scene()->clear();
  for (const auto &rect : data.rectangles) {
    ui->m_view->scene()->addRect(rect.object, rect.pen, rect.brush);
  }
  this->show();
  ui->m_view->fitToItems();
}
