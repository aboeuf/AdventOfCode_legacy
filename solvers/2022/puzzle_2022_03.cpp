#include <QHash>
#include <solvers/common.h>
#include <solvers/2022/puzzle_2022_03.h>

namespace puzzle_2022_03 {

using Int = long long unsigned int;

class PrioritiesComputeur {
public:
  PrioritiesComputeur() {
    const auto items =
        QString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    auto curr_item = std::begin(items);
    for (Int i = 1; i < 53; ++i) {
      m_priorities.insert(*curr_item, i);
      ++curr_item;
    }
  }

  Int operator()(const QChar &c) const {
    if (m_priorities.contains(c))
      return m_priorities.value(c);
    return Int{0};
  }

private:
  QHash<QChar, Int> m_priorities;
};

const auto priority = PrioritiesComputeur{};

class Rucksack {
public:
  Rucksack(const QString &input) : m_contents{input} {
    const auto half_size = input.size() / 2;
    const auto first_compartment = input.mid(0, half_size);
    const auto second_compartment = input.mid(half_size);

    if (first_compartment.size() != second_compartment.size() or
        first_compartment.isEmpty())
      return;

    for (const auto &c1 : first_compartment) {
      for (const auto &c2 : second_compartment) {
        if (c1 == c2) {
          m_common_item = c1;
          return;
        }
      }
    }
  }

  const QString &contents() const { return m_contents; }
  bool isValid() const { return m_common_item != QChar{'$'}; }
  Int commonItemPriority() const { return priority(m_common_item); }

private:
  QString m_contents;
  QChar m_common_item{'$'};
};

class Rucksacks {
public:
  Rucksacks(const QString &input) {
    const auto lines = common::splitLines(input);
    m_sacks.reserve(lines.size());
    for (const auto &line : lines)
      if (not line.isEmpty())
        m_sacks.emplace_back(line);
  }

  bool isValid() const {
    return m_sacks.size() % 3 == 0 and
           std::all_of(std::begin(m_sacks), std::end(m_sacks),
                       [](const auto &sack) { return sack.isValid(); });
  }

  Int checkSumPuzzleOne() const {
    auto sum = Int{0};
    for (const auto &sack : m_sacks)
      sum += sack.commonItemPriority();
    return sum;
  }

  Int checkSumPuzzleTwo() const {
    auto sum = Int{0};
    for (auto i = 0u; i < m_sacks.size() / 3; ++i) {
      const auto badge_item = getBadgeItem(i);
      sum += priority(badge_item);
    }
    return sum;
  }

private:
  QChar getBadgeItem(std::size_t group_index) const {
    if (m_sacks.size() < 3 * group_index + 2)
      return QChar('@');
    for (const auto &c1 : m_sacks[3 * group_index].contents()) {
      for (const auto &c2 : m_sacks[3 * group_index + 1].contents()) {
        if (c1 != c2)
          continue;
        for (const auto &c3 : m_sacks[3 * group_index + 2].contents()) {
          if (c1 == c3)
            return c1;
        }
      }
    }
    return QChar{'$'};
  }

  std::vector<Rucksack> m_sacks;
};

} // namespace puzzle_2022_03

void Solver_2022_03_1::solve(const QString &input) {
  const auto sacks = puzzle_2022_03::Rucksacks(input);
  emit output(
      QString("Input is %1").arg(sacks.isValid() ? "valid" : "invalid"));
  emit finished(QString("%1").arg(sacks.checkSumPuzzleOne()));
}

void Solver_2022_03_2::solve(const QString &input) {
  const auto sacks = puzzle_2022_03::Rucksacks(input);
  emit output(
      QString("Input is %1").arg(sacks.isValid() ? "valid" : "invalid"));
  emit finished(QString("%1").arg(sacks.checkSumPuzzleTwo()));
}
