#include <QQueue>
#include <solvers/common.h>
#include <solvers/2023/puzzle_2023_23.h>

#include <QFile>
#include <QTextStream>

namespace puzzle_2023_23 {

using Int = unsigned long long int;

enum class Direction { none, north, west, south, east };

const auto directions = QVector<Direction>{Direction::north, Direction::west,
                                           Direction::south, Direction::east};

inline QVector<Direction> getNextDirections(const QChar &c, bool is_icy) {
  if (c == QChar('#'))
    return QVector<Direction>{};
  if (is_icy) {
    if (c == QChar('^'))
      return QVector<Direction>{Direction::north};
    if (c == QChar('<'))
      return QVector<Direction>{Direction::west};
    if (c == QChar('v'))
      return QVector<Direction>{Direction::south};
    if (c == QChar('>'))
      return QVector<Direction>{Direction::east};
  }
  return directions;
}

inline Direction oppositeDirection(const Direction direction) {
  if (direction == Direction::north)
    return Direction::south;
  if (direction == Direction::west)
    return Direction::east;
  if (direction == Direction::south)
    return Direction::north;
  if (direction == Direction::east)
    return Direction::west;
  return Direction::none;
}

inline QString dirToString(const Direction direction) {
  if (direction == Direction::north)
    return "^";
  if (direction == Direction::west)
    return "<";
  if (direction == Direction::south)
    return "v";
  if (direction == Direction::east)
    return ">";
  return "?";
}

struct Vertex {
  Vertex() = default;
  Vertex(int x, int y) : x{x}, y{y} {}

  Vertex(const Vertex &previous, const Direction next) : Vertex{previous} {
    if (next == Direction::north)
      --x;
    else if (next == Direction::west)
      --y;
    else if (next == Direction::south)
      ++x;
    else if (next == Direction::east)
      ++y;
    else
      common::throwInvalidArgumentError(
          "Vertex::constructor: unknown direction");
  }

  QString toString() const { return QString("%1,%2").arg(x).arg(y); }

  int x{0};
  int y{0};
};

bool operator==(const Vertex &lhs, const Vertex &rhs) {
  return lhs.x == rhs.x and lhs.y == rhs.y;
}

uint qHash(const Vertex &vertex) { return qHash(vertex.toString()); }

class Grid {
public:
  Grid(const QString &input) : m_start{-1, -1}, m_end{-1, -1} {
    const auto lines = common::splitLines(input);
    if (lines.empty())
      common::throwInvalidArgumentError("Grid::constructor: empty");
    m_length = lines.size();
    m_cells.reserve(m_length);
    m_width = lines.front().size();
    for (const auto &line : lines) {
      if (line.size() != m_width)
        common::throwInvalidArgumentError(
            "Grid::constructor: incoherent width");
      m_cells.emplace_back();
      m_cells.back().reserve(m_width);
      for (auto c : line)
        m_cells.back().emplace_back(c);
    }
    for (auto y = 0; y < m_width; ++y) {
      if (m_cells[0][y] == '.') {
        m_start = Vertex(0, y);
        break;
      }
    }
    if (m_start.x < 0 or m_start.y < 0)
      common::throwInvalidArgumentError(
          "Grid::constructor: cannot find start point");
    for (auto y = m_width - 1; y >= 0; --y) {
      if (m_cells[m_length - 1][y] == '.') {
        m_end = Vertex(m_length - 1, y);
        break;
      }
    }
    if (m_end.x < 0 or m_end.y < 0)
      common::throwInvalidArgumentError(
          "Grid::constructor: cannot find end point");
  }

  const Vertex &start() const { return m_start; }
  const Vertex &end() const { return m_end; }
  const std::vector<std::vector<QChar>> &cells() const { return m_cells; }
  int length() const { return m_length; }
  int width() const { return m_width; }

  bool isFree(const Vertex &vertex) const {
    return vertex.x > -1 and vertex.x < m_length and vertex.y > -1 and
           vertex.y < m_width and m_cells[vertex.x][vertex.y] != QChar('#');
  }

private:
  Vertex m_start;
  Vertex m_end;
  std::vector<std::vector<QChar>> m_cells;
  int m_length;
  int m_width;
};

struct Neighbors {
  QSet<Vertex> parents{};
  QSet<Vertex> children{};
};

struct Edge {
  Edge() = default;
  Edge(const Vertex &head, const Vertex &tail) : head{head}, tail{tail} {}

  QString toString() const { return head.toString() + "|" + tail.toString(); }

  Vertex head{};
  Vertex tail{};
};

bool operator==(const Edge &lhs, const Edge &rhs) {
  return lhs.head == rhs.head and lhs.tail == rhs.tail;
}

uint qHash(const Edge &edge) { return qHash(edge.toString()); }

struct Path {
  Path() = default;
  Path(const Vertex &vertex) : last{vertex}, vertices{vertex} {}
  Path(const Path &parent, const Vertex &child, const uint edge_length)
      : last{child}, vertices{parent.vertices},
        length{parent.length + edge_length} {
    vertices << child;
  }

  Vertex last{};
  QSet<Vertex> vertices{};
  uint length{0};
};

class Graph {
public:
  Graph(const QString &input, bool is_icy) : m_grid{input} {
    // Add all nodes
    for (auto i = 0; i < m_grid.length(); ++i) {
      for (auto j = 0; j < m_grid.width(); ++j) {
        const auto head = Vertex{i, j};
        if (m_grid.isFree(head)) {
          for (const auto direction :
               getNextDirections(m_grid.cells()[i][j], is_icy)) {
            const auto tail = Vertex{head, direction};
            if (m_grid.isFree(tail)) {
              m_vertices[head].children << tail;
              m_vertices[tail].parents << head;
              m_edges[Edge{head, tail}] = 1u;
            }
          }
        }
      }
    }

    exportToCSV(
        "/home/aboeuf/Projects/AdventOfCode/python/2023/day_23/before.txt");

    const auto get_prunning_neighbors =
        [this](
            const QHash<Vertex, Neighbors>::iterator &vertex) -> QSet<Vertex> {
      if (vertex->parents.size() != 2 or vertex->children.size() != 2)
        return QSet<Vertex>{};
      auto neighbors = QSet<Vertex>{};
      for (auto &parent : vertex->parents)
        neighbors << parent;
      for (auto &child : vertex->children)
        neighbors << child;
      if (neighbors.size() != 2)
        return QSet<Vertex>{};
      return neighbors;
    };

    // Prune nodes with only one parent and one child
    auto vertex = std::begin(m_vertices);
    for (; vertex != std::end(m_vertices);) {
      const auto neighbors = get_prunning_neighbors(vertex);
      if (neighbors.size() == 2) {
        auto it = std::begin(vertex->parents);
        auto A = m_vertices.find(*it);
        if (A == std::end(m_vertices))
          common::throwRunTimeError("Graph::constructor: cannot find vertex A");

        ++it;
        auto B = m_vertices.find(*it);
        if (B == std::end(m_vertices))
          common::throwRunTimeError("Graph::constructor: cannot find vertex B");

        const auto A_to_vertex = m_edges.find(Edge{A.key(), vertex.key()});
        if (A_to_vertex == std::end(m_edges))
          common::throwRunTimeError(
              "Graph::constructor: cannot find edge A to vertex");

        const auto vertex_to_A = m_edges.find(Edge{vertex.key(), A.key()});
        if (vertex_to_A == std::end(m_edges))
          common::throwRunTimeError(
              "Graph::constructor: cannot find edge vertex to A");

        const auto B_to_vertex = m_edges.find(Edge{B.key(), vertex.key()});
        if (B_to_vertex == std::end(m_edges))
          common::throwRunTimeError(
              "Graph::constructor: cannot find edge B to vertex");

        const auto vertex_to_B = m_edges.find(Edge{vertex.key(), B.key()});
        if (vertex_to_B == std::end(m_edges))
          common::throwRunTimeError(
              "Graph::constructor: cannot find edge vertex to B");

        const auto edge_A_to_B = Edge{A.key(), B.key()};
        const auto weight_A_to_B = A_to_vertex.value() + vertex_to_B.value();

        const auto edge_B_to_A = Edge{B.key(), A.key()};
        const auto weight_B_to_A = B_to_vertex.value() + vertex_to_A.value();

        m_edges.remove(A_to_vertex.key());
        m_edges.remove(vertex_to_A.key());
        m_edges.remove(B_to_vertex.key());
        m_edges.remove(vertex_to_B.key());

        m_edges[edge_A_to_B] = weight_A_to_B;
        m_edges[edge_B_to_A] = weight_B_to_A;

        A->parents.remove(vertex.key());
        B->parents.remove(vertex.key());
        A->children.remove(vertex.key());
        B->children.remove(vertex.key());

        A->parents << B.key();
        A->children << B.key();
        B->parents << A.key();
        B->children << A.key();

        m_vertices.erase(vertex);
        vertex = std::begin(m_vertices);
      } else {
        ++vertex;
      }
    }

    exportToCSV(
        "/home/aboeuf/Projects/AdventOfCode/python/2023/day_23/after.txt");
  }

  QString solve() const {
    auto max_length = 0u;
    auto Q = QQueue<Path>();
    Q.enqueue(Path(m_grid.start()));
    while (not Q.empty()) {
      const auto current = Q.dequeue();
      if (current.last == m_grid.end()) {
        max_length = std::max(max_length, current.length);
      } else {
        const auto parent = m_vertices.find(current.last);
        if (parent == std::end(m_vertices))
          common::throwRunTimeError("Graph::solve: cannot find parent vertex");
        for (auto child = std::begin(parent->children);
             child != std::end(parent->children); ++child) {
          if (not current.vertices.contains(*child)) {
            const auto edge = m_edges.find(Edge(parent.key(), *child));
            if (edge == std::end(m_edges))
              common::throwRunTimeError("Graph::solve: cannot find edge");
            Q.enqueue(Path(current, *child, edge.value()));
          }
        }
      }
    }
    return QString("%1").arg(max_length);
  }

  void exportToCSV(const QString &filepath) const {
    QFile file(filepath);
    if (not file.open(QIODevice::WriteOnly | QIODevice::Text))
      common::throwInvalidArgumentError(
          QString("Graph::exportToCSV: cannot open file \"%1\"").arg(filepath));
    QTextStream out(&file);
    for (auto edge = std::cbegin(m_edges); edge != std::cend(m_edges); ++edge)
      out << edge.key().toString() + "|" << edge.value() << "\n";
    file.close();
  }

private:
  Grid m_grid;
  QHash<Vertex, Neighbors> m_vertices;
  QHash<Edge, uint> m_edges;
};

} // namespace puzzle_2023_23

void Solver_2023_23_1::solve(const QString &input) {
  const auto is_icy = true;
  const auto graph = puzzle_2023_23::Graph{input, is_icy};
  emit finished(graph.solve());
}

void Solver_2023_23_2::solve(const QString &input) {
  const auto is_icy = false;
  const auto graph = puzzle_2023_23::Graph{input, is_icy};
  emit finished(graph.solve());
}
