#include <QDebug>
#include <deque>
#include <optional>
#include <set>
#include <solvers/2021/puzzle_2021_19.h>
#include <solvers/common.h>

using Int = unsigned long long int;

struct Vec3 {
  Vec3() = default;
  Vec3(int x, int y, int z) : m_x{x}, m_y{y}, m_z{z} {}
  Vec3(const Vec3 &v) : m_x{v.m_x}, m_y{v.m_y}, m_z{v.m_z} {}

  int m_x{0};
  int m_y{0};
  int m_z{0};

  Vec3 operator-() const { return Vec3{-m_x, -m_y, -m_z}; }

  Int dist(const Vec3 &other) const {
    return static_cast<Int>(std::abs(m_x - other.m_x)) +
           static_cast<Int>(std::abs(m_y - other.m_y)) +
           static_cast<Int>(std::abs(m_z - other.m_z));
  }
};

Vec3 operator+(const Vec3 &lhs, const Vec3 &rhs) {
  return Vec3{lhs.m_x + rhs.m_x, lhs.m_y + rhs.m_y, lhs.m_z + rhs.m_z};
}

Vec3 operator-(const Vec3 &lhs, const Vec3 &rhs) {
  return Vec3{lhs.m_x - rhs.m_x, lhs.m_y - rhs.m_y, lhs.m_z - rhs.m_z};
}

Vec3 operator*(int scalar, const Vec3 &point) {
  return Vec3{scalar * point.m_x, scalar * point.m_y, scalar * point.m_z};
}

bool operator==(const Vec3 &lhs, const Vec3 &rhs) {
  return lhs.m_x == rhs.m_x and lhs.m_y == rhs.m_y and lhs.m_z == rhs.m_z;
}

bool operator<(const Vec3 &lhs, const Vec3 &rhs) {
  if (lhs.m_x == rhs.m_x) {
    if (lhs.m_y == rhs.m_y)
      return lhs.m_z < rhs.m_z;
    return lhs.m_y < rhs.m_y;
  }
  return lhs.m_x < rhs.m_x;
}

struct ScanInput {
  ScanInput(uint id) : m_id{id} {}
  uint m_id;
  uint m_size{0};
  QStringList::const_iterator m_begin;
  QStringList::const_iterator m_end;
};

struct Frame {
  Vec3 m_x_axis{};
  Vec3 m_y_axis{};
  Vec3 m_z_axis{};
};

const std::array<Frame, 24> frames = []() {
  std::array<Frame, 24> f{};
  f[0].m_x_axis.m_x = 1;
  f[0].m_y_axis.m_y = 1;
  f[0].m_z_axis.m_z = 1;
  f[8].m_x_axis.m_y = 1;
  f[8].m_y_axis.m_x = -1;
  f[8].m_z_axis.m_z = 1;
  f[16].m_x_axis.m_z = 1;
  f[16].m_y_axis.m_y = 1;
  f[16].m_z_axis.m_x = -1;
  for (auto i = 0; i < 24; ++i) {
    if (i % 8 == 0)
      continue;
    if (i % 4 == 0) {
      f[i].m_x_axis = -f[i - 4].m_x_axis;
      f[i].m_y_axis = f[i - 4].m_y_axis;
      f[i].m_z_axis = -f[i - 4].m_z_axis;
      continue;
    }
    f[i].m_x_axis = f[i - 1].m_x_axis;
    f[i].m_y_axis = f[i - 1].m_z_axis;
    f[i].m_z_axis = -f[i - 1].m_y_axis;
  }
  return f;
}();

class Scanner {
public:
  Scanner() = default;

  Scanner(const ScanInput &input) : m_id{input.m_id} {
    if (m_id == 0)
      m_matching_scan = 0;
    m_beacons.reserve(input.m_size);
    for (auto it = input.m_begin; it != input.m_end; ++it) {
      const auto coordinates = common::toVecUInt(*it);
      if (coordinates.size() > 2)
        m_beacons.emplace_back(coordinates[0], coordinates[1], coordinates[2]);
    }
  }

  Vec3 toWorld(uint beacon_index) const {
    const auto &p = m_beacons[beacon_index % m_beacons.size()];
    const auto &f = frames[m_frame % 24];
    return m_position + p.m_x * f.m_x_axis + p.m_y * f.m_y_axis +
           p.m_z * f.m_z_axis;
  }

  bool match(const Scanner &other) {
    m_matching_scan = std::nullopt;
    m_position = Vec3(0, 0, 0);
    for (m_frame = 0u; m_frame < 24u; ++m_frame) {
      auto translations = std::map<Vec3, uint>{};
      for (auto i = 0u; i < m_beacons.size(); ++i) {
        const auto this_in_world = toWorld(i);
        for (auto j = 0u; j < other.beacons().size(); ++j) {
          const auto other_in_world = other.toWorld(j);
          auto translation =
              translations
                  .insert(std::make_pair(other_in_world - this_in_world, 0))
                  .first;
          ++translation->second;
          if (translation->second >= 12) {
            m_position = translation->first;
            m_matching_scan = other.id();
            return true;
          }
        }
      }
    }
    m_frame = 0u;
    return false;
  }

  uint id() const { return m_id; }
  bool matched() const { return m_matching_scan.has_value(); }
  const std::optional<uint> &matchingScan() const { return m_matching_scan; }
  const Vec3 &position() const { return m_position; }
  const std::vector<Vec3> &beacons() const { return m_beacons; }
  uint frame() const { return m_frame; }

protected:
  uint m_id;
  std::optional<uint> m_matching_scan{std::nullopt};
  std::vector<Vec3> m_beacons{};
  Vec3 m_position{};
  uint m_frame{0};
};

class Map {
public:
  Map(const QString &input) {
    const auto lines = common::splitLines(input);
    auto inputs = QList<ScanInput>{};
    for (auto it = std::cbegin(lines); it != std::cend(lines); ++it) {
      if (it->isEmpty()) {
        if (not inputs.empty())
          inputs.back().m_end = it;
      } else if (it->contains("scanner")) {
        inputs << ScanInput{static_cast<uint>(inputs.size())};
        inputs.back().m_begin = std::next(it);
      } else {
        if (not inputs.empty())
          ++inputs.back().m_size;
      }
    }
    if (not inputs.empty())
      inputs.back().m_end = std::cend(lines);
    m_scanners.reserve(inputs.size());
    for (const auto &scan : inputs)
      m_scanners.emplace_back(scan);
    auto not_matched = std::set<uint>{};
    auto to_be_tested = std::deque<std::pair<uint, uint>>{};
    for (auto i = 1u; i < m_scanners.size(); ++i) {
      to_be_tested.push_back(std::make_pair(i, 0u));
      not_matched.insert(i);
    }
    while (not to_be_tested.empty()) {
      const auto p = to_be_tested.front();
      to_be_tested.pop_front();
      if (m_scanners[p.first].matched())
        continue;
      if (m_scanners[p.first].match(m_scanners[p.second])) {
        not_matched.erase(p.first);
        for (auto i : not_matched)
          to_be_tested.push_back(std::make_pair(i, p.first));
      }
    }
    if (not not_matched.empty()) {
      auto dbg = QString("Failed to match scans:");
      for (auto i : not_matched)
        dbg += QString(" %1").arg(i);
      qDebug() << dbg;
    }
  }

  QString solvePuzzleOne() const {
    auto beacons = std::set<Vec3>{};
    for (const auto &scanner : m_scanners)
      for (auto i = 0u; i < scanner.beacons().size(); ++i)
        beacons.insert(scanner.toWorld(i));
    return QString("%1").arg(beacons.size());
  }

  QString solvePuzzleTwo() const {
    auto max_dist = Int{0};
    for (auto i = 0; i < m_scanners.size(); ++i)
      for (auto j = i + 1; j < m_scanners.size(); ++j)
        max_dist = std::max(
            max_dist, m_scanners[i].position().dist(m_scanners[j].position()));
    return QString("%1").arg(max_dist);
  }

private:
  std::vector<Scanner> m_scanners;
};

void Solver_2021_19_1::solve(const QString &input) {
  emit finished(Map(input).solvePuzzleOne());
}

void Solver_2021_19_2::solve(const QString &input) {
  emit finished(Map(input).solvePuzzleTwo());
}
