#include <solvers/common.h>
#include <solvers/2023/puzzle_2023_02.h>

using Int = unsigned int;

namespace puzzle_2023_02 {

struct Cubes {

  Cubes() = default;

  Cubes(Int red, Int green, Int blue) : red{red}, green{green}, blue{blue} {}

  bool operator>(const Cubes &other) const {
    return red > other.red or green > other.green or blue > other.blue;
  }

  Int power() const { return red * green * blue; }

  QString toString() const {
    return QString("[%1, %2, %3]").arg(red).arg(green).arg(blue);
  }

  Int red{0};
  Int green{0};
  Int blue{0};
};

class Game {
public:
  Game() = default;

  Game(const QString &input) {
    const auto index_and_draws = common::splitValues(input, ':');
    m_index = common::splitValues(index_and_draws.front(), ' ').back().toUInt();
    const auto draws = common::splitValues(index_and_draws.back(), ';');
    m_random_draws.reserve(draws.size());
    for (const auto &draw : draws) {
      const auto splitted_draw = common::splitValues(draw, ',');
      auto red = Int{0};
      auto green = Int{0};
      auto blue = Int{0};
      for (const auto &color : splitted_draw) {
        const auto splitted_colors = common::splitValues(color.mid(1), ' ');
        const auto value = splitted_colors.front().toUInt();
        if (splitted_colors.back() == "red")
          red = value;
        else if (splitted_colors.back() == "green")
          green = value;
        else if (splitted_colors.back() == "blue")
          blue = value;
      }
      m_random_draws.emplace_back(red, green, blue);
    }
  }

  QString toString() const {
    auto res = QString("Game %1:").arg(m_index);
    for (const auto &draw : m_random_draws)
      res += " " + draw.toString();
    return res;
  }

  Int index() const { return m_index; }

  bool isPossible(const Cubes &reality) const {
    for (const auto &draw : m_random_draws)
      if (draw > reality)
        return false;
    return true;
  }

  Cubes minPossible() const {
    auto min = Cubes{};
    for (const auto &draw : m_random_draws) {
      min.red = std::max(min.red, draw.red);
      min.green = std::max(min.green, draw.green);
      min.blue = std::max(min.blue, draw.blue);
    }
    return min;
  }

  Int minPower() const { return minPossible().power(); }

private:
  Int m_index;
  std::vector<Cubes> m_random_draws;
};

class Games {
public:
  Games(const QString &input) {
    const auto lines = common::splitLines(input);
    m_games.reserve(lines.size());
    for (const auto &line : lines)
      m_games.emplace_back(line);
  }

  QString toString() const {
    auto res = QString("");
    for (const auto &game : m_games)
      res += game.toString() + "\n";
    return res;
  }

  Int possibilityValue(const Cubes &reality) const {
    auto sum = Int{0};
    for (const auto &game : m_games)
      if (game.isPossible(reality))
        sum += game.index();
    return sum;
  }

  Int minPowerSum() const {
    auto sum = Int{0};
    for (const auto &game : m_games)
      sum += game.minPower();
    return sum;
  }

private:
  std::vector<Game> m_games;
};

} // namespace puzzle_2023_02

void Solver_2023_02_1::solve(const QString &input) {
  const auto games = puzzle_2023_02::Games(input);
  emit output(games.toString());
  const auto reality = puzzle_2023_02::Cubes(12, 13, 14);
  const auto value = games.possibilityValue(reality);
  emit finished(QString("%1").arg(value));
}

void Solver_2023_02_2::solve(const QString &input) {
  const auto games = puzzle_2023_02::Games(input);
  emit output(games.toString());
  const auto value = games.minPowerSum();
  emit finished(QString("%1").arg(value));
}
