#include <2023/puzzle_2023_09.h>
#include <common.h>

namespace puzzle_2023_09 {

using Int = long long int;

struct Sequence : public QVector<Int> {
  Sequence() : QVector<Int>{}, all_zero{true} {}

  Sequence(const QString &input)
      : QVector<Int>{common::toVecLongLong(input, ' ')}, all_zero{true} {
    for (auto value : *this) {
      if (value != Int{0}) {
        all_zero = false;
        break;
      }
    }
  }

  Int makeForwardPrediction() const {
    if (all_zero)
      return Int{0};
    return this->back() + derivative().makeForwardPrediction();
  }

  Int makeBackwardPrediction() const {
    if (all_zero)
      return Int{0};
    return this->front() - derivative().makeBackwardPrediction();
  }

  Sequence derivative() const {
    auto result = Sequence{};
    result.reserve(this->size() - 1);
    for (auto i = 0; i + 1 < this->size(); ++i) {
      const auto diff = this->at(i + 1) - this->at(i);
      if (diff != Int{0})
        result.all_zero = false;
      result.push_back(diff);
    }
    return result;
  }

  bool all_zero;
};

class Readings {
public:
  Readings(const QString &input) {
    auto lines = common::splitLines(input);
    m_sequences.reserve(lines.size());
    for (const auto &line : lines)
      m_sequences.emplace_back(line);
  }

  QString solveOne() const {
    auto sum = Int{0};
    for (const auto &sequence : m_sequences)
      sum += sequence.makeForwardPrediction();
    return QString("%1").arg(sum);
  }

  QString solveTwo() const {
    auto sum = Int{0};
    for (const auto &sequence : m_sequences)
      sum += sequence.makeBackwardPrediction();
    return QString("%1").arg(sum);
  }

private:
  std::vector<Sequence> m_sequences;
};

} // namespace puzzle_2023_09

void Solver_2023_09_1::solve(const QString &input) {
  const auto readings = puzzle_2023_09::Readings{input};
  emit finished(readings.solveOne());
}

void Solver_2023_09_2::solve(const QString &input) {
  const auto readings = puzzle_2023_09::Readings{input};
  emit finished(readings.solveTwo());
}
