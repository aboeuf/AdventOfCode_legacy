#include <2017/puzzle_2017_09.h>
#include <common.h>

namespace puzzle_2017_09 {

class Line {
public:
  Line(const QString &input) : m_line{input} {}

private:
  QString m_line;
};

class Lines {
public:
  Lines(const QString &input) {
    auto lines = common::splitLines(input);
    lines.reserve(lines.size());
    for (const auto &line : lines)
      m_lines.emplace_back(line);
  }

  QString solveOne() const { return "Default"; }
  QString solveTwo() const { return "Default"; }

private:
  std::vector<Line> m_lines;
};

} // namespace puzzle_2017_09

void Solver_2017_09_1::solve(const QString &input) {
  emit output(input);
  const auto lines = puzzle_2017_09::Lines{input};
  emit finished(lines.solveOne());
}

void Solver_2017_09_2::solve(const QString &input) {
  emit output(input);
  const auto lines = puzzle_2017_09::Lines{input};
  emit finished(lines.solveTwo());
}
