#include <2015/puzzle_2015_02.h>
#include <common.h>

using Int = unsigned long long int;

struct Box
{
  Box(const QString& input) :
    m_dimensions{common::toULongLongIntValues(input, 'x')} {
    std::sort(std::begin(m_dimensions), std::end(m_dimensions));
  }

  Int wrappingSurface() const
  {
    auto smallest_side = std::numeric_limits<Int>::max();
    auto sum = Int{0};
    for (auto i = 0; i < m_dimensions.size(); ++i) {
      for (auto j = i + 1; j < m_dimensions.size(); ++j) {
        const auto surface = m_dimensions[i] * m_dimensions[j];
        smallest_side = std::min(smallest_side, surface);
        sum += surface;
      }
    }
    return Int{2} * sum + smallest_side;
  }

  Int ribbonLength() const
  {
    return 2 * (m_dimensions[0] + m_dimensions[1]) +
        m_dimensions[0] * m_dimensions[1] * m_dimensions[2];
  }

  QVector<Int> m_dimensions;
};

struct Boxes
{
  Boxes(const QString& input) {
    const auto lines = common::splitLines(input);
    m_boxes.reserve(lines.size());
    for (const auto& line : lines)
      m_boxes.emplace_back(line);
  }

  QString solvePuzzleOne() const
  {
    auto sum = Int{0};
    for (const auto& box : m_boxes)
      sum += box.wrappingSurface();
    return QString("%1").arg(sum);
  }

  QString solvePuzzleTwo() const
  {
    auto sum = Int{0};
    for (const auto& box : m_boxes)
      sum += box.ribbonLength();
    return QString("%1").arg(sum);
  }

  std::vector<Box> m_boxes;
};

void Solver_2015_02_1::solve(const QString& input)
{
  emit finished(Boxes(input).solvePuzzleOne());
}

void Solver_2015_02_2::solve(const QString& input)
{
  emit finished(Boxes(input).solvePuzzleTwo());
}

