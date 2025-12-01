#include <map>
#include <set>
#include <solvers/2022/puzzle_2022_15.h>
#include <solvers/common.h>

namespace puzzle_2022_15 {

using Int = long long int;

struct Position {
  Position() = default;
  Position(Int x, Int y) : x{x}, y{y} {}

  Int distance(const Position &other) const {
    return std::abs(x - other.x) + std::abs(y - other.y);
  }

  bool operator<(const Position &other) const {
    if (x == other.x)
      return y < other.y;
    return x < other.x;
  }

  Int x{0};
  Int y{0};
};

class Sensor {
public:
  Sensor() = default;
  Sensor(Int sensor_x, Int sensor_y, Int beacon_x, Int beacon_y)
      : m_position{sensor_x, sensor_y}, m_nearest_beacon{beacon_x, beacon_y},
        m_range{m_position.distance(m_nearest_beacon)} {}

  const Position &position() const { return m_position; }
  const Position &nearestBeacon() const { return m_nearest_beacon; }
  Int range() const { return m_range; }

private:
  Position m_position{};
  Position m_nearest_beacon{};
  Int m_range{0};
};

class Interval {
public:
  Interval(Int min, Int max)
      : m_min{std::min(min, max)}, m_max{std::max(min, max)} {}

  bool canMergeWith(const Interval &other) const {
    if (m_max + 1 == other.min())
      return true;
    if (other.max() + 1 == m_min)
      return true;
    return m_min <= other.max() and other.min() <= m_max;
  }

  void mergeWith(const Interval &other) {
    m_min = std::min(m_min, other.min());
    m_max = std::max(m_max, other.max());
  }

  Int min() const { return m_min; }
  Int max() const { return m_max; }
  Int size() const { return m_max - m_min + 1; }
  bool contains(Int value) const { return m_min <= value and value <= m_max; }

private:
  Int m_min;
  Int m_max;
};

class Coverage {
public:
  Coverage() = default;

  Coverage &operator<<(Interval interval) {
    for (auto it = std::begin(m_intervals); it != std::end(m_intervals);) {
      if (it->canMergeWith(interval)) {
        interval.mergeWith(*it);
        it = m_intervals.erase(it);
      } else {
        ++it;
      }
    }
    for (auto it = std::begin(m_intervals); it != std::end(m_intervals); ++it) {
      if (interval.max() < it->min()) {
        m_intervals.insert(it, interval);
        return *this;
      }
    }
    m_intervals.push_back(interval);
    return *this;
  }

  const std::list<Interval> &intervals() const { return m_intervals; }

  Int size() const {
    auto res = Int{0};
    for (const auto &interval : m_intervals)
      res += interval.size();
    return res;
  }

  bool contains(Int value) const {
    if (m_intervals.empty() or value < m_intervals.front().min() or
        m_intervals.front().max() < value)
      return false;
    for (const auto &interval : m_intervals)
      if (interval.contains(value))
        return true;
    return false;
  }

  Coverage getComplementIn(const Interval &interval) const {
    auto complement = Coverage{};
    if (m_intervals.empty()) {
      complement << interval;
      return complement;
    }
    if (interval.min() < m_intervals.front().min())
      complement << Interval{interval.min(), m_intervals.front().min() - 1};
    for (auto it = std::begin(m_intervals);
         std::next(it) != std::end(m_intervals); ++it) {
      if (interval.max() <= it->max())
        return complement;
      complement << Interval{
          it->max() + 1, std::min(interval.max(), std::next(it)->min() - 1)};
    }
    if (interval.max() > m_intervals.back().max())
      complement << Interval{m_intervals.back().max() + 1, interval.max()};
    return complement;
  }

private:
  std::list<Interval> m_intervals{};
};

class Puzzle {
public:
  Puzzle(const QString &input) {
    const auto lines = common::splitLines(input);
    for (const auto &line : lines) {
      auto rx =
          QRegExp("^Sensor at x=(-{0,1})(\\d+), y=(-{0,1})(\\d+): closest "
                  "beacon is at x=(-{0,1})(\\d+), y=(-{0,1})(\\d+)$");
      if (not rx.exactMatch(line))
        throw std::invalid_argument(
            QString("invalid line \"%1\"").arg(line).toStdString());
      const auto value = [&rx](auto i) {
        if (rx.cap(i) != "" and rx.cap(i) != "-")
          throw std::invalid_argument(QString("invalid sign operator \"%1\"")
                                          .arg(rx.cap(i))
                                          .toStdString());
        bool ok;
        const auto abs_val = static_cast<Int>(rx.cap(i + 1).toLongLong(&ok));
        if (not ok)
          throw std::invalid_argument(
              QString("cannot convert \"%1\" to long long int")
                  .arg(rx.cap(i + 1))
                  .toStdString());
        return (rx.cap(i) == "-" ? Int{-1} : Int{1}) * abs_val;
      };
      const auto sensor_x = value(1);
      const auto sensor_y = value(3);
      const auto beacon_x = value(5);
      const auto beacon_y = value(7);
      m_sensors.emplace_back(sensor_x, sensor_y, beacon_x, beacon_y);
      m_beacons.insert(Position{beacon_x, beacon_y});
    }
  }

  QString solveOne(Int y) const {
    const auto coverage = getXCoverageAt(y);
    auto nb_locations = coverage.size();
    for (const auto &beacon : m_beacons)
      if (beacon.y == y and coverage.contains(beacon.x))
        --nb_locations;
    return QString("%1").arg(nb_locations);
  }

  QString solveTwo(Int coord_max) const {
    if (coord_max < 0)
      return "coord_max < 0";

    const auto search_interval = Interval{0, coord_max};

    auto x_possible_locations = std::unordered_map<Int, Coverage>{};
    const auto get_x_possible_locations_at = [&x_possible_locations,
                                              &search_interval,
                                              this](Int y) -> const Coverage & {
      auto it = x_possible_locations.find(y);
      if (it == std::end(x_possible_locations))
        it =
            x_possible_locations
                .insert({y, getXCoverageAt(y).getComplementIn(search_interval)})
                .first;
      return it->second;
    };

    auto y_possible_locations = std::unordered_map<Int, Coverage>{};
    const auto get_y_possible_locations_at = [&y_possible_locations,
                                              &search_interval,
                                              this](Int x) -> const Coverage & {
      auto it = y_possible_locations.find(x);
      if (it == std::end(y_possible_locations))
        it =
            y_possible_locations
                .insert({x, getYCoverageAt(x).getComplementIn(search_interval)})
                .first;
      return it->second;
    };

    for (auto x = search_interval.min(); x <= search_interval.max(); ++x) {
      const auto &possible_y = get_y_possible_locations_at(x);
      for (const auto &interval : possible_y.intervals()) {
        for (auto y = interval.min(); y <= interval.max(); ++y) {
          const auto &possible_x = get_x_possible_locations_at(y);
          if (possible_x.contains(x))
            return QString("%1").arg(Int{4000000} * x + y);
        }
      }
    }

    return "NO SOLUTION";
  }

private:
  std::optional<Interval> getXCoverageAt(std::size_t i, Int y) const {
    const auto delta_y = abs(y - m_sensors[i].position().y);
    const auto delta_x = m_sensors[i].range() - delta_y;
    if (delta_x < 0)
      return std::nullopt;
    return Interval{m_sensors[i].position().x - delta_x,
                    m_sensors[i].position().x + delta_x};
  }

  Coverage getXCoverageAt(Int y) const {
    auto coverage = Coverage{};
    for (auto i = 0u; i < m_sensors.size(); ++i) {
      const auto &interval = getXCoverageAt(i, y);
      if (interval.has_value())
        coverage << *interval;
    }

    return coverage;
  }

  std::optional<Interval> getYCoverageAt(std::size_t i, Int x) const {
    const auto delta_x = abs(x - m_sensors[i].position().x);
    const auto delta_y = m_sensors[i].range() - delta_x;
    if (delta_y < 0)
      return std::nullopt;
    return Interval{m_sensors[i].position().y - delta_y,
                    m_sensors[i].position().y + delta_y};
  }

  Coverage getYCoverageAt(Int x) const {
    auto coverage = Coverage{};
    for (auto i = 0u; i < m_sensors.size(); ++i) {
      const auto &interval = getYCoverageAt(i, x);
      if (interval.has_value())
        coverage << *interval;
    }
    return coverage;
  }

  std::vector<Sensor> m_sensors;
  std::set<Position> m_beacons;
};

} // namespace puzzle_2022_15

void Solver_2022_15_1::solve(const QString &input) {
  emit finished(puzzle_2022_15::Puzzle(input).solveOne(2000000));
}

void Solver_2022_15_2::solve(const QString &input) {
  emit finished(puzzle_2022_15::Puzzle(input).solveTwo(4000000));
}
