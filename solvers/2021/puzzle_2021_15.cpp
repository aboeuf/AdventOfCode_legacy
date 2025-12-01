#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <solvers/common.h>
#include <solvers/2021/puzzle_2021_15.h>

using Int = std::size_t;

class RiskMap {
private:
  using Graph =
      boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
                            boost::no_property,
                            boost::property<boost::edge_weight_t, double>>;

  using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

public:
  RiskMap(const QString &input, Int nb_tiles_per_row) {
    if (input.isEmpty() or nb_tiles_per_row == 0)
      return;
    const auto lines = common::splitLines(input);
    auto risk_levels = std::vector<std::vector<Int>>{};
    risk_levels.reserve(lines.size());
    for (const auto &line : lines) {
      risk_levels.emplace_back();
      risk_levels.back().reserve(line.size());
      for (const auto &chr : line)
        risk_levels.back().emplace_back(
            static_cast<Int>(QString("%1").arg(chr).toInt() - 1));
    }
    const auto get_risk_level = [&risk_levels](auto i, auto j) {
      const auto x = i % risk_levels.size();
      const auto y = j % risk_levels[x].size();
      const auto shift = (i / risk_levels.size()) + (j / risk_levels[x].size());
      return (risk_levels[x][y] + shift) % 9 + 1;
    };

    auto graph = Graph{};
    auto vertices = std::vector<std::vector<Vertex>>{};
    const auto N = nb_tiles_per_row * risk_levels.size();
    vertices.reserve(N);
    for (auto i = 0; i < N; ++i) {
      vertices.emplace_back();
      vertices.back().reserve(N);
      for (auto j = 0; j < N; ++j)
        vertices.back().emplace_back(boost::add_vertex(graph));
    }

    auto weights = boost::get(boost::edge_weight, graph);
    const auto add_edge = [&weights, &graph, &get_risk_level,
                           &vertices](auto pi, auto pj, auto i, auto j) {
      weights[boost::add_edge(vertices[pi][pj], vertices[i][j], graph).first] =
          get_risk_level(i, j);
    };
    for (auto i = 0; i < N; ++i) {
      for (auto j = 0; j < N; ++j) {
        if (i > 0) {
          add_edge(i - 1, j, i, j);
          add_edge(i, j, i - 1, j);
        }
        if (j > 0) {
          add_edge(i, j - 1, i, j);
          add_edge(i, j, i, j - 1);
        }
      }
    }

    auto dist_map = std::vector<int>{};
    dist_map.resize(boost::num_vertices(graph));
    boost::dijkstra_shortest_paths(graph, vertices.front().front(),
                                   boost::distance_map(&dist_map[0]));
    m_lowest = QString("%1").arg(dist_map[vertices.back().back()]);
  }

  QString lowest() const { return m_lowest; }

private:
  QString m_lowest{"Error"};
};

void Solver_2021_15_1::solve(const QString &input) {
  emit finished(RiskMap(input, 1u).lowest());
}

void Solver_2021_15_2::solve(const QString &input) {
  emit finished(RiskMap(input, 5u).lowest());
}
