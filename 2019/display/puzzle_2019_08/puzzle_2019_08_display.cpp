#include "puzzle_2019_08_display.h"
#include "ui_puzzle_2019_08_display.h"
#include <2019/puzzle_2019_08.h>

Puzzle_2019_08_Display::Puzzle_2019_08_Display(const puzzle_2019_08::Layer& layer, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Puzzle_2019_08_Display)
{
  ui->setupUi(this);
  ui->m_view->setScene(&m_scene);
  QPen pen;
  pen.setWidthF(0.01);
  QBrush brush(QColor("Black"));
  for (int i = 0; i < layer.size(); ++i) {
    for (int j = 0; j < layer[i].size(); ++j) {
      if (layer[i][j] == 1) {
        m_scene.addRect(j, -i, 1.0, 1.0, pen, brush);
      }
    }
  }
}

Puzzle_2019_08_Display::~Puzzle_2019_08_Display()
{
  delete ui;
}
