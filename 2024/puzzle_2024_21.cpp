#include <2024/puzzle_2024_21.h>
#include <common.h>
#include <functional>
#include <initializer_list>
#include <limits>
#include <qpoint_hash.hpp>

#include <iostream>

namespace puzzle_2024_21 {

using Int = unsigned long long;

const auto cardinal_directions = []() {
  auto directions = QHash<QPoint, QChar>();
  directions.insert(QPoint(-1, 0), '^');
  directions.insert(QPoint(1, 0), 'v');
  directions.insert(QPoint(0, -1), '<');
  directions.insert(QPoint(0, 1), '>');
  return directions;
}();

inline void checkCharacter(const QChar &c, const QString &caller) {
  if (c != 'A' and not c.isDigit()) {
    common::throwInvalidArgumentError(
        QString("puzzle_2024_21::%1: invalid character '\%2'")
            .arg(caller)
            .arg(c));
  }
}

using Compare = std::function<bool(const QChar &, const QChar &)>;

class OpenSet {
public:
  OpenSet(const Compare &compare, int size) : m_compare{compare} {
    m_heap.reserve(size);
  }

  bool empty() const { return std::empty(m_heap); }

  QChar pop() {
    const auto point = m_heap.front();
    std::pop_heap(std::begin(m_heap), std::end(m_heap), m_compare);
    m_heap.pop_back();
    m_unique.remove(point);
    return point;
  }

  void push(const QChar &point) {
    if (not m_unique.contains(point)) {
      m_unique.insert(point);
      m_heap.push_back(point);
    }
    std::push_heap(std::begin(m_heap), std::end(m_heap), m_compare);
  }

private:
  Compare m_compare;
  std::vector<QChar> m_heap;
  QSet<QChar> m_unique;
};

struct Cost {
  Cost() = default;
  Cost(const Int value, const QString &sequence)
      : value{value}, sequence{sequence} {}
  Int value{1u};
  QString sequence{"A"};
};

const auto directional_layout = []() {
  auto layout = QStringList();
  layout << "#^A";
  layout << "<v>";
  return layout;
}();

const auto numeric_layout = []() {
  auto layout = QStringList();
  layout << "789";
  layout << "456";
  layout << "123";
  layout << "#0A";
  return layout;
}();

using SequenceMap = QHash<QChar, QHash<QChar, QSet<QString>>>;

enum class KeypadType { directional, numeric };

class Keypad {
public:
  Keypad(KeypadType type, const std::optional<Keypad> &parent) : m_type{type} {
    const auto &layout =
        type == KeypadType::directional ? directional_layout : numeric_layout;
    m_nb_rows = static_cast<int>(std::size(layout));
    if (m_nb_rows == 0u) {
      common::throwInvalidArgumentError("puzzle_2024_21::Keypad: empty layout");
    }
    auto row = std::begin(layout);
    m_nb_columns = static_cast<int>(row->size());
    m_layout = std::vector<std::vector<int>>(
        m_nb_rows, std::vector<int>(m_nb_columns, -1));
    auto i = 0;
    for (; row != std::end(layout); ++row) {
      if (static_cast<int>(row->size()) != m_nb_columns) {
        common::throwInvalidArgumentError(
            "puzzle_2024_21::Keypad: incoherent number of columns in layout");
      }
      for (auto j = 0; j < m_nb_columns; ++j) {
        const auto c = QChar(row->at(j));
        if (c != '#') {
          m_positions[c] = QPoint(i, j);
          m_layout[i][j] = m_characters.size();
          m_characters.push_back(c);
        }
      }
      ++i;
    }
    for (const auto &c_start : m_characters) {
      if (parent.has_value()) {
        const auto &sequence_map = type == KeypadType::directional
                                       ? s_directional_sequence_map
                                       : s_numeric_sequence_map;
        for (const auto &c_end : m_characters) {
          auto min_cost = std::numeric_limits<Int>::max();
          auto min_sequence = QString("");
          for (const auto &sequence : sequence_map[c_start][c_end]) {
            const auto cost = parent->getCost(sequence);
            if (cost < min_cost) {
              min_cost = cost;
              min_sequence = sequence;
            }
          }
          m_cost_map[c_start][c_end] = Cost(min_cost, min_sequence);
        }
      } else {
        for (const auto &c_end : m_characters) {
          m_cost_map[c_start][c_end] = Cost();
        }
      }
    }
  }

  int nbRows() const { return m_nb_rows; }
  int nbColumns() const { return m_nb_columns; }

  const QString &characters() const { return m_characters; }

  bool isInPad(const QPoint &point) const {
    return point.x() >= 0 and point.x() < m_nb_rows and point.y() >= 0 and
           point.y() < m_nb_columns and m_layout[point.x()][point.y()] >= 0 and
           m_layout[point.x()][point.y()] < m_characters.size();
  }

  QChar at(const QPoint &point) const {
    return m_characters[m_layout[point.x()][point.y()]];
  }

  const QPoint &position(const QChar &c) const {
    const auto it = m_positions.constFind(c);
    if (it == m_positions.cend()) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_21::Keypad::position: cannot find character "
                  "'%1' in keypad")
              .arg(c));
    }
    return it.value();
  }

  Int getCost(const QString &sequence, QString *details = nullptr) const {
    auto prev = QChar('A');
    auto cost = Int{0};
    for (const auto &c : sequence) {
      const auto transition_cost = getCost(prev, c, true).value;
      if (details) {
        *details += QString("%1(%2, %3, %4)")
                        .arg(cost == Int{0} ? "" : " + ")
                        .arg(prev)
                        .arg(c)
                        .arg(transition_cost);
      }
      cost += transition_cost;
      prev = c;
    }
    if (details) {
      *details += QString(" = %1").arg(cost);
    }
    return cost;
  }

  SequenceMap computeSequenceMap() const {
    auto sequence_map = SequenceMap();
    for (const auto &start : m_characters) {
      auto cost_map = QHash<QChar, uint>();
      const auto get_cost = [&cost_map](const QChar &c) {
        const auto it = cost_map.constFind(c);
        if (it == cost_map.cend()) {
          return std::numeric_limits<uint>::max();
        }
        return *it;
      };
      const auto compare = [&get_cost](const QChar &lhs, const QChar &rhs) {
        return get_cost(rhs) < get_cost(lhs);
      };
      auto open_set = OpenSet(compare, m_characters.size());
      cost_map[start] = 0u;
      sequence_map[start][start].insert("A");
      open_set.push(start);
      while (not open_set.empty()) {
        const auto current = open_set.pop();
        const auto current_position = position(current);
        const auto next_cost = cost_map[current] + 1u;
        for (auto it = cardinal_directions.cbegin();
             it != cardinal_directions.cend(); ++it) {
          const auto next_position = current_position + it.key();
          if (isInPad(next_position)) {
            const auto next = at(next_position);
            if (next_cost <= get_cost(next)) {
              cost_map[next] = next_cost;
              for (const auto &current_sequence :
                   sequence_map[start][current]) {
                sequence_map[start][next].insert(
                    QString("%1%2A")
                        .arg(current_sequence.chopped(1))
                        .arg(it.value()));
              }
            }
          }
        }
      }
    }
    return sequence_map;
  }

  void printCostMap(int index,
                    const std::optional<Keypad> &parent = std::nullopt) const {
    std::cout << "K" << index << ":" << std::endl;
    auto line = QString("+");
    for (auto i = 0; i < m_characters.size() + 1; ++i) {
      line += QString("----+");
    }
    std::cout << line.toStdString() << std::endl << "|    ";
    for (const auto &c : m_characters) {
      std::cout << QString("| %1  ").arg(c).toStdString();
    }
    std::cout << "|" << std::endl << line.toStdString() << std::endl;
    for (const auto &start : m_characters) {
      std::cout << QString("| %1  ").arg(start).toStdString();
      for (const auto &end : m_characters) {
        std::cout << QString("| %1 ")
                         .arg(getCost(start, end, true).value, 2, 10,
                              QChar('0'))
                         .toStdString();
      }
      std::cout << "|" << std::endl << line.toStdString() << std::endl;
    }
    for (const auto &start : m_characters) {
      for (const auto &end : m_characters) {
        const auto extra_string = [this, start, end, &parent]() {
          if (not parent.has_value()) {
            return QString("");
          }
          auto details = QString();
          const auto cost =
              parent->getCost(m_cost_map[start][end].sequence, &details);
          return QString(" (%2 = %1)").arg(cost).arg(details);
        }();
        std::cout << QString("K%6(%1, %2) = K%7(%3) = %4%5")
                         .arg(start)
                         .arg(end)
                         .arg(m_cost_map[start][end].sequence)
                         .arg(m_cost_map[start][end].value)
                         .arg(extra_string)
                         .arg(index)
                         .arg(index - 1)
                         .toStdString()
                  << std::endl;
      }
    }
    std::cout << std::endl;
  }

private:
  const Cost &getCost(const QChar &start, const QChar &end,
                      bool throw_exception) const {
    const auto start_it = m_cost_map.constFind(start);
    if (start_it == m_cost_map.cend()) {
      if (throw_exception) {
        common::throwInvalidArgumentError(
            QString("puzzle_2024_21::Keypad::getCost: cannot find start "
                    "character '\%1' in cost map")
                .arg(start));
      }
      return s_max_cost;
    }
    const auto end_it = start_it.value().constFind(end);
    if (end_it == start_it.value().cend()) {
      if (throw_exception) {
        common::throwInvalidArgumentError(
            QString("puzzle_2024_21::Keypad::getCost: cannot find (start, end) "
                    "characters ('\%1', '\%2') in cost map")
                .arg(start)
                .arg(end));
      }
      return s_max_cost;
    }
    return end_it.value();
  }

  KeypadType m_type;
  QHash<QChar, QHash<QChar, Cost>> m_cost_map;
  QString m_characters;
  QHash<QChar, QPoint> m_positions;
  int m_nb_rows;
  int m_nb_columns;
  std::vector<std::vector<int>> m_layout;
  static Cost s_max_cost;
  static SequenceMap s_directional_sequence_map;
  static SequenceMap s_numeric_sequence_map;
};

Cost Keypad::s_max_cost = Cost(std::numeric_limits<Int>::max(), "");

SequenceMap Keypad::s_directional_sequence_map =
    Keypad(KeypadType::directional, std::nullopt).computeSequenceMap();

SequenceMap Keypad::s_numeric_sequence_map =
    Keypad(KeypadType::numeric, std::nullopt).computeSequenceMap();

class Code {
public:
  Code(const QString &code) : m_code{code} {
    for (const auto &c : code) {
      checkCharacter(c, "Code");
    }
    auto copy = QString(m_code);
    copy.remove('A');
    while (copy.startsWith('0')) {
      copy = copy.mid(1);
    }
    auto ok = true;
    m_numeric_value = copy.toULongLong(&ok);
    if (not ok) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_21::Code: cannot extract numeric value from "
                  "token \"%1\" (extracted from code \"%2\"")
              .arg(copy)
              .arg(code));
    }
  }

  const QString &toString() const { return m_code; }
  Int numericValue() const { return m_numeric_value; }

private:
  QString m_code;
  Int m_numeric_value;
};

class Codes {
public:
  Codes(const QString &input) {
    const auto lines = common::splitLines(input, true);
    m_codes.reserve(lines.size());
    for (const auto &line : lines) {
      m_codes.emplace_back(line);
    }
  }

  QString solve(uint nb_directional_keypads) const {
    auto directional_keypads = std::vector<Keypad>();
    directional_keypads.reserve(nb_directional_keypads);
    directional_keypads.emplace_back(KeypadType::directional, std::nullopt);
    for (auto i = 1u; i < nb_directional_keypads; ++i) {
      directional_keypads.emplace_back(KeypadType::directional,
                                       directional_keypads.back());
    }
    const auto numeric_keypad =
        Keypad(KeypadType::numeric, directional_keypads.back());
    auto sum = 0u;
    for (const auto &code : m_codes) {
      sum += code.numericValue() * numeric_keypad.getCost(code.toString());
    }
    return QString("%1").arg(sum);
  }

  QString solveTwo() const { return "Default"; }

private:
  std::vector<Code> m_codes;
};

} // namespace puzzle_2024_21

void Solver_2024_21_1::solve(const QString &input) {
  const auto lines = puzzle_2024_21::Codes(input);
  emit finished(lines.solve(3u));
}

void Solver_2024_21_2::solve(const QString &input) {
    const auto human = puzzle_2024_21::Keypad(puzzle_2024_21::KeypadType::directional, std::nullopt);
  human.printCostMap(0);
  const auto robot_1 = puzzle_2024_21::Keypad(puzzle_2024_21::KeypadType::directional, human);
  robot_1.printCostMap(1, human);
  const auto robot_2 = puzzle_2024_21::Keypad(puzzle_2024_21::KeypadType::directional, robot_1);
  robot_2.printCostMap(2, robot_1);
  const auto robot_3 = puzzle_2024_21::Keypad(puzzle_2024_21::KeypadType::numeric, robot_2);
  robot_3.printCostMap(3, robot_2);
  const auto lines = common::splitLines(input);
  for (const auto &line : lines) {
    const auto code = puzzle_2024_21::Code(line);
    emit output(QString("%1: %2 * %3")
                    .arg(code.toString())
                    .arg(robot_3.getCost(code.toString()))
                    .arg(code.numericValue()));
  }
  emit finished("Done");
}
