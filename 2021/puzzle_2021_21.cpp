#include <2021/puzzle_2021_21.h>
#include <common.h>

namespace puzzle_2021_21
{

using Int = unsigned long long int;

constexpr auto nb_per_position_increments = std::array<Int, 7>{1, 3, 6, 7, 6, 3, 1};

class DeterministicDice
{
public:
  DeterministicDice() {}

  Int nbRolls() const { return m_nb_rolls; }

  Int roll()
  {
    ++m_nb_rolls;
    auto res = m_value;
    ++m_value;
    if (m_value > 1000u)
      m_value = 1u;
    return res;
  }

private:
  Int m_value{1u};
  Int m_nb_rolls{0u};
};

struct Player
{
  Player() = default;
  Player(Int position) : m_position{position} {}
  Player(Int position, Int score) : m_position{position}, m_score{score} {}
  Player(const Player& other) : m_position{other.m_position}, m_score{other.m_score} {}

  bool operator < (const Player& other) const
  {
    if (m_position == other.m_position)
      return m_score < other.m_score;
    return m_position < other.m_position;
  }

  bool operator == (const Player& other) const
  {
    return m_position == other.m_position and m_score == other.m_score;
  }

  Int m_position{0};
  Int m_score{0};
};

struct State : public std::array<Player, 2>
{
  bool operator < (const State& other) const
  {
    if ((*this)[0] == other[0])
      return (*this)[1] < other[1];
    return (*this)[0] < other[0];
  }
};

bool parseInput(const QString& input, std::array<Player, 2>& players)
{
  const auto lines = common::splitLines(input);
  if (lines.size() < 2)
    return false;
  bool ok;
  auto rx = QRegExp("^Player (\\d+) starting position: (\\d+)$");
  for (auto i = 0u; i < 2u; ++i) {
    if (rx.exactMatch(lines[i])) {
      const auto position = rx.cap(2).toULongLong(&ok);
      if (not ok)
        return false;
      if (position > 0) {
        const auto player_id = rx.cap(1).toUInt(&ok);
        if (ok and player_id != 0 and player_id - 1 == i) {
          players[i].m_position = (position - 1) % 10;
          players[i].m_score = 0;
        }
        else
          return false;
      } else
        return false;
    } else
      return false;
  }
  return true;
}

class DeterministicDiracDiceGame
{
public:
  DeterministicDiracDiceGame(const QString& input)
  {
    if (not parseInput(input, m_players)) {
      m_result = "Invalid input";
      return;
    }
    auto dice = DeterministicDice{};
    auto player_id = 0;
    for (;;) {
      auto& player = m_players[player_id];
      for (auto i = 0u; i < 3u; ++i)
        player.m_position += dice.roll();
      player.m_position = player.m_position % 10;
      player.m_score += player.m_position + 1;
      if (player.m_score >= 1000u)
        break;
      player_id = (player_id + 1) % 2;
    }
    m_result = QString("%1").arg(std::min(m_players[0].m_score, m_players[1].m_score) * dice.nbRolls());
  }

  const QString& result() { return m_result; }

private:
  std::array<Player, 2> m_players{};
  QString m_result{};
};

class QuantumDiracDiceGame
{
public:
  QuantumDiracDiceGame(const QString& input)
  {
    auto initial_state = State{};
    if (not parseInput(input, initial_state))
      return;
    auto states = std::map<State, Int>{{initial_state, 1}};
    auto active_player_id = 0u;
    auto nb_wins = std::array<Int, 2>{};
    while (not states.empty()) {
      const auto passive_player_id = (active_player_id + 1) % 2;
      auto new_states = std::map<State, Int>{};
      for (auto it = std::cbegin(states); it != std::cend(states); ++it) {
        const auto& active_player = it->first[active_player_id];
        const auto& passive_player = it->first[passive_player_id];
        for (int i = 0; i < 7; ++i) {
          const auto next_pos = (active_player.m_position + i + 3) % 10;
          const auto next_score = active_player.m_score + next_pos + 1;
          const auto nb_spawn = nb_per_position_increments[i] * it->second;
          if (next_score < 21) {
            auto next_state = State();
            next_state[active_player_id].m_position = next_pos;
            next_state[active_player_id].m_score = next_score;
            next_state[passive_player_id].m_position = passive_player.m_position;
            next_state[passive_player_id].m_score = passive_player.m_score;
            auto& nb_states = new_states.insert(std::make_pair(next_state, Int(0))).first->second;
            nb_states += nb_spawn;
          }
          else
            nb_wins[active_player_id] += nb_spawn;
        }
      }
      active_player_id = passive_player_id;
      std::swap(states, new_states);
    }
    m_result = QString("%1").arg(std::max(nb_wins[0], nb_wins[1]));
  }

  const QString& result() { return m_result; }

private:
  QString m_result{"Invalid input"};
};

}

void Solver_2021_21_1::solve(const QString& input)
{
  emit finished(puzzle_2021_21::DeterministicDiracDiceGame(input).result());
}

void Solver_2021_21_2::solve(const QString& input)
{
  emit finished(puzzle_2021_21::QuantumDiracDiceGame(input).result());
}
