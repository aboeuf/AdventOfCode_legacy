#include <solvers/2018/puzzle_2018_09.h>
#include <solvers/common.h>

namespace puzzle_2018_09 {

using Int = unsigned long long;

inline Int toInt(const QString &string) {
  auto ok = true;
  const auto value = string.toULongLong(&ok);
  if (not ok) {
    common::throwInvalidArgumentError(
        QString("puzzle_2018_09::toInt: cannot convert string \"%1\" to "
                "unsigned integer")
            .arg(string));
  }
  return value;
}

struct Marble {
  Marble(Int value = Int(0), Marble *previous = nullptr, Marble *next = nullptr)
      : value{value}, previous{previous}, next{next} {
    if (previous) {
      previous->next = this;
    } else {
      this->previous = this;
    }
    if (next) {
      next->previous = this;
    } else {
      this->next = this;
    }
  }

  Int value;
  Marble *previous;
  Marble *next;
};

struct Player {
  Int score{0u};
};

class Game {
public:
  Game(const QString &input, bool v2) {
    const auto first_line = common::splitLines(input, true).front();
    auto rx = QRegExp("^(\\d+) players; last marble is worth (\\d+) points$");
    if (not rx.exactMatch(first_line)) {
      common::throwInvalidArgumentError(
          QString("puzzle_2018_09::PuzzleInputs: cannot parse line \"%1\"")
              .arg(first_line));
    }
    m_nb_players = toInt(rx.cap(1));
    m_last_marble_value = toInt(rx.cap(2));
    if (v2) {
      m_last_marble_value *= Int(100);
    }
    m_players.resize(m_nb_players);
    m_current_marble = new Marble();
    ++m_nb_marbles;
    run();
  }

  ~Game() {
    while (m_nb_marbles != Int(0)) {
      deleteCurrent();
    }
  }

  QString maxScore() const {
    auto max_score = Int(0);
    for (const auto &player : m_players) {
      max_score = std::max(max_score, player.score);
    }
    return QString("%1").arg(max_score);
  }

private:
  void deleteCurrent() {
    if (m_nb_marbles == Int(0)) {
      common::throwRunTimeError("puzzle_2018_09::deleteCurrent: empty circle");
    }
    if (not m_current_marble) {
      common::throwRunTimeError(
          "puzzle_2018_09::deleteCurrent: current marble is nullptr");
    }
    --m_nb_marbles;
    if (m_nb_marbles == Int(0)) {
      delete m_current_marble;
      m_current_marble = nullptr;
      return;
    }
    auto *previous = m_current_marble->previous;
    auto *next = m_current_marble->next;
    previous->next = next;
    next->previous = previous;
    delete m_current_marble;
    m_current_marble = next;
  }

  void run() {
    auto current_player_index = Int(0);
    for (auto marble_value = Int(1); marble_value <= m_last_marble_value;
         ++marble_value) {
      if (marble_value % Int(23) == Int(0)) {
        m_players[current_player_index].score += marble_value;
        for (auto i = 0; i < 7; ++i) {
          m_current_marble = m_current_marble->previous;
        }
        m_players[current_player_index].score += m_current_marble->value;
        deleteCurrent();
      } else {
        auto *previous = m_current_marble->next;
        auto *next = previous->next;
        m_current_marble = new Marble(marble_value, previous, next);
        ++m_nb_marbles;
      }
      ++current_player_index;
      if (current_player_index >= m_nb_players) {
        current_player_index = Int(0);
      }
    }
  }

  Int m_nb_marbles{0};
  Int m_nb_players;
  Int m_last_marble_value;
  std::vector<Player> m_players;
  Marble *m_current_marble;
};

} // namespace puzzle_2018_09

void Solver_2018_09_1::solve(const QString &input) {
  const auto game = puzzle_2018_09::Game(input, false);
  emit finished(game.maxScore());
}

void Solver_2018_09_2::solve(const QString &input) {
  const auto game = puzzle_2018_09::Game(input, true);
  emit finished(game.maxScore());
}
