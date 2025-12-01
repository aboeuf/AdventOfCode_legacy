#include <functional>
#include <limits>
#include <solvers/2024/puzzle_2024_23.h>
#include <solvers/common.h>

namespace puzzle_2024_23 {

using NodeIndex = std::size_t;

class Nodes {
public:
  void reserve(const int size) { m_labels.reserve(size); }

  NodeIndex add(const QString &label) {
    const auto it = m_indexes.constFind(label);
    if (it == m_indexes.cend()) {
      const auto index = std::size(m_labels);
      m_indexes.insert(label, index);
      m_labels.emplace_back(label);
      return index;
    }
    return *it;
  }

  NodeIndex getIndex(const QString &label) const {
    const auto it = m_indexes.constFind(label);
    if (it == m_indexes.cend()) {
      return std::numeric_limits<NodeIndex>::max();
    }
    return *it;
  }

  const QString &getLabel(const NodeIndex index) const {
    if (index < std::size(m_labels)) {
      return m_labels[index];
    }
    return s_error_label;
  }

private:
  std::vector<QString> m_labels;
  QHash<QString, NodeIndex> m_indexes;
  static QString s_error_label;
};

QString Nodes::s_error_label = QString("out_of_range");

template <typename DataType>
inline QString listToString(const QList<DataType> &list) {
  auto result = QString();
  for (auto it = std::cbegin(list); it != std::cend(list); ++it) {
    result += QString("%1").arg(*it);
    if (std::next(it) != std::cend(list)) {
      result += ",";
    }
  }
  return result;
}

class Clique {
public:
  Clique(QList<NodeIndex> &&indexes) : m_indexes{indexes} {
    std::sort(std::begin(m_indexes), std::end(m_indexes));
  }

  const QList<NodeIndex> &indexes() const { return m_indexes; }

  bool operator==(const Clique &other) const {
    return m_indexes == other.indexes();
  }

  QString toString(const std::optional<Nodes> &nodes = std::nullopt) const {
    if (nodes.has_value()) {
      QStringList labels;
      for (const auto index : m_indexes) {
        labels << nodes->getLabel(index);
      }
      std::sort(std::begin(labels), std::end(labels));
      return listToString(labels);
    }
    return listToString(m_indexes);
  }

private:
  QList<NodeIndex> m_indexes;
};

inline uint qHash(const Clique &cycle) { return qHash(cycle.toString()); }

using IndexSet = QSet<NodeIndex>;
using Graph = QHash<NodeIndex, IndexSet>;

inline void BronKerbosch(const Graph &graph, IndexSet &R, IndexSet &P,
                         IndexSet &X, QSet<Clique> &max_cliques) {
  if (P.isEmpty() and X.isEmpty()) {
    auto indexes = QList<NodeIndex>(std::cbegin(R), std::cend(R));
    max_cliques.insert(Clique(std::move(indexes)));
  }
  while (not P.empty()) {
    const auto v = *std::begin(P);
    const auto &Nv = graph[v];
    auto next_R = R + IndexSet({v});
    auto next_P = P & Nv;
    auto next_X = X & Nv;
    BronKerbosch(graph, next_R, next_P, next_X, max_cliques);
    P.erase(std::begin(P));
    X.insert(v);
  }
}

inline QSet<Clique> getMaxCliques(const Graph &graph) {
  auto max_cliques = QSet<Clique>();
  auto R = IndexSet();
  auto P = IndexSet();
  for (const auto index : graph.keys()) {
    P.insert(index);
  }
  auto X = IndexSet();
  BronKerbosch(graph, R, P, X, max_cliques);
  return max_cliques;
}

class Network {
public:
  Network(const QString &input) {
    const auto lines = common::splitLines(input);
    m_nodes.reserve(2 * lines.size());
    for (const auto &line : lines) {
      const auto tokens = common::splitValues(line, '-');
      const auto vA = m_nodes.add(tokens[0]);
      const auto vB = m_nodes.add(tokens[1]);
      m_graph[vA].insert(vB);
      m_graph[vB].insert(vA);
    }
  }

  QString solveOne() const {
    auto cycles = QSet<Clique>();
    for (auto it = std::begin(m_graph); it != std::end(m_graph); ++it) {
      if (m_nodes.getLabel(it.key()).startsWith("t")) {
        for (auto first = std::cbegin(it.value());
             first != std::cend(it.value()); ++first) {
          for (auto second = std::next(first); second != std::cend(it.value());
               ++second) {
            if (m_graph[*first].contains(*second)) {
              auto indexes = QList<NodeIndex>({it.key(), *first, *second});
              cycles.insert(Clique(std::move(indexes)));
            }
          }
        }
      }
    }
    return QString("%1").arg(cycles.size());
  }

  QString solveTwo() const {
    const auto max_cliques = getMaxCliques(m_graph);
    auto max_it =
        std::max_element(std::cbegin(max_cliques), std::cend(max_cliques),
                         [](const Clique &lhs, const Clique &rhs) {
                           return lhs.indexes().size() < rhs.indexes().size();
                         });
    if (max_it == std::cend(max_cliques)) {
      return "Failure";
    }
    return max_it->toString(m_nodes);
  }

private:
  Nodes m_nodes;
  Graph m_graph;
};

} // namespace puzzle_2024_23

void Solver_2024_23_1::solve(const QString &input) {
  const auto network = puzzle_2024_23::Network(input);
  emit finished(network.solveOne());
}

void Solver_2024_23_2::solve(const QString &input) {
  const auto network = puzzle_2024_23::Network(input);
  emit finished(network.solveTwo());
}
