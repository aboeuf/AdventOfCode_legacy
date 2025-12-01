#pragma once

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QWheelEvent>

class View : public QGraphicsView {
  Q_OBJECT

public:
  explicit View(QWidget *parent = nullptr);

public slots:
  void fitToItems();

protected:
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

private:
  QRectF itemsRect() const;
  bool m_drag_mode_on{false};
  bool m_right_click_available{false};
  QRectF m_view_rect;
  QPointF m_scene_pos;
  QPoint m_viewport_pos;
};
