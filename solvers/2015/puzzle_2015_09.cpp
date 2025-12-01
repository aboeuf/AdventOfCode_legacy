#include <optional>
#include <set>
#include <solvers/2015/puzzle_2015_09.h>
#include <solvers/common.h>

struct ParsedLine {
  ParsedLine(const QString &line) {
    auto rx = QRegExp{"^(.+) to (.+) = (\\d+)$"};
    m_valid = rx.exactMatch(line);
    if (m_valid) {
      m_from = rx.cap(1);
      m_to = rx.cap(2);
      m_dist = rx.cap(3).toULongLong();
    }
  }

  bool m_valid;
  QString m_from;
  QString m_to;
  uint m_dist;
};

using DistMap = std::vector<std::vector<std::optional<uint>>>;

struct Node {
  Node(uint loc_id, uint size, const DistMap &map) : loc_id{loc_id}, dist{0} {
    for (auto i = 0u; i < size; ++i)
      if (i != loc_id and map[loc_id][i])
        children.insert(i);
    for (auto it = children.begin(); it != children.end();) {
      if (not map[loc_id][*it])
        it = children.erase(it);
      else
        ++it;
    }
  }

  Node(uint loc_id, const Node &previous, const DistMap &map) : loc_id{loc_id} {
    dist = previous.dist + *(map[previous.loc_id][loc_id]);
    children = previous.children;
    for (auto it = children.begin(); it != children.end();) {
      if (not map[loc_id][*it])
        it = children.erase(it);
      else
        ++it;
    }
  }

  uint loc_id;
  uint dist;
  std::set<uint> children{};
};

using Comp = std::function<bool(const Node &, const Node &)>;

struct Graph {
public:
  Graph(const QString &input) {
    const auto lines = common::splitLines(input);
    auto parsed = std::vector<ParsedLine>{};
    parsed.reserve(lines.size());
    auto id = uint{0};
    auto ids = QMap<QString, uint>{};
    for (const auto &line : lines) {
      parsed.emplace_back(line);
      if (parsed.back().m_valid) {
        if (not ids.contains(parsed.back().m_from))
          ids[parsed.back().m_from] = id++;
        if (not ids.contains(parsed.back().m_to))
          ids[parsed.back().m_to] = id++;
      } else {
        parsed.pop_back();
      }
    }
    m_size = id;
    m_distances.resize(m_size);
    for (auto i = 0u; i < m_size; ++i) {
      m_distances[i].resize(m_size);
      for (auto j = 0u; j < m_size; ++j)
        m_distances[i][j] = std::nullopt;
    }
    for (const auto &line : parsed) {
      const auto &i = ids[line.m_from];
      const auto &j = ids[line.m_to];
      m_distances[i][j] = line.m_dist;
      m_distances[j][i] = line.m_dist;
    }
  }

  QString shortestPathLenght() const {
    const auto dists = aStar(
        [](const Node &lhs, const Node &rhs) { return lhs.dist < rhs.dist; });
    return dists.empty() ? "none" : QString("%1").arg(dists.front());
  }

  QString longestPathLenght() const {
    const auto dists = aStar(
        [](const Node &lhs, const Node &rhs) { return lhs.dist > rhs.dist; });
    return dists.empty() ? "none" : QString("%1").arg(dists.back());
  }

private:
  std::vector<uint> aStar(const Comp &comp) const {
    auto dists = std::vector<uint>{};
    dists.reserve(m_size);
    for (auto i = 0u; i < m_size; ++i) {
      const auto dist = aStar(i, comp);
      if (dist)
        dists.push_back(*dist);
    }
    std::sort(std::begin(dists), std::end(dists));
    return dists;
  }

  std::optional<uint> aStar(uint i, const Comp &comp) const {
    auto open_set = std::multiset<Node, Comp>{comp};
    open_set.insert(Node(i, m_size, m_distances));
    while (not open_set.empty()) {
      const auto current = *(std::begin(open_set));
      if (current.children.empty())
        return current.dist;
      open_set.erase(std::begin(open_set));
      for (auto &c : current.children)
        if (m_distances[current.loc_id][c])
          open_set.insert(Node(c, current, m_distances));
    }
    return std::nullopt;
  }
  DistMap m_distances;
  uint m_size;
};

void Solver_2015_09_1::solve(const QString &input) {
  emit finished(Graph(input).shortestPathLenght());
}

void Solver_2015_09_2::solve(const QString &input) {
  emit finished(Graph(input).longestPathLenght());
}
