#include <2021/puzzle_2021_12.h>
#include <common.h>
#include <QSet>
#include <QMap>

bool isSmall(const QString& label)
{
  return std::all_of(std::begin(label),
                     std::end(label),
                     [](const auto& chr) { return chr.isLower(); });
}

struct Node
{
  Node(const QString& label) : m_label{label} {}
  QString m_label;
  QSet<const Node*> m_neighbors;
};

struct Path
{
  bool addV1(const Node* node)
  {
    const auto visited = m_nb_visits.contains(node->m_label);
    if (isSmall(node->m_label) and visited)
      return false;
    if (visited)
      ++m_nb_visits[node->m_label];
    else
      m_nb_visits[node->m_label] = 0u;
    m_nodes << node->m_label;
    return true;
  }

  bool addV2(const Node* node)
  {
    const auto visited = m_nb_visits.contains(node->m_label);
    if (isSmall(node->m_label) and visited) {
      if (node->m_label == "start" or node->m_label == "end")
        return false;
      for (auto it = std::begin(m_nb_visits); it != std::end(m_nb_visits); ++it)
        if (isSmall(it.key()) and it.value() > 1u)
          return false;
    }
    if (visited)
      ++m_nb_visits[node->m_label];
    else
      m_nb_visits[node->m_label] = 1u;
    m_nodes << node->m_label;
    return true;
  }

  QStringList m_nodes{};
  QMap<QString, uint> m_nb_visits{};
};

struct Graph
{
  Graph(const QString& input) {
    auto entries = QList<QStringList>{};
    auto labels = QSet<QString>{};
    const auto lines = common::splitLines(input);
    for (const auto& line : lines) {
      entries << common::splitValues(line, QChar('-'));
      labels.insert(entries.back().front());
      labels.insert(entries.back().back());
    }
    m_nodes.reserve(labels.size());
    for (const auto& label : labels) {
      m_labels[label] = m_nodes.size();
      m_nodes.emplace_back(label);
    }
    for (const auto& entry : entries) {
      get(entry.front())->m_neighbors.insert(get(entry.back()));
      get(entry.back())->m_neighbors.insert(get(entry.front()));
    }
  }

  Node* get(const QString& label) { return &(m_nodes[m_labels[label]]); }
  const Node* get(const QString& label) const { return &(m_nodes[m_labels[label]]); }

  std::list<Path> getPaths(bool v1) const
  {
    const auto add = [&v1](auto& copy, const auto* node)
    {
      if (v1)
        return copy.addV1(node);
      return copy.addV2(node);
    };

    auto paths = std::list<Path>{};
    paths.push_back(Path());
    add(paths.back(), get("start"));
    for (auto it = std::cbegin(paths); it != std::cend(paths);) {
      if (it->m_nodes.empty())
        it = paths.erase(it);
      else if (it->m_nodes.back() == "end")
        ++it;
      else {
        auto has_children = false;
        for (const auto* node : get(it->m_nodes.back())->m_neighbors) {
          auto copy = Path{*it};
          if (add(copy, node)) {
            has_children = true;
            paths.push_back(copy);
          }
        }
        if (has_children)
          it = paths.erase(it);
        else
          ++it;
      }
    }
    return paths;
  }

  QString solve(bool v1) const
  {
    const auto paths = getPaths(v1);
    auto nb_at_end = 0u;
    for (const auto& path : paths)
      if (path.m_nodes.back() == "end")
        ++nb_at_end;
    return QString("%1").arg(nb_at_end);
  }

  std::vector<Node> m_nodes;
  QMap<QString, std::size_t> m_labels;
};

void Solver_2021_12_1::solve(const QString& input)
{
  emit finished(Graph(input).solve(true));
}

void Solver_2021_12_2::solve(const QString& input)
{
  emit finished(Graph(input).solve(false));
}
