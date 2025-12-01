#include "puzzle_2020_24_display.h"
#include "ui_puzzle_2020_24_display.h"
#include <QApplication>
#include <QGraphicsPolygonItem>
#include <cmath>
#include <solvers/2020/puzzle_2020_24.h>
#define M_PI 3.14159265358979323846

Puzzle_2020_24_Display::Puzzle_2020_24_Display(puzzle_2020_24::Lobby *lobby,
                                               QWidget *parent)
    : QWidget(parent), ui(new Ui::Puzzle_2020_24_Display), m_lobby{lobby} {
  ui->setupUi(this);
  connect(this, SIGNAL(advanceDays(uint)), m_lobby, SLOT(advanceDays(uint)),
          Qt::DirectConnection);
  connect(m_lobby, SIGNAL(finished()), this, SLOT(updateDispay()),
          Qt::DirectConnection);
  ui->m_view->setScene(&m_scene);
  updateDispay();
}

Puzzle_2020_24_Display::~Puzzle_2020_24_Display() {
  delete ui;
  delete m_lobby;
}

void Puzzle_2020_24_Display::updateDispay() {
  ui->m_label_day->setText(QString("Day %1").arg(m_day));
  m_scene.clear();
  QPointF u{1, 0}, v{0.5, 0.5 * std::sqrt(3.0)};
  QPolygonF hexagon;
  qreal l = std::sqrt(3.0) / 3.0;
  for (int i = 0; i < 6; ++i) {
    qreal a = M_PI / 6.0 + static_cast<qreal>(i) * M_PI / 3.0;
    hexagon << l * QPointF(std::cos(a), std::sin(a));
  }
  QPen pen(QColor("Grey"));
  pen.setWidthF(0.05);
  int x_min{0}, x_max{0}, y_min{0}, y_max{0};
  qreal d_max{0};
  for (const puzzle_2020_24::Tile &t : m_lobby->m_tiles) {
    x_min = std::min(x_min, t.m_x);
    x_max = std::max(x_max, t.m_x);
    y_min = std::min(y_min, t.m_y);
    y_max = std::max(y_max, t.m_y);
    QPointF p = t.m_x * u + t.m_y * v;
    d_max = std::max(d_max, std::sqrt(p.x() * p.x() + p.y() * p.y()));
  }
  for (int x = x_min; x <= x_max; ++x) {
    for (int y = y_min; y <= y_max; ++y) {
      QPointF p = x * u + y * v;
      if (std::sqrt(p.x() * p.x() + p.y() * p.y()) > d_max)
        continue;
      QGraphicsPolygonItem *tile = m_scene.addPolygon(hexagon);
      tile->setPos(p);
      tile->setPen(pen);
      if (m_lobby->isBlackTile(puzzle_2020_24::Tile{x, y}))
        tile->setBrush(QColor("Black"));
      else
        tile->setBrush(QColor("White"));
    }
  }
  pen.setColor(QColor("Red"));
  m_scene.addLine(0, 0, 0.5 * l, 0, pen);
  pen.setColor(QColor("Green"));
  m_scene.addLine(0, 0, 0, 0.5 * l, pen);
  qApp->processEvents();
  if (m_days_to_advance != 0) {
    --m_days_to_advance;
    ++m_day;
    emit advanceDays(1);
  }
}

void Puzzle_2020_24_Display::on_m_spin_box_nb_days_valueChanged(int arg1) {
  ui->m_label_days->setText(QString("day%1").arg(arg1 > 1 ? "s" : ""));
}

void Puzzle_2020_24_Display::on_m_push_button_advance_clicked() {
  uint days_to_advance = static_cast<uint>(ui->m_spin_box_nb_days->value());
  if (ui->m_check_box_show_advancement->isChecked()) {
    m_days_to_advance = days_to_advance - 1;
    ++m_day;
    emit advanceDays(1);
  } else {
    m_days_to_advance = 0;
    m_day += days_to_advance;
    emit advanceDays(days_to_advance);
  }
}
