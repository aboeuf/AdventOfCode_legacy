#include <QScrollBar>
#include <QTimer>
#include <QtMath>
#include <gui/display/view.h>

constexpr auto zoom_factor = 1.0015;

View::View(QWidget *parent)
    : QGraphicsView{new QGraphicsScene{-1.e4, -1.e4, 2.e4, 2.e4}, parent} {
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setInteractive(true);
  setMouseTracking(true);
  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  scale(1., -1);

  QPen pen;
  pen.setWidth(0);
  QBrush brush(QColor("Black"));
  scene()->addRect(0, 0, 1.0, 1.0, pen, brush);

  QTimer::singleShot(200, this, SLOT(fitToItems()));
}

void View::fitToItems() { fitInView(itemsRect(), Qt::KeepAspectRatio); }

void View::mouseDoubleClickEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton)
    fitToItems();
  event->accept();
}

void View::mouseMoveEvent(QMouseEvent *event) {
  if (m_drag_mode_on) {
    const auto delta_viewport = m_viewport_pos - event->pos();
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() +
                                    delta_viewport.x());
    verticalScrollBar()->setValue(verticalScrollBar()->value() +
                                  delta_viewport.y());
  }
  m_viewport_pos = event->pos();
  m_scene_pos = mapToScene(event->pos());
  event->accept();
}

void View::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton)
    m_drag_mode_on = true;
  event->accept();
}

void View::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton)
    m_drag_mode_on = false;
  event->accept();
}

void View::wheelEvent(QWheelEvent *event) {
  const auto angle = event->angleDelta().y();
  if (angle != 0) {
    const auto factor = qPow(zoom_factor, angle);
    scale(factor, factor);
    centerOn(m_scene_pos);
    const auto delta_viewport_pos =
        m_viewport_pos -
        QPointF{viewport()->width() / 2.0, viewport()->height() / 2.0};
    const auto viewport_center = mapFromScene(m_scene_pos) - delta_viewport_pos;
    centerOn(mapToScene(viewport_center.toPoint()));
  }
  event->accept();
}

QRectF View::itemsRect() const {
  auto rect = QRectF{0., 0., 0., 0.};
  for (auto *item : items())
    rect |= item->boundingRect().translated(item->pos());
  return rect;
}
