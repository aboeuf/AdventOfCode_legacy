#include <solvers/2025/puzzle_2025_08.h>
#include <solvers/common.h>

#include <cmath>
#include <unordered_set>

#include <iostream>

namespace puzzle_2025_08 {

using Int = unsigned long long int;

struct JunctionBox {
  JunctionBox(const QString &input) {
    const auto values = common::toVecULongLong(input);
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
  Boxes(const QString &input) {
    const auto lines = common::splitLines(input);
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
        m_sorted_distances[distance] = std::make_pair(i, j);
      }
    }
  }

  QString solveOne(std::size_t max_nb_connections) {
    auto nb_connections = 0u;
    for (const auto &[_, indexes] : m_sorted_distances) {
      connect(indexes.first, indexes.second);
      ++nb_connections;
      if (nb_connections >= max_nb_connections) {
        break;
      }
    }
    auto circuits_ids = std::vector<std::size_t>{};
    circuits_ids.reserve(m_circuits.size());
    for (const auto &[id, _] : m_circuits) {
      circuits_ids.emplace_back(id);
    }
    std::sort(std::begin(circuits_ids), std::end(circuits_ids),
              [this](const std::size_t lhs, const std::size_t rhs) {
                return m_circuits[rhs].size() < m_circuits[lhs].size();
              });
    auto res = 1u;
    for (auto i = 0u; i < std::min(circuits_ids.size(), std::size_t{3}); ++i) {
      res *= m_circuits[circuits_ids[i]].size();
    }
    return QString("%1").arg(res);
  }

  QString solveTwo() {
    for (const auto &[_, indexes] : m_sorted_distances) {
      connect(indexes.first, indexes.second);
      if (m_circuits.size() == 1 and
          std::cbegin(m_circuits)->second.size() == m_boxes.size()) {
        return QString("%1").arg(m_boxes[indexes.first].x *
                                 m_boxes[indexes.second].x);
      }
    }
    return "failure";
  }

private:
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

  std::vector<JunctionBox> m_boxes;
  std::map<double, std::pair<std::size_t, std::size_t>> m_sorted_distances;
  std::unordered_map<std::size_t, std::unordered_set<std::size_t>> m_circuits;
  std::size_t m_next_circuit_id{0};
};

} // namespace puzzle_2025_08

void Solver_2025_08_1::solve(const QString &input) {
  auto boxes = puzzle_2025_08::Boxes(input);
  emit finished(boxes.solveOne(1000u));
}

void Solver_2025_08_2::solve(const QString &input) {
  auto boxes = puzzle_2025_08::Boxes(input);
  emit finished(boxes.solveTwo());
}
