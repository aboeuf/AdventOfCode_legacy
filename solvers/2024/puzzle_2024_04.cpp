#include <solvers/common.h>
#include <deque>
#include <solvers/2024/puzzle_2024_04.h>

namespace puzzle_2024_04 {

inline std::optional<QChar> getNext(const QChar &c) {
  if (c == 'X') {
    return 'M';
  }
  if (c == 'M') {
    return 'A';
  }
  if (c == 'A') {
    return 'S';
  }
  return std::nullopt;
}

struct Coordinates {
  int row;
  int column;

  bool operator==(const Coordinates &other) const {
    return row == other.row and column == other.column;
  }
};

struct Node;
using NodePtr = std::shared_ptr<Node>;

struct Node {
  Node(const QChar &value, int row, int column)
      : value{value}, coordinates{row, column} {}

  Node(const QChar &value, const Coordinates &coordinates, NodePtr parent)
      : value{value}, coordinates{coordinates}, parent{parent} {}

  bool hasParentAt(const Coordinates &coordinates) {
    auto current = parent;
    while (current) {
      if (current->coordinates == coordinates) {
        return true;
      }
    }
    return false;
  }

  std::size_t nbLeafs() const {
    if (children.empty()) {
      return value == 'S' ? 1u : 0u;
    }
    auto nb_leafs = 0u;
    for (const auto &child : children) {
      nb_leafs += child->nbLeafs();
    }
    return nb_leafs;
  }

  QChar value;
  Coordinates coordinates;
  NodePtr parent{nullptr};
  std::deque<NodePtr> children{};
};

class WordSearch {
public:
  WordSearch(const QString &input) {
    const auto lines = common::splitLines(input);
    m_nb_rows = lines.size();
    if (m_nb_rows == 0) {
      throw std::invalid_argument{"empty input"};
    }
    m_cells.reserve(m_nb_rows);
    m_nb_columns = lines.front().size();
    auto row = 0;
    for (const auto &line : lines) {
      if (lines.front().size() != m_nb_columns) {
        throw std::invalid_argument{"incoherent line size"};
      }
      m_cells.emplace_back();
      m_cells.back().reserve(m_nb_columns);
      auto column = 0;
      for (const auto &c : line) {
        if (c == 'X') {
          m_x_roots.push_back(std::make_shared<Node>(c, row, column));
        }
        m_cells.back().emplace_back(c);
        ++column;
      }
      ++row;
    }
    for (auto &node : m_x_roots) {
      for (auto i = -1; i < 2; ++i) {
        for (auto j = -1; j < 2; ++j) {
          develop(node, i, j);
        }
      }
    }
  }

  QString solveOne() const {
    auto nb_leafs = 0u;
    for (const auto &node : m_x_roots) {
      nb_leafs += node->nbLeafs();
    }
    return QString("%1").arg(nb_leafs);
  }

  QString solveTwo() const {
    auto nb_x_mas = 0u;
    for (auto row = 1; row + 1 < m_nb_rows; ++row) {
      for (auto column = 1; column + 1 < m_nb_columns; ++column) {
        if (m_cells[row][column] != 'A') {
          continue;
        }
        if (m_cells[row - 1][column - 1] != 'M' and
            m_cells[row - 1][column - 1] != 'S') {
          continue;
        }
        if (m_cells[row - 1][column - 1] == 'M' and
            m_cells[row + 1][column + 1] != 'S') {
          continue;
        }
        if (m_cells[row - 1][column - 1] == 'S' and
            m_cells[row + 1][column + 1] != 'M') {
          continue;
        }
        if (m_cells[row - 1][column + 1] != 'M' and
            m_cells[row - 1][column + 1] != 'S') {
          continue;
        }
        if (m_cells[row - 1][column + 1] == 'M' and
            m_cells[row + 1][column - 1] != 'S') {
          continue;
        }
        if (m_cells[row - 1][column + 1] == 'S' and
            m_cells[row + 1][column - 1] != 'M') {
          continue;
        }
        ++nb_x_mas;
      }
    }
    return QString("%1").arg(nb_x_mas);
  }

private:
  void develop(NodePtr node, int i, int j) {
    if ((i == 0 and j == 0) or not node) {
      return;
    }
    const auto next_char = getNext(node->value);
    if (not next_char.has_value()) {
      return;
    }
    const auto child_coordinates =
        Coordinates{node->coordinates.row + i, node->coordinates.column + j};
    if (child_coordinates.row >= 0 and child_coordinates.column >= 0 and
        child_coordinates.row < m_nb_rows and
        child_coordinates.column < m_nb_columns and
        m_cells[child_coordinates.row][child_coordinates.column] ==
            *next_char) {

      auto child = std::make_shared<Node>(*next_char, child_coordinates, node);
      node->children.push_back(child);
      develop(child, i, j);
    }
  }

  int m_nb_rows;
  int m_nb_columns;
  std::vector<std::vector<QChar>> m_cells;
  std::deque<NodePtr> m_x_roots;
};

} // namespace puzzle_2024_04

void Solver_2024_04_1::solve(const QString &input) {
  const auto word_search = puzzle_2024_04::WordSearch{input};
  emit finished(word_search.solveOne());
}

void Solver_2024_04_2::solve(const QString &input) {
  const auto word_search = puzzle_2024_04::WordSearch{input};
  emit finished(word_search.solveTwo());
}
