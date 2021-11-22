#ifndef PUZZLE_2018_06_DISPLAY_H
#define PUZZLE_2018_06_DISPLAY_H

#include <QWidget>
#include <QGraphicsScene>

namespace Ui {
class Puzzle_2018_06_Display;
}

namespace puzzle_2018_06 {
class VoronoiDiagram;
}

class Puzzle_2018_06_Display : public QWidget
{
  Q_OBJECT

public:
  explicit Puzzle_2018_06_Display(const puzzle_2018_06::VoronoiDiagram& diagram, QWidget *parent = nullptr);
  ~Puzzle_2018_06_Display();

private:
  Ui::Puzzle_2018_06_Display *ui;
  QGraphicsScene m_scene;
};

#endif // PUZZLE_2018_06_DISPLAY_H
