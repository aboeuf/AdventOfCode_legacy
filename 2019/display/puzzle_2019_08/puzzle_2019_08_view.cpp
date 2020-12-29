#include "puzzle_2019_08_view.h"
#include <QtMath>
#include <QGridLayout>
#include <QDebug>

Puzzle_2019_08_GraphicsView::Puzzle_2019_08_GraphicsView(Puzzle_2019_08_View* view) : QGraphicsView(), m_view{view} { }

void Puzzle_2019_08_GraphicsView::wheelEvent(QWheelEvent *e)
{
  if (e->modifiers() & Qt::ControlModifier) {
    if (e->delta() > 0)
      m_view->zoomIn(6);
    else
      m_view->zoomOut(6);
    e->accept();
  } else {
    QGraphicsView::wheelEvent(e);
  }
}

Puzzle_2019_08_View::Puzzle_2019_08_View(QWidget* parent) : QWidget(parent), m_zoom_level{428}, m_drag_mode_on{false}
{
  m_graphics_view = new Puzzle_2019_08_GraphicsView(this);
  m_graphics_view->setRenderHint(QPainter::Antialiasing, true);
  m_graphics_view->setDragMode(QGraphicsView::RubberBandDrag);
  m_graphics_view->setOptimizationFlags(QGraphicsView::DontSavePainterState);
  m_graphics_view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
  m_graphics_view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  QGridLayout *layout = new QGridLayout;
  layout->addWidget(m_graphics_view, 0, 0);
  setLayout(layout);

  setupMatrix();
  m_graphics_view->ensureVisible(QRectF(0, 0, 0, 0));
}

void Puzzle_2019_08_View::setScene(QGraphicsScene* scene)
{
  m_graphics_view->setScene(scene);
}

void Puzzle_2019_08_View::zoomIn(int level)
{
  m_zoom_level += level;
  setupMatrix();
}

void Puzzle_2019_08_View::zoomOut(int level)
{
  m_zoom_level -= level;
  setupMatrix();
}

void Puzzle_2019_08_View::setDragMode(bool on)
{
  m_drag_mode_on = on;
  m_graphics_view->setDragMode(m_drag_mode_on ? QGraphicsView::ScrollHandDrag : QGraphicsView::RubberBandDrag);
  m_graphics_view->setInteractive(m_drag_mode_on);
}

void Puzzle_2019_08_View::setupMatrix()
{
  qreal scale = qPow(qreal(2), (m_zoom_level - 250) / qreal(50));
  QMatrix matrix;
  matrix.scale(scale, -scale);
  m_graphics_view->setMatrix(matrix);
}
