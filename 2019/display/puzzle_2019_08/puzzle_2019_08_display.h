#ifndef PUZZLE_2019_08_DISPLAY_H
#define PUZZLE_2019_08_DISPLAY_H

#include <QWidget>
#include <QGraphicsScene>

namespace Ui {
class Puzzle_2019_08_Display;
}

namespace puzzle_2019_08
{
struct Layer;
}

class Puzzle_2019_08_Display : public QWidget
{
  Q_OBJECT

public:
  explicit Puzzle_2019_08_Display(const puzzle_2019_08::Layer& layer,
                                  QWidget *parent = nullptr);
  ~Puzzle_2019_08_Display();

private:
  Ui::Puzzle_2019_08_Display *ui;
  QGraphicsScene m_scene;
};

#endif // PUZZLE_2019_08_DISPLAY_H
