#include <solvers/common.h>
#include <numeric>
#include <solvers/2023/puzzle_2023_08.h>

namespace puzzle_2023_08 {

using Int = unsigned long long int;

class Node {
public:
  Node() = default;
  Node(QString input) {
    input.remove(' ');
    auto splitted = common::splitValues(input, '=');
    m_label = splitted[0];
    input = splitted[1];
    input.remove('(').remove(')');
    splitted = common::splitValues(input, ',');
    m_left = splitted[0];
    m_right = splitted[1];
  }

  const QString &label() const { return m_label; }
  const QString &left() const { return m_left; }
  const QString &right() const { return m_right; }

private:
  QString m_label{};
  QString m_left{};
  QString m_right{};
};

class Map {
public:
  Map(const QString &input) {
    auto lines = common::splitLines(input);
    auto it = lines.begin();
    m_instructions = *it;
    ++it;
    ++it;
    for (; it != lines.end(); ++it) {
      auto node = Node{*it};
      m_nodes[node.label()] = node;
    }
  }

  Node get(const QString &label) const {
    if (m_nodes.contains(label))
      return m_nodes[label];
    common::throwInvalidArgumentError(
        QString("get: cannot find not with label \"%1\"").arg(label));
    return Node{};
  }

  Node getNext(const Node &node, const QChar &direction) const {
    if (direction == 'L')
      return get(node.left());
    if (direction == 'R')
      return get(node.right());
    common::throwInvalidArgumentError(
        QString("getNext: invalid direction '%1'").arg(direction));
    return Node{};
  }

  QString solveOne() const {
    auto current = get("AAA");
    auto nb_steps = Int{0};
    auto index = 0;
    while (current.label() != "ZZZ") {
      ++nb_steps;
      current = getNext(current, m_instructions[index]);
      index = (index + 1) % m_instructions.size();
    }
    return QString("%1").arg(nb_steps);
  }

  QString solveTwo() const {
    auto result = std::optional<Int>{};
    for (const auto &node : m_nodes) {
      if (node.label().back() == 'A') {
        auto current = node;
        auto nb_steps = Int{0};
        auto index = 0;
        while (current.label().back() != 'Z') {
          ++nb_steps;
          current = getNext(current, m_instructions[index]);
          index = (index + 1) % m_instructions.size();
        }
        if (result.has_value())
          result = (*result * nb_steps) / std::gcd(*result, nb_steps);
        else
          result = nb_steps;
      }
    }
    return result.has_value() ? QString("%1").arg(*result) : "Failure";
  }

private:
  QString m_instructions;
  QMap<QString, Node> m_nodes;
};

} // namespace puzzle_2023_08

void Solver_2023_08_1::solve(const QString &input) {
  const auto map = puzzle_2023_08::Map{input};
  emit finished(map.solveOne());
}

void Solver_2023_08_2::solve(const QString &input) {
  const auto map = puzzle_2023_08::Map{input};
  emit finished(map.solveTwo());
}
