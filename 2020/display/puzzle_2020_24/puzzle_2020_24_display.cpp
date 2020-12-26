#include "puzzle_2020_24_display.h"
#include "ui_puzzle_2020_24_display.h"

Puzzle_2020_24_Display::Puzzle_2020_24_Display(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Puzzle_2020_24_Display)
{
  ui->setupUi(this);
}

Puzzle_2020_24_Display::~Puzzle_2020_24_Display()
{
  delete ui;
}
