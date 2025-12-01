#include <limits>
#include <solvers/2016/puzzle_2016_06.h>
#include <solvers/common.h>
#include <solvers/qchar_hash.hpp>

namespace puzzle_2016_06 {

using Int = unsigned long long;

class OccurencesCounter {
public:
  QChar mostCommon() const {
    auto n_max = Int{0};
    auto most_common = QChar('$');
    for (const auto &[c, n] : m_record) {
      if (n > n_max) {
        n_max = n;
        most_common = c;
      }
    }
    return most_common;
  }

  QChar leastCommon() const {
    auto n_min = std::numeric_limits<Int>::max();
    auto least_common = QChar('$');
    for (const auto &[c, n] : m_record) {
      if (n < n_min) {
        n_min = n;
        least_common = c;
      }
    }
    return least_common;
  }

  void record(const QChar &c) {
    ++m_record.insert(std::make_pair(c, Int{0})).first->second;
  }

private:
  std::map<QChar, Int> m_record{};
};

class Messages {
public:
  Messages(const QString &input) {
    const auto messages = common::splitLines(input);
    for (const auto &message : messages) {
      m_max_size = std::max(m_max_size, message.size());
      for (auto i = 0; i < message.size(); ++i) {
        m_counters[i].record(message[i]);
      }
    }
  }

  QString solveOne() const {
    auto result = QString{};
    for (auto i = 0; i < m_max_size; ++i) {
      const auto it = m_counters.find(i);
      result.push_back(it == std::cend(m_counters) ? '#'
                                                   : it->second.mostCommon());
    }
    return result;
  }

  QString solveTwo() const {
    auto result = QString{};
    for (auto i = 0; i < m_max_size; ++i) {
      const auto it = m_counters.find(i);
      result.push_back(it == std::cend(m_counters) ? '#'
                                                   : it->second.leastCommon());
    }
    return result;
  }

private:
  std::unordered_map<int, OccurencesCounter> m_counters;
  int m_max_size{0};
};

} // namespace puzzle_2016_06

void Solver_2016_06_1::solve(const QString &input) {
  const auto messages = puzzle_2016_06::Messages{input};
  emit finished(messages.solveOne());
}

void Solver_2016_06_2::solve(const QString &input) {
  const auto messages = puzzle_2016_06::Messages{input};
  emit finished(messages.solveTwo());
}
