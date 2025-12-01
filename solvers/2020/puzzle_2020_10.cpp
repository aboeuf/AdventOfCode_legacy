#include <QVector>
#include <algorithm>
#include <solvers/2020/puzzle_2020_10.h>
#include <solvers/common.h>

namespace puzzle_2020_10 {

struct AdapterChain : public QVector<int> {
  using Int = unsigned long long int;

  AdapterChain(const QString &input, int max_jump, int min_jump = 0u)
      : m_min_jump{min_jump}, m_max_jump{max_jump} {
    const QStringList lines = common::splitLines(input);
    QVector<int> data;
    data.reserve(lines.size());
    bool ok;
    for (const QString line : lines) {
      int value = line.toInt(&ok);
      if (ok)
        data << value;
    }
    std::sort(data.begin(), data.end());
    this->reserve(data.size() + 2);
    *this << 0;
    for (int i : data) {
      const int diff = i - this->back();
      if (m_min_jump <= diff && diff <= m_max_jump)
        *this << i;
    }
    *this << this->back() + m_max_jump;
  }

  QVector<int> getJumpsCounts() const {
    QVector<int> counts(m_max_jump + 1, 0);
    for (auto it = std::next(this->begin()); it != this->end(); ++it)
      ++counts[*it - *std::prev(it)];
    return counts;
  }

  Int getNbArrangements() const {
    if (this->empty())
      return Int{0u};
    QVector<Int> nb_arrangements(this->size(), Int{0u});
    nb_arrangements.back() = Int{1u};
    for (int i = nb_arrangements.size() - 2; i >= 0; --i) {
      int nb_children = 0;
      for (int j = 1; j <= m_max_jump && i + j < this->size(); ++j) {
        const int diff = this->at(i + j) - this->at(i);
        if (m_min_jump <= diff && diff <= m_max_jump)
          ++nb_children;
      }
      for (int j = 1; j <= nb_children; ++j)
        nb_arrangements[i] += nb_arrangements[i + j];
    }
    return nb_arrangements[0];
  }

  int m_min_jump{0};
  int m_max_jump{0};
};

} // namespace puzzle_2020_10

void Solver_2020_10_1::solve(const QString &input) {
  using namespace puzzle_2020_10;
  const AdapterChain chain(input, 3);
  const QVector<int> counts = chain.getJumpsCounts();
  emit finished(QString::number(counts[1] * counts[3]));
}

void Solver_2020_10_2::solve(const QString &input) {
  using namespace puzzle_2020_10;
  const AdapterChain chain(input, 3);
  emit finished(QString::number(chain.getNbArrangements()));
}
