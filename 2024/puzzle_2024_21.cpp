#include <2024/puzzle_2024_21.h>
#include <common.h>
#include <functional>
#include <initializer_list>
#include <limits>
#include <qpoint_hash.hpp>

namespace puzzle_2024_21 {

using Int = unsigned long long;

const auto cardinal_directions =
    QSet<QPoint>{QPoint{-1, 0}, QPoint{1, 0}, QPoint{0, -1}, QPoint{0, 1}};


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

class Keypad {
public:
  Keypad(const std::initializer_list<std::string> &layout,
         const std::optional<Keypad> &parent) {
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
      for (const auto &c_end : m_characters) {
        if (not parent.has_value() or c_start != c_end) {
          m_cost_map[c_start][c_end] = Int{1};
        } else {
          m_cost_map[c_start][c_end] = Int{1};
        }
      }
    }
  }

  virtual ~Keypad() = default;

  int nbRows() const { return m_nb_rows; }
  int nbColumns() const { return m_nb_columns; }

  const QString &characters() const { return m_characters; }

  bool isInPad(const QPoint &point) const {
    return point.x() >= 0 and point.x() < m_nb_rows and point.y() >= 0 and
           point.y() < m_nb_columns and m_layout[point.x()][point.y()] >= 0 and
           m_layout[point.x()][point.y()] < m_characters.size();
  }

  QChar at(const QPoint &point) {
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

  Int getCost(const QChar &start, const QChar &end) const {
    const auto start_it = m_cost_map.constFind(start);
    if (start_it == m_cost_map.cend()) {
      return std::numeric_limits<Int>::max();
    }
    const auto end_it = start_it.value().constFind(end);
    if (end_it == start_it.value().cend()) {
      return std::numeric_limits<Int>::max();
    }
    return end_it.value();
  }

private:
  QHash<QChar, QHash<QChar, Int>> m_cost_map;
  QString m_characters;
  QHash<QChar, QPoint> m_positions;
  int m_nb_rows;
  int m_nb_columns;
  std::vector<std::vector<int>> m_layout;
};

class DirectionalKeypad : public Keypad {
public:
  DirectionalKeypad(const std::optional<DirectionalKeypad> &parent = std::nullopt)
      : Keypad({"#^A", "<v>"}, parent) {}

  virtual ~DirectionalKeypad() final {}
};

class NumericKeypad : public Keypad {
public:
  NumericKeypad(const DirectionalKeypad &parent)
      : Keypad({"789", "456", "123", "#0A"}, parent) {}
  virtual ~NumericKeypad() final {}
};

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

  int size() const { return m_code.size(); }

  QChar at(int i) const { return m_code.at(i); }

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
    auto directional_keypads = std::vector<DirectionalKeypad>();
    directional_keypads.reserve(nb_directional_keypads);
    directional_keypads.emplace_back();
    for (auto i = 1u; i < nb_directional_keypads; ++i) {
      directional_keypads.emplace_back(directional_keypads.back());
    }
    const auto numeric_keypad = NumericKeypad(directional_keypads.back());
    auto sum = 0u;
    for (const auto &code : m_codes) {
      auto cost = numeric_keypad.getCost('A', code.at(0));
      for (auto i = 0; i + 1 < code.size(); ++i) {
        cost += numeric_keypad.getCost(code.at(i), code.at(i + 1));
      }
      sum += code.numericValue() * cost;
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
  const auto lines = puzzle_2024_21::Codes(input);
  emit finished(lines.solveTwo());
}
