#include <solvers/common.h>
#include <functional>
#include <limits>
#include <solvers/qpoint_hash.hpp>
#include <solvers/2024/puzzle_2024_21.h>

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

using SequenceMap = QHash<QChar, QHash<QChar, QSet<QString>>>;

class Keypad {
public:
  Keypad(const QStringList &layout) {
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
    for (const auto &start : m_characters) {
      auto cost_map = QHash<QChar, uint>();
      const auto get_cost = [&cost_map](const QChar &c) {
        return cost_map.value(c, std::numeric_limits<uint>::max());
      };
      const auto compare = [&get_cost](const QChar &lhs, const QChar &rhs) {
        return get_cost(rhs) < get_cost(lhs);
      };
      auto open_set = OpenSet(compare, m_characters.size());
      cost_map[start] = 0u;
      m_sequence_map[start][start].insert("A");
      open_set.push(start);
      while (not open_set.empty()) {
        const auto current = open_set.pop();
        const auto current_position = position(current);
        const auto alternative_cost = cost_map[current] + 1u;
        for (auto it = cardinal_directions.cbegin();
             it != cardinal_directions.cend(); ++it) {
          const auto next_position = current_position + it.key();
          if (isInPad(next_position)) {
            const auto next = at(next_position);
            const auto next_cost = get_cost(next);
            if (alternative_cost <= next_cost) {
              if (alternative_cost < next_cost) {
                cost_map[next] = alternative_cost;
                m_sequence_map[start][next].clear();
              }
              for (const auto &current_sequence :
                   m_sequence_map[start][current]) {
                m_sequence_map[start][next].insert(
                    QString("%1%2A")
                        .arg(current_sequence.chopped(1))
                        .arg(it.value()));
              }
              if (alternative_cost < next_cost) {
                open_set.push(next);
              }
            }
          }
        }
      }
    }
  }

  int nbRows() const { return m_nb_rows; }
  int nbColumns() const { return m_nb_columns; }
  const QString &characters() const { return m_characters; }

  const QSet<QString> &getSequences(const QChar &start,
                                    const QChar &end) const {
    const auto start_it = m_sequence_map.constFind(start);
    if (start_it == m_sequence_map.cend()) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_21::Keypad::getSequences: cannot find start "
                  "character '\%1' in cost map")
              .arg(start));
    }
    const auto end_it = start_it.value().constFind(end);
    if (end_it == start_it.value().cend()) {
      common::throwInvalidArgumentError(
          QString(
              "puzzle_2024_21::Keypad::getSequences: cannot find (start, end) "
              "characters ('\%1', '\%2') in cost map")
              .arg(start)
              .arg(end));
    }
    return end_it.value();
  }

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

private:
  int m_nb_rows;
  int m_nb_columns;
  QString m_characters;
  SequenceMap m_sequence_map;
  QHash<QChar, QPoint> m_positions;
  std::vector<std::vector<int>> m_layout;
};

class CostMap {
public:
  CostMap(const Keypad &keypad) {
    for (const auto &start : keypad.characters()) {
      for (const auto &end : keypad.characters()) {
        m_cost_map[start][end] = Int(1);
      }
    }
  }

  CostMap(const Keypad &keypad, const CostMap &parent) {
    for (const auto &start : keypad.characters()) {
      for (const auto &end : keypad.characters()) {
        auto min_cost = std::numeric_limits<Int>::max();
        for (const auto &sequence : keypad.getSequences(start, end)) {
          min_cost = std::min(min_cost, parent.getCost(sequence));
        }
        m_cost_map[start][end] = min_cost;
      }
    }
  }

  Int getCost(const QString &sequence) const {
    auto prev = QChar('A');
    auto cost = Int(0);
    for (const auto &c : sequence) {
      cost += getCost(prev, c);
      prev = c;
    }
    return cost;
  }

private:
  Int getCost(const QChar &start, const QChar &end) const {
    const auto start_it = m_cost_map.constFind(start);
    if (start_it == m_cost_map.cend()) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_21::CostMap::getCost: cannot find start "
                  "character '\%1' in cost map")
              .arg(start));
    }
    const auto end_it = start_it.value().constFind(end);
    if (end_it == start_it.value().cend()) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_21::CostMap::getCost: cannot find (start, end) "
                  "characters ('\%1', '\%2') in cost map")
              .arg(start)
              .arg(end));
    }
    return end_it.value();
  }

  QHash<QChar, QHash<QChar, Int>> m_cost_map;
};

class Code {
public:
  Code(const QString &code) : m_code{code} {
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

  QString solve(uint nb_keypads) const {
    const auto directional_keypad = Keypad(directional_layout);
    auto directional_cost_map = CostMap(directional_keypad);
    for (auto i = 0u; i < nb_keypads; ++i) {
      auto next_cost_map = CostMap(directional_keypad, directional_cost_map);
      std::swap(directional_cost_map, next_cost_map);
    }
    const auto numeric_keypad = Keypad(numeric_layout);
    const auto numeric_cost_map = CostMap(numeric_keypad, directional_cost_map);
    auto sum = Int(0);
    for (const auto &code : m_codes) {
      sum += code.numericValue() * numeric_cost_map.getCost(code.toString());
    }
    return QString("%1").arg(sum);
  }

  QString solveTwo() const { return "Default"; }

private:
  std::vector<Code> m_codes;
};

} // namespace puzzle_2024_21

void Solver_2024_21_1::solve(const QString &input) {
  const auto codes = puzzle_2024_21::Codes(input);
  emit finished(codes.solve(2u));
}

void Solver_2024_21_2::solve(const QString &input) {
  const auto codes = puzzle_2024_21::Codes(input);
  emit finished(codes.solve(25u));
}
