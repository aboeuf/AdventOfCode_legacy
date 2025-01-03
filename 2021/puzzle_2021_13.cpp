#include <2021/puzzle_2021_13.h>
#include <common.h>
#include <deque>
#include <set>

struct Dot : QPoint {
  Dot(int x, int y) : QPoint(x, y) {}
  bool operator<(const QPoint &other) const {
    if (x() == other.x())
      return y() < other.y();
    return x() < other.x();
  }
};

struct FoldInstruction {
  FoldInstruction(const QString &input) {
    auto rx = QRegExp{"^fold along (x|y)=(\\d+)$"};
    if (rx.exactMatch(input)) {
      m_on_x = rx.cap(1) == "x";
      m_value = rx.cap(2).toInt();
    }
  }
  bool m_on_x{false};
  int m_value{-1};
};

class Paper {
public:
  Paper(const QString &input) {
    const auto lines = common::splitLines(input);
    auto folds = std::deque<QStringList::const_iterator>{};
    for (auto it = std::begin(lines); it != std::end(lines); ++it) {
      if (it->startsWith("fold")) {
        folds.push_back(it);
      } else {
        const auto values = common::toVecUInt(*it);
        if (values.size() == 2u)
          m_dots.insert(Dot(values[0], values[1]));
      }
    }
    m_folds.reserve(folds.size());
    for (auto &it : folds) {
      auto fold = FoldInstruction(*it);
      if (fold.m_value >= 0)
        m_folds.push_back(std::move(fold));
    }
  }

  QString nbDots() const { return QString("%1").arg(m_dots.size()); }

  void display(Display *display) const {
    if (not display)
      return;
    display->scene()->clear();
    QPen pen;
    pen.setWidth(0);
    QBrush brush(QColor("Black"));
    for (const auto &dot : m_dots)
      display->scene()->addRect(dot.x(), -dot.y(), 1.0, 1.0, pen, brush);
    display->show();
    display->fit();
  }

  void applyFolds(uint nb_max = 0) {
    if (nb_max == 0)
      nb_max = static_cast<uint>(m_folds.size());
    for (auto i = 0u; i < nb_max; ++i)
      applyFold(i);
  }

private:
  void applyFold(uint index) {
    if (index < m_folds.size()) {
      const auto &fold = m_folds[index];
      auto new_dots = std::list<Dot>{};
      for (auto it = std::begin(m_dots); it != std::end(m_dots);) {
        if (fold.m_on_x) {
          if (it->x() > fold.m_value) {
            new_dots.push_back(Dot(2 * fold.m_value - it->x(), it->y()));
            it = m_dots.erase(it);
          } else
            ++it;
        } else {
          if (it->y() > fold.m_value) {
            new_dots.push_back(Dot(it->x(), 2 * fold.m_value - it->y()));
            it = m_dots.erase(it);
          } else
            ++it;
        }
      }
      for (const auto &dot : new_dots)
        m_dots.insert(dot);
    }
  }

  std::vector<FoldInstruction> m_folds;
  std::set<Dot> m_dots;
};

void Solver_2021_13_1::solve(const QString &input) {
  Paper paper(input);
  paper.applyFolds(1);
  emit finished(paper.nbDots());
}

void Solver_2021_13_2::solve(const QString &input) {
  if (not this->m_display)
    emit finished("No display");
  Paper paper(input);
  paper.applyFolds();
  paper.display(this->m_display);
  emit finished("");
}
