#include <2020/puzzle_2020_15.h>
#include <QMap>
#include <QVector>
#include <common.h>

namespace puzzle_2020_15 {

struct Game {
  Game(const QString &input) {
    QVector<uint> start = common::toUInt(input);
    for (const uint &i : start)
      speak(i);
  }

  void speak(uint number) {
    uint last_spoken_at = m_last_spoken_at.value(number, m_current_turn);
    m_next = m_current_turn - last_spoken_at;
    m_last_spoken_at[number] = m_current_turn;
    ++m_current_turn;
    m_last_spoken = number;
  }

  void playNextTurn() { speak(m_next); }

  uint m_next;
  uint m_current_turn{0};
  QMap<uint, uint> m_last_spoken_at{};
  uint m_last_spoken;
};

} // namespace puzzle_2020_15

void Solver_2020_15_1::solve(const QString &input) {
  using namespace puzzle_2020_15;
  Game game(input);
  while (game.m_current_turn < 2020) {
    game.playNextTurn();
  }
  emit finished(QString::number(game.m_last_spoken));
}

void Solver_2020_15_2::solve(const QString &input) {
  using namespace puzzle_2020_15;
  Game game(input);
  while (game.m_current_turn < 30000000) {
    game.playNextTurn();
  }
  emit finished(QString::number(game.m_last_spoken));
}
