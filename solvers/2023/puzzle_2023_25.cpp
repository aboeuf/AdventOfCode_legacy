#include <QQueue>
#include <solvers/common.h>
#include <limits>
#include <solvers/2023/puzzle_2023_25.h>

namespace puzzle_2023_25 {

struct Edge {
  Edge() = default;
  Edge(const QString &head, const QString &tail) : head{head}, tail{tail} {}
  QString head{""};
  QString tail{""};
};

bool operator==(const Edge &lhs, const Edge &rhs) {
  return (lhs.head == rhs.head and lhs.tail == rhs.tail) or
         (lhs.head == rhs.tail and lhs.tail == rhs.head);
}

uint qHash(const Edge &edge) {
  return std::min(qHash(QString("%1<->%2").arg(edge.head, edge.tail)),
                  qHash(QString("%1<->%2").arg(edge.tail, edge.head)));
}

class Graph {
public:
  Graph(const QString &input) {
    const auto lines = common::splitLines(input);
    for (const auto &line : lines) {
      const auto splitted = common::splitValues(line, ':');
      if (splitted.size() != 2)
        common::throwInvalidArgumentError("Graph::constructor: bad split size");
      const auto node = splitted[0];
      if (not node.isEmpty()) {
        const auto neighbors = common::splitValues(splitted[1].mid(1), ' ');
        for (const auto &neighbor : neighbors) {
          if (not neighbor.isEmpty()) {
            m_data[node] << neighbor;
            m_data[neighbor] << node;
          }
        }
      }
    }
  }

  QString minDegreeNode() const {
    auto min_degree_node = QString("");
    auto min_degree = std::numeric_limits<int>::max();
    for (auto it = std::begin(m_data); it != std::end(m_data); ++it) {
      if (it.value().size() < min_degree) {
        min_degree = it.value().size();
        min_degree_node = it.key();
      }
    }
    return min_degree_node;
  }

  QSet<Edge> edges(const QString &node) const {
    auto edge_set = QSet<Edge>{};
    if (m_data.contains(node)) {
      for (const auto &neighbor : m_data[node])
        edge_set << Edge{node, neighbor};
    }
    return edge_set;
  }

  QSet<QString> dominatingSet(const QString &start_with = QString("")) const {
    const auto first = [&start_with, this]() -> QString {
      if (start_with.isEmpty())
        return m_data.begin().key();
      if (not m_data.contains(start_with))
        common::throwInvalidArgumentError(
            QString("Graph::dominatingSet: cannot find starting node \"%1\"")
                .arg(start_with));
      return QString("");
    }();
    auto dominating_set = QSet<QString>{first};
    auto dominated_nodes = m_data[first];
    auto remaining_nodes =
        QSet<QString>{std::begin(m_data.keys()), std::end(m_data.keys())};
    remaining_nodes -= dominating_set;
    remaining_nodes -= dominated_nodes;
    while (not remaining_nodes.empty()) {
      const auto node = *remaining_nodes.begin();
      remaining_nodes.erase(remaining_nodes.begin());
      const auto undominated_neighbors = m_data[node] - dominating_set;
      dominating_set << node;
      dominated_nodes |= undominated_neighbors;
      remaining_nodes -= undominated_neighbors;
    }
    return dominating_set;
  }

  uint edmondKarpMaxFlow(const QString &s, const QString &t) const {
    return 0u;
  }

  QSet<Edge> minimumSTEdgeCut(const QString &s, const QString &t) const {
    return QSet<Edge>{};
  }

  QSet<Edge> minimumEdgeCut() const {
    const auto min_degree_node = minDegreeNode();
    auto min_cut = edges(min_degree_node);
    auto dominating_set = [this]() {
      for (auto it = std::begin(m_data); it != std::end(m_data); ++it) {
        const auto d_set = dominatingSet(it.key());
        if (d_set.size() > 1)
          return d_set;
      }
      return QSet<QString>{};
    }();
    if (dominating_set.size() < 2)
      return min_cut;
    const auto v = *dominating_set.begin();
    dominating_set.erase(dominating_set.begin());
    for (const auto &w : dominating_set) {
      auto this_cut = minimumSTEdgeCut(v, w);
      if (this_cut.size() <= min_cut.size())
        std::swap(min_cut, this_cut);
    }
    return min_cut;
  }

  QString solve() { return "Default"; }

private:
  QHash<QString, QSet<QString>> m_data;
};

} // namespace puzzle_2023_25

void Solver_2023_25_1::solve(const QString &input) {
  auto graph = puzzle_2023_25::Graph{input};
  emit finished(graph.solve());
}

void Solver_2023_25_2::solve(const QString &input) {
  auto graph = puzzle_2023_25::Graph{input};
  emit finished(graph.solve());
}
