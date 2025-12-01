#include <QSet>
#include <solvers/common.h>
#include <solvers/2020/puzzle_2020_22.h>

namespace puzzle_2020_22 {

using Int = unsigned int;

class Deck {
public:
  Deck() = default;

  void set(QStringList &lines) {
    if (lines.empty())
      return;
    QString line = lines.front();
    lines.pop_front();
    QRegExp rx("Player (\\d+):");
    if (rx.lastIndexIn(line) != 0)
      return;
    m_player_id = rx.capturedTexts()[1].toUInt();
    bool ok;
    while (!lines.isEmpty()) {
      line = lines.front();
      lines.pop_front();
      if (line.isEmpty())
        return;
      Int card = line.toULongLong(&ok);
      if (ok)
        m_cards << card;
    }
  }

  Int pop() {
    if (m_cards.isEmpty())
      return 0;
    Int card = m_cards.front();
    m_cards.pop_front();
    return card;
  }

  void operator<<(Int card) { m_cards << card; }

  bool empty() const { return m_cards.isEmpty(); }

  Int value() const {
    Int sum = 0;
    Int multiplier = 1;
    for (auto card = m_cards.rbegin(); card != m_cards.rend(); ++card) {
      sum += multiplier * (*card);
      ++multiplier;
    }
    return sum;
  }

  QString toString() const {
    QString s;
    for (Int card : m_cards)
      s += QString::number(card) + ",";
    if (s.endsWith(","))
      s.chop(1);
    return s;
  }

  const QList<Int> &cards() const { return m_cards; }

private:
  uint m_player_id{0};
  QList<Int> m_cards{};
};

struct CombatGame {
  CombatGame(const QString &input) {
    QStringList lines = common::splitLines(input);
    m_player_1.set(lines);
    m_player_2.set(lines);
  }

  void play() {
    while (!m_player_1.empty() && !m_player_2.empty()) {
      Int c1 = m_player_1.pop();
      Int c2 = m_player_2.pop();
      if (c1 < c2) {
        m_player_2 << c2;
        m_player_2 << c1;
      } else {
        m_player_1 << c1;
        m_player_1 << c2;
      }
    }
  }

  Int getWinnerDeckValue() const {
    return m_player_1.empty() ? m_player_2.value() : m_player_1.value();
  }

  Deck m_player_1{};
  Deck m_player_2{};
};

struct RecursiveCombatGame {
  RecursiveCombatGame(const QString &input) {
    QStringList lines = common::splitLines(input);
    m_player_1.set(lines);
    m_player_2.set(lines);
  }

  RecursiveCombatGame(const Deck &player_1, const Deck &player_2, int n1,
                      int n2, Int depth)
      : m_depth{depth + 1} {
    for (auto card = player_1.cards().begin();
         m_player_1.cards().size() != n1 && card != player_1.cards().end();
         ++card)
      m_player_1 << *card;

    for (auto card = player_2.cards().begin();
         m_player_2.cards().size() != n2 && card != player_2.cards().end();
         ++card)
      m_player_2 << *card;
  }

  bool play() {
    QSet<QString> history{};
    bool player_1_wins;
    while (!m_player_1.empty() && !m_player_2.empty()) {
      QString state = toString();
      if (history.contains(state))
        return true;
      history.insert(state);
      Int c1 = m_player_1.pop();
      Int c2 = m_player_2.pop();
      int n1 = static_cast<int>(c1);
      int n2 = static_cast<int>(c2);
      if (m_player_1.cards().size() < n1 || m_player_2.cards().size() < n2)
        player_1_wins = n1 > n2;
      else {
        RecursiveCombatGame game(m_player_1, m_player_2, n1, n2, m_depth);
        player_1_wins = game.play();
      }
      if (player_1_wins) {
        m_player_1 << c1;
        m_player_1 << c2;
      } else {
        m_player_2 << c2;
        m_player_2 << c1;
      }
    }
    return m_player_2.empty();
  }

  QString toString() const {
    return m_player_1.toString() + "|" + m_player_2.toString();
  }

  Int getWinnerDeckValue() const {
    return m_player_1.empty() ? m_player_2.value() : m_player_1.value();
  }

  Int m_depth{0};
  Deck m_player_1{};
  Deck m_player_2{};
};

} // namespace puzzle_2020_22

void Solver_2020_22_1::solve(const QString &input) {
  using namespace puzzle_2020_22;
  CombatGame game(input);
  game.play();
  emit finished(QString::number(game.getWinnerDeckValue()));
}

void Solver_2020_22_2::solve(const QString &input) {
  using namespace puzzle_2020_22;
  RecursiveCombatGame game(input);
  game.play();
  emit finished(QString::number(game.getWinnerDeckValue()));
}
