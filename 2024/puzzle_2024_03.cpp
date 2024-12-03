#include <2024/puzzle_2024_03.h>
#include <common.h>

namespace puzzle_2024_03 {

class Program {
public:
  Program(const QString &input) : m_raw{input} {}

  QString solveOne() const {
    auto rx = QRegExp{"mul\\((\\d{1,3}),(\\d{1,3})\\)"};
    auto pos = 0;
    auto sum = 0;
    while ((pos = rx.indexIn(m_raw, pos)) != -1) {
      sum += rx.cap(1).toInt() * rx.cap(2).toInt();
      pos += rx.matchedLength();
    }
    return QString("%1").arg(sum);
  }

  QString solveTwo() const {
    auto rx = QRegExp{"mul\\((\\d{1,3}),(\\d{1,3})\\)|do\\(\\)|don't\\(\\)"};
    auto pos = 0;
    auto sum = 0;
    auto enabled = true;
    while ((pos = rx.indexIn(m_raw, pos)) != -1) {
      if (rx.cap(0) == "do()") {
        enabled = true;
      } else if (rx.cap(0) == "don't()") {
        enabled = false;
      } else if (enabled) {
        sum += rx.cap(1).toInt() * rx.cap(2).toInt();
      }
      pos += rx.matchedLength();
    }
    return QString("%1").arg(sum);
  }

private:
  QString m_raw;
};

} // namespace puzzle_2024_03

void Solver_2024_03_1::solve(const QString &input) {
  const auto program = puzzle_2024_03::Program{input};
  emit finished(program.solveOne());
}

void Solver_2024_03_2::solve(const QString &input) {
    const auto program = puzzle_2024_03::Program{input};
    emit finished(program.solveTwo());
}
