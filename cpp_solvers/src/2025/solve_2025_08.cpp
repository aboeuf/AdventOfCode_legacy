#include <solvers.hpp>

#include <cmath>
#include <map>
#include <optional>
#include <unordered_set>

namespace puzzle_2025_08 {

constexpr auto max_nb_connections = 1000u;

using Int = unsigned long long int;
using Distances = std::map<double, std::pair<std::size_t, std::size_t>>;
using Circuits =
    std::unordered_map<std::size_t, std::unordered_set<std::size_t>>;

struct JunctionBox {
  JunctionBox(const std::string &input) {
    const auto values = aoc::splitToLongLongInt(input);
    if (values.size() != 3) {
      throw std::invalid_argument("wrong size");
    }
    x = values.at(0);
    y = values.at(1);
    z = values.at(2);
  }

  Int x;
  Int y;
  Int z;
  std::optional<std::size_t> circuit;
  std::unordered_set<std::size_t> neighbors;
};

class Boxes {
public:
  Boxes(const std::string &input) {
    const auto lines = aoc::splitLines(input);
    m_boxes.reserve(lines.size());
    for (const auto &line : lines) {
      m_boxes.emplace_back(line);
    }
    for (auto i = 0u; i < m_boxes.size(); ++i) {
      for (auto j = i + 1; j < m_boxes.size(); ++j) {
        const auto dx = static_cast<double>(m_boxes[i].x) -
                        static_cast<double>(m_boxes[j].x);
        const auto dy = static_cast<double>(m_boxes[i].y) -
                        static_cast<double>(m_boxes[j].y);
        const auto dz = static_cast<double>(m_boxes[i].z) -
                        static_cast<double>(m_boxes[j].z);
        const auto distance = std::sqrt(dx * dx + dy * dy + dz * dz);
        m_distances[distance] = std::make_pair(i, j);
      }
    }
  }

  std::size_t size() const { return m_boxes.size(); }

  const JunctionBox &at(std::size_t i) const { return m_boxes[i]; }

  const Distances &distances() const { return m_distances; }

  const Circuits &circuits() const { return m_circuits; }

  void connect(std::size_t i, std::size_t j) {
    if (m_boxes[i].neighbors.find(j) != std::cend(m_boxes[i].neighbors)) {
      return;
    }
    auto circuit_id = m_next_circuit_id;
    if (m_boxes[i].circuit.has_value()) {
      const auto circuit_i_id = *m_boxes[i].circuit;
      if (m_boxes[j].circuit.has_value()) {
        const auto circuit_j_id = *m_boxes[j].circuit;
        if (circuit_i_id == circuit_j_id) {
          return;
        }
        auto &circuit_i = m_circuits[circuit_i_id];
        auto &circuit_j = m_circuits[circuit_j_id];
        for (const auto box_index : circuit_j) {
          circuit_i.insert(box_index);
          m_boxes[box_index].circuit = circuit_i_id;
        }
        m_circuits.erase(circuit_j_id);
      }
      circuit_id = circuit_i_id;
    } else if (m_boxes[j].circuit.has_value()) {
      circuit_id = *m_boxes[j].circuit;
    } else {
      ++m_next_circuit_id;
    }

    m_boxes[i].circuit = circuit_id;
    m_boxes[i].neighbors.insert(j);
    m_circuits[circuit_id].insert(i);

    m_boxes[j].circuit = circuit_id;
    m_boxes[j].neighbors.insert(i);
    m_circuits[circuit_id].insert(j);
  }

private:
  std::vector<JunctionBox> m_boxes;
  Distances m_distances;
  Circuits m_circuits;
  std::size_t m_next_circuit_id{0};
};

} // namespace puzzle_2025_08

namespace aoc {

Result solve_2025_08(const std::string &input) {
  auto boxes = puzzle_2025_08::Boxes(input);
  auto res = Result();
  auto nb_connections = 0u;
  for (const auto &[_, indexes] : boxes.distances()) {
    boxes.connect(indexes.first, indexes.second);
    ++nb_connections;
    if (nb_connections == puzzle_2025_08::max_nb_connections) {
      auto circuits_ids = std::vector<std::size_t>{};
      circuits_ids.reserve(boxes.circuits().size());
      for (const auto &[id, _] : boxes.circuits()) {
        circuits_ids.emplace_back(id);
      }
      std::sort(std::begin(circuits_ids), std::end(circuits_ids),
                [&boxes](const std::size_t lhs, const std::size_t rhs) {
                  return boxes.circuits().find(rhs)->second.size() <
                         boxes.circuits().find(lhs)->second.size();
                });
      auto res_part_one = 1u;
      for (auto i = 0u; i < std::min(circuits_ids.size(), std::size_t{3});
           ++i) {
        res_part_one *= boxes.circuits().find(circuits_ids[i])->second.size();
      }
      res.part_one_solution = toString(res_part_one);
    }
    if (boxes.circuits().size() == 1 and
        std::cbegin(boxes.circuits())->second.size() == boxes.size()) {
      res.part_two_solution =
          toString(boxes.at(indexes.first).x * boxes.at(indexes.second).x);
      return res;
    }
  }
  return Result("failure");
}

} // namespace aoc
