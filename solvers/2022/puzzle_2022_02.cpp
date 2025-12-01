#include <solvers/2022/puzzle_2022_02.h>
#include <solvers/common.h>

namespace puzzle_2022_02 {

using Int = long long unsigned int;

enum class HandShape { ERROR = 0u, ROCK = 1u, PAPER = 2u, SCISSORS = 3u };

inline Int getHandShapeValue(const HandShape &shape) {
  return static_cast<Int>(shape);
}

enum class Outcome { ERROR = 0u, LOOSE = 1u, DRAW = 2u, WIN = 3u };

inline Int getOutcomeValue(const Outcome &outcome) {
  return static_cast<Int>(outcome);
}

inline Int getOutcomeReward(const Outcome &outcome) {
  const auto value = getOutcomeValue(outcome);
  if (value == 0u)
    return 0u;
  return 3u * (value - 1u);
}

const auto outcome_matrix = std::array<std::array<Outcome, 4>, 4>{
    std::array<Outcome, 4>{Outcome::ERROR, Outcome::ERROR, Outcome::ERROR,
                           Outcome::ERROR},
    std::array<Outcome, 4>{Outcome::ERROR, Outcome::DRAW, Outcome::WIN,
                           Outcome::LOOSE},
    std::array<Outcome, 4>{Outcome::ERROR, Outcome::LOOSE, Outcome::DRAW,
                           Outcome::WIN},
    std::array<Outcome, 4>{Outcome::ERROR, Outcome::WIN, Outcome::LOOSE,
                           Outcome::DRAW}};

const auto hand_shape_matrix = std::array<std::array<HandShape, 4>, 4>{
    std::array<HandShape, 4>{HandShape::ERROR, HandShape::ERROR,
                             HandShape::ERROR, HandShape::ERROR},
    std::array<HandShape, 4>{HandShape::ERROR, HandShape::SCISSORS,
                             HandShape::ROCK, HandShape::PAPER},
    std::array<HandShape, 4>{HandShape::ERROR, HandShape::ROCK,
                             HandShape::PAPER, HandShape::SCISSORS},
    std::array<HandShape, 4>{HandShape::ERROR, HandShape::PAPER,
                             HandShape::SCISSORS, HandShape::ROCK}};

inline HandShape getHandShapeFromString(const QString &input) {
  if (input == QString("A") or input == QString("X"))
    return HandShape::ROCK;
  if (input == QString("B") or input == QString("Y"))
    return HandShape::PAPER;
  if (input == QString("C") or input == QString("Z"))
    return HandShape::SCISSORS;
  return HandShape::ERROR;
}

inline Outcome getOutcomeFromString(const QString &input) {
  if (input == QString("X"))
    return Outcome::LOOSE;
  if (input == QString("Y"))
    return Outcome::DRAW;
  if (input == QString("Z"))
    return Outcome::WIN;
  return Outcome::ERROR;
}

class Round {

public:
  Round(const QString &input, bool first_strategy) {
    const auto values = common::splitValues(input, QChar(' '));
    if (values.size() != 2)
      return;
    m_opponent_hand_shape = getHandShapeFromString(values[0]);
    if (first_strategy) {
      m_self_hand_shape = getHandShapeFromString(values[1]);
    } else {
      const auto i = getHandShapeValue(m_opponent_hand_shape);
      const auto j = getOutcomeValue(getOutcomeFromString(values[1]));
      m_self_hand_shape = hand_shape_matrix[i][j];
    }
  }

  HandShape opponentHandShape() const { return m_opponent_hand_shape; }
  HandShape selfHandShape() const { return m_self_hand_shape; }

  bool isValid() const {
    return m_opponent_hand_shape != HandShape::ERROR and
           m_self_hand_shape != HandShape::ERROR;
  }

  Outcome getOutcome() const {
    const auto i = getHandShapeValue(m_opponent_hand_shape);
    const auto j = getHandShapeValue(m_self_hand_shape);
    return outcome_matrix[i][j];
  }

  Int getValue() const {
    return getHandShapeValue(m_self_hand_shape) +
           getOutcomeReward(getOutcome());
  }

private:
  bool m_valid{false};
  HandShape m_opponent_hand_shape{HandShape::ERROR};
  HandShape m_self_hand_shape{HandShape::ERROR};
  Int m_value{0};
};

class Game {

public:
  Game(const QString &input, bool first_strategy) {
    const auto lines = common::splitLines(input);
    m_rounds.reserve(std::size(lines));
    for (const auto &line : lines)
      m_rounds.emplace_back(line, first_strategy);
  }

  bool isValid() const {
    return std::all_of(std::begin(m_rounds), std::end(m_rounds),
                       [](const auto &round) { return round.isValid(); });
  }

  Int getValue() const {
    auto res = Int{0};
    for (const auto &round : m_rounds)
      res += round.getValue();
    return res;
  }

private:
  std::vector<Round> m_rounds;
};

} // namespace puzzle_2022_02

void Solver_2022_02_1::solve(const QString &input) {
  const auto game = puzzle_2022_02::Game{input, true};
  emit output(QString("Game is %1").arg(game.isValid() ? "valid" : "invalid"));
  emit finished(QString("%1").arg(game.getValue()));
}

void Solver_2022_02_2::solve(const QString &input) {
  const auto game = puzzle_2022_02::Game{input, false};
  emit output(QString("Game is %1").arg(game.isValid() ? "valid" : "invalid"));
  emit finished(QString("%1").arg(game.getValue()));
}
