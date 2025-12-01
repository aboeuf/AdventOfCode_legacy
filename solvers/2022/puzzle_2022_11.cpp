#include <QQueue>
#include <solvers/common.h>
#include <functional>
#include <solvers/2022/puzzle_2022_11.h>

namespace puzzle_2022_11 {

using Int = long long unsigned int;

inline QStringList match(const QString &pattern, QStringList &input) {
  auto rx = QRegExp("^" + pattern + "$");
  if (not rx.exactMatch(input.front()))
    throw std::invalid_argument(
        QString("invalid line \"%1\"\nDid not match pattern \"%2\"")
            .arg(input.front())
            .arg(pattern)
            .toStdString());
  do {
    input.pop_front();
  } while (not input.empty() and input.front().isEmpty());
  return rx.capturedTexts();
}

struct ItemToThrow {
  ItemToThrow(Int worry_level, std::size_t throw_to)
      : worry_level{worry_level}, throw_to{throw_to} {}
  Int worry_level;
  std::size_t throw_to;
};

class Monkey {
public:
  Monkey(QStringList &lines) {
    m_index =
        static_cast<std::size_t>(match("Monkey (\\d+):", lines)[1].toUInt());

    auto items_str = match("  Starting items: (.*)", lines)[1];
    items_str.remove(' ');
    const auto items = common::toVecUInt(items_str, ',');
    for (auto item : items)
      m_items.enqueue(item);

    const auto operation = match("  Operation: new = old (.) (.+)", lines);
    const auto is_addition = [&operation]() {
      if (operation[1] == "+")
        return true;
      if (operation[1] == "*")
        return false;
      throw std::invalid_argument(
          QString("invalid operator \"%1\"").arg(operation[1]).toStdString());
    }();
    if (operation[2] == "old") {
      if (is_addition)
        m_operation = [](Int i) { return i + i; };
      else
        m_operation = [](Int i) { return i * i; };
    } else {
      bool ok;
      const auto value = operation[2].toUInt(&ok);
      if (not ok)
        throw std::invalid_argument(
            QString("invalid operand \"%1\"").arg(operation[2]).toStdString());
      if (is_addition)
        m_operation = [value](Int i) { return i + value; };
      else if (operation[1] == "*")
        m_operation = [value](auto i) { return i * value; };
    }

    m_test = match("  Test: divisible by (\\d+)", lines)[1].toUInt();
    m_monkey_if_true = static_cast<std::size_t>(
        match("    If true: throw to monkey (\\d+)", lines)[1].toUInt());
    m_monkey_if_false = static_cast<std::size_t>(
        match("    If false: throw to monkey (\\d+)", lines)[1].toUInt());
  }

  std::size_t index() const { return m_index; }
  Int test() const { return m_test; }
  Int nbInspections() const { return m_nb_inspections; }

  std::vector<ItemToThrow> applyTurn(bool relaxed) {
    auto items = std::vector<ItemToThrow>();
    items.reserve(m_items.size());
    m_nb_inspections += m_items.size();
    while (not m_items.empty()) {
      auto worry_level = m_items.dequeue();
      worry_level = m_operation(worry_level);
      if (relaxed)
        worry_level /= 3;
      items.emplace_back(worry_level, worry_level % m_test == 0
                                          ? m_monkey_if_true
                                          : m_monkey_if_false);
    }
    return items;
  }

  void catchItem(Int item) { m_items.enqueue(item); }

private:
  std::size_t m_index;
  QQueue<Int> m_items;
  std::function<Int(Int)> m_operation;
  Int m_test;
  std::size_t m_monkey_if_true;
  std::size_t m_monkey_if_false;
  Int m_nb_inspections{0};
};

class Monkeys {
public:
  Monkeys(const QString &input) {
    auto lines = common::splitLines(input);
    while (not lines.empty())
      m_monkeys.emplace_back(lines);
    m_mod = Int{1};
    for (auto i = 0u; i < m_monkeys.size(); ++i) {
      m_mod *= m_monkeys[i].test();
      if (m_monkeys[i].index() != i) {
        throw std::invalid_argument("index mismatch");
      }
    }
  }

  QString solvePuzzle(uint nb_rounds, bool relaxed) {
    for (auto i = 0u; i < nb_rounds; ++i) {
      for (auto &monkey : m_monkeys) {
        const auto items = monkey.applyTurn(relaxed);
        for (const auto &item : items)
          m_monkeys[item.throw_to].catchItem(
              relaxed ? item.worry_level : item.worry_level % m_mod);
      }
    }
    auto nb_inspections = std::vector<Int>{};
    nb_inspections.reserve(m_monkeys.size());
    for (const auto &monkey : m_monkeys)
      nb_inspections.push_back(monkey.nbInspections());
    std::sort(std::begin(nb_inspections), std::end(nb_inspections),
              [](auto lhs, auto rhs) { return lhs > rhs; });
    return QString("%1").arg(nb_inspections[0] * nb_inspections[1]);
  }

private:
  std::vector<Monkey> m_monkeys;
  Int m_mod;
};

} // namespace puzzle_2022_11

void Solver_2022_11_1::solve(const QString &input) {
  auto monkeys = puzzle_2022_11::Monkeys(input);
  emit finished(monkeys.solvePuzzle(20u, true));
}

void Solver_2022_11_2::solve(const QString &input) {
  auto monkeys = puzzle_2022_11::Monkeys(input);
  emit finished(monkeys.solvePuzzle(10000u, false));
}
