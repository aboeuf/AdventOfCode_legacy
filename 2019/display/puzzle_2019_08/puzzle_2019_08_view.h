#pragma once

#include <QFrame>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QWheelEvent>

class Puzzle_2019_08_View;
class QGraphicsScene;

class Puzzle_2019_08_GraphicsView : public QGraphicsView
{
  Q_OBJECT

public:
  Puzzle_2019_08_GraphicsView(Puzzle_2019_08_View* view);

protected:
  void wheelEvent(QWheelEvent*) override;

private:
  Puzzle_2019_08_View* m_view;
};

class Puzzle_2019_08_View : public QWidget
{
  Q_OBJECT

public:
  explicit Puzzle_2019_08_View(QWidget *parent = 0);

  void setScene(QGraphicsScene* scene);
  void zoomIn(int level = 1);
  void zoomOut(int level = 1);
  void setDragMode(bool on);

private:
  void setupMatrix();

  Puzzle_2019_08_GraphicsView* m_graphics_view;
  int m_zoom_level;
  bool m_drag_mode_on;
};
