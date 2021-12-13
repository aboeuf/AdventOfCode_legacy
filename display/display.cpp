#include "display.h"
#include "ui_display.h"

Display::Display(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::Display)
{
  ui->setupUi(this);
}

Display::~Display()
{
  delete ui;
}

QGraphicsScene* Display::scene()
{
  return ui->m_view->scene();
}

void Display::fit()
{
  ui->m_view->fitToItems();
}
