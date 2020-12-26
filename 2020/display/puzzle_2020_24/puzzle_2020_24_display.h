#ifndef PUZZLE_2020_24_DISPLAY_H
#define PUZZLE_2020_24_DISPLAY_H

#include <QWidget>

namespace Ui {
class Puzzle_2020_24_Display;
}

namespace puzzle_20 {

}
class Puzzle_2020_24_Display : public QWidget
{
  Q_OBJECT

public:
  explicit Puzzle_2020_24_Display(QWidget *parent = nullptr);
  ~Puzzle_2020_24_Display();

private:
  Ui::Puzzle_2020_24_Display *ui;
};

#endif // PUZZLE_2020_24_DISPLAY_H
