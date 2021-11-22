#include "puzzle_2018_06_display.h"
#include "ui_puzzle_2018_06_display.h"
#include <2018/puzzle_2018_06.h>

Puzzle_2018_06_Display::Puzzle_2018_06_Display(const puzzle_2018_06::VoronoiDiagram &diagram, QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Puzzle_2018_06_Display)
{
  ui->setupUi(this);
  ui->m_view->setScene(&m_scene);

  for (auto x = diagram.xMin() - 25; x <= diagram.xMax() + 25; ++x) {
    for (auto y = diagram.xMin() - 25; y <= diagram.yMax() + 25; ++y) {
      const auto closests = diagram.closestPoints(x, y);
      if (closests.size() == 1) {
        const auto col = static_cast<double>(closests[0]) / static_cast<double>(diagram.points().size() - 1);
        const auto color = QColor::fromHslF(col, 0.95, 0.5);
        m_scene.addRect(static_cast<double>(x) - 0.5, static_cast<double>(y) - 0.5, 1.0, 1.0, QPen{Qt::NoPen}, QBrush{color});
      }
    }
  }

  QPen pen;
  pen.setWidth(0);
  pen.setColor(QColor("Black"));
  for (const auto& pt : diagram.points()) {
    const auto x = static_cast<double>(pt.x());
    const auto y = static_cast<double>(pt.y());
    m_scene.addRect(x - 0.5, y - 0.5, 1.0, 1.0, pen, QBrush{Qt::NoBrush});
  }
  m_scene.addRect(diagram.xMin(), diagram.yMin(),
                  diagram.xMax() - diagram.xMin(),
                  diagram.yMax() - diagram.yMin(), pen, QBrush{Qt::NoBrush});
}

Puzzle_2018_06_Display::~Puzzle_2018_06_Display()
{
  delete ui;
}
