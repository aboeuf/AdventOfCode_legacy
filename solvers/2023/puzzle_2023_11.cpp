#include <set>
#include <solvers/2023/puzzle_2023_11.h>
#include <solvers/common.h>

namespace puzzle_2023_11 {

using Int = unsigned long long int;

struct Galaxy {
  Galaxy() = default;
  Galaxy(Int x, Int y) : x{x}, y{y} {}

  Int x{0};
  Int y{0};
};

Int absDiff(Int a, Int b) { return a > b ? a - b : b - a; }

Int distance(const Galaxy &a, const Galaxy &b) {
  return absDiff(a.x, b.x) + absDiff(a.y, b.y);
}

class Universe {
public:
  Universe(const QString &input) {
    const auto lines = common::splitLines(input);
    m_galaxies.reserve(lines.size() * lines.size());
    auto x = Int{0};
    for (const auto &line : lines) {
      auto y = Int{0};
      for (const auto &c : line) {
        if (c == '#') {
          m_galaxies.emplace_back(x, y);
        }
        ++y;
      }
      ++x;
    }
    m_initial_distances = shortestDistancesSum();
    expand();
    m_expansion_constant = absDiff(shortestDistancesSum(), m_initial_distances);
  }

  Int distancesAfterExpansion(Int expansion_time) const {
    if (expansion_time < Int{2})
      return m_initial_distances;
    return m_initial_distances +
           (expansion_time - Int{1}) * m_expansion_constant;
  }

private:
  void expand() {
    const auto expand_in_one_direction =
        [this](const auto &get_coordinate, const auto &increment_coordinate) {
          auto coordinates = std::set<Int>{};
          auto max = Int{0};
          for (const auto &galaxy : m_galaxies) {
            const auto coordinate = get_coordinate(galaxy);
            coordinates.insert(coordinate);
            max = std::max(max, coordinate);
          }
          for (auto coordinate = Int{0};
               not coordinates.empty() and coordinate <= max; ++coordinate) {
            if (coordinate == *std::begin(coordinates)) {
              coordinates.erase(std::begin(coordinates));
            } else {
              for (auto &galaxy : m_galaxies) {
                if (get_coordinate(galaxy) > coordinate)
                  increment_coordinate(galaxy);
              }
              auto new_coordinates = std::set<Int>{};
              for (auto it = std::begin(coordinates);
                   it != std::end(coordinates); ++it) {
                new_coordinates.insert(*it + 1);
              }
              std::swap(coordinates, new_coordinates);
              coordinate += 1;
              max += 1;
            }
          }
        };

    const auto get_x = [](const Galaxy &galaxy) { return galaxy.x; };
    const auto incr_x = [](Galaxy &galaxy) { ++galaxy.x; };
    expand_in_one_direction(get_x, incr_x);

    const auto get_y = [](const Galaxy &galaxy) { return galaxy.y; };
    const auto incr_y = [](Galaxy &galaxy) { ++galaxy.y; };
    expand_in_one_direction(get_y, incr_y);
  }

  uint shortestDistancesSum() const {
    auto sum = 0u;
    for (auto i = 0u; i < m_galaxies.size(); ++i) {
      for (auto j = i + 1; j < m_galaxies.size(); ++j) {
        const auto dist = distance(m_galaxies[i], m_galaxies[j]);
        sum += dist;
      }
    }
    return sum;
  }

  std::vector<Galaxy> m_galaxies;
  Int m_initial_distances{0};
  Int m_expansion_constant{0};
};

} // namespace puzzle_2023_11

void Solver_2023_11_1::solve(const QString &input) {
  const auto universe = puzzle_2023_11::Universe{input};
  emit finished(QString("%1").arg(universe.distancesAfterExpansion(2)));
}

void Solver_2023_11_2::solve(const QString &input) {
  const auto universe = puzzle_2023_11::Universe{input};
  emit finished(QString("%1").arg(universe.distancesAfterExpansion(1000000)));
}
