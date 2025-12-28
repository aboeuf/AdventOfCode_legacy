#include <solvers/2025/puzzle_2025_11.h>
#include <solvers/common.h>

#include <map>
#include <set>

namespace puzzle_2025_11 {

using Int = unsigned long long int;

class Graph {
public:
  Graph(const QString &input) {
    const auto lines = common::splitLines(input);
    for (const auto &line : lines) {
      const auto values = common::splitValues(line, ' ');
      auto it = std::cbegin(values);
      const auto from = it->mid(0, it->size() - 1).toStdString();
      ++it;
      m_nodes.try_emplace(from, std::set<std::string>());
      for (; it != std::cend(values); ++it) {
        const auto to = it->toStdString();
        m_nodes.try_emplace(to, std::set<std::string>());
        m_nodes[from].insert(to);
      }
    }
  }

  Int getNbPaths(const std::string &source, const std::string &target) const {
    auto cache = std::map<std::string, Int>{};
    std::function<Int(const std::string &)> count_paths_rec;
    count_paths_rec = [this, &target, &cache,
                       &count_paths_rec](const std::string &u) {
      if (u == target) {
        return Int(1);
      }
      auto [it, is_new] = cache.emplace(u, Int(0));
      if (is_new) {
        for (const auto &v : m_nodes.find(u)->second) {
          it->second += count_paths_rec(v);
        }
      }
      return it->second;
    };
    return count_paths_rec(source);
  }

  QString solveOne() const {
    return QString("%1").arg(getNbPaths("you", "out"));
  }

  QString solveTwo() const {
    return QString("%1").arg(
        getNbPaths("svr", "fft") * getNbPaths("fft", "dac") *
            getNbPaths("dac", "out") +
        getNbPaths("svr", "dac") * getNbPaths("dac", "fft") *
            getNbPaths("fft", "out"));
  }

private:
  std::map<std::string, std::set<std::string>> m_nodes;
  mutable std::map<std::string, std::map<std::string, Int>> m_cached_nb_paths;
};

} // namespace puzzle_2025_11

void Solver_2025_11_1::solve(const QString &input) {
  const auto graph = puzzle_2025_11::Graph(input);
  emit finished(graph.solveOne());
}

void Solver_2025_11_2::solve(const QString &input) {
  const auto graph = puzzle_2025_11::Graph(input);
  emit finished(graph.solveTwo());
}
