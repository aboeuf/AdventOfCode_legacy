#include <2023/puzzle_2023_04.h>
#include <common.h>

namespace puzzle_2023_04 {

inline uint twoPower(const uint power) {
  static auto two_power = QMap<uint, uint>{{0u, 1u}, {1u, 2u}};
  if (not two_power.contains(power))
    two_power[power] = 2u * twoPower(power - 1u);
  return two_power[power];
}

inline uint nbMatchesOnCard(const QString &input) {
  const auto split_index_numbers = common::splitValues(input, ':');
  const auto split_numbers = common::splitValues(split_index_numbers[1], '|');
  auto winning_numbers = common::toUInt(split_numbers[0], ' ');
  auto candidate_numbers = common::toUInt(split_numbers[1], ' ');
  auto nb_matches = 0u;
  for (const auto winning : winning_numbers)
    if (candidate_numbers.contains(winning))
      ++nb_matches;
  return nb_matches;
}

class Cards {
public:
  Cards(const QString &input) {
    const auto lines = common::splitLines(input);
    m_nb_matches_per_card.reserve(lines.size());
    for (const auto &line : lines)
      m_nb_matches_per_card.emplace_back(nbMatchesOnCard(line));
  }

  uint nbPointsV1() const {
    auto nb_points = 0u;
    for (const auto &nb_matches : m_nb_matches_per_card)
      if (nb_matches > 0u)
        nb_points += twoPower(nb_matches - 1u);
    return nb_points;
  }

  uint nbPointsV2() const {
    const auto size = m_nb_matches_per_card.size();
    auto nb_cards = std::vector<uint>{};
    nb_cards.reserve(size);
    for (auto i = 0u; i < size; ++i)
      nb_cards.push_back(1u);
    auto nb_points = 1u;
    for (auto i = 1u; i < size; ++i) {
      for (auto j = i; j < i + m_nb_matches_per_card[i - 1]; ++j)
        nb_cards[j] += nb_cards[i - 1];
      nb_points += nb_cards[i];
    }
    return nb_points;
  }

private:
  std::vector<uint> m_nb_matches_per_card;
};

} // namespace puzzle_2023_04

void Solver_2023_04_1::solve(const QString &input) {
  const auto cards = puzzle_2023_04::Cards(input);
  const auto nb_points = cards.nbPointsV1();
  emit finished(QString("%1").arg(nb_points));
}

void Solver_2023_04_2::solve(const QString &input) {
  const auto cards = puzzle_2023_04::Cards(input);
  const auto nb_points = cards.nbPointsV2();
  emit finished(QString("%1").arg(nb_points));
}
