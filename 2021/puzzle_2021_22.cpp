#include <2021/puzzle_2021_22.h>
#include <common.h>
#include <optional>
#include <initializer_list>
#include <deque>
#include <set>

#include <QDebug>
#include <QPen>
#include <QFile>
#include <QTextStream>

namespace puzzle_2021_22
{

using Int = long long int;
using UInt = unsigned long long int;

struct Cuboid : public std::array<puzzle_2021_22::Int, 6>
{
  Cuboid() : std::array<Int, 6>{0, 0, 0, 0, 0, 0} {}
  Cuboid(std::array<Int, 6>&& arr) : std::array<Int, 6>(std::move(arr)) {}

  bool contains(const Cuboid& other) const
  {
    for (auto i = 0; i < 3; ++i) {
      if ((*this)[2 * i] > other[2 * i] or (*this)[2 * i + 1] < other[2 * i + 1])
        return false;
    }
    return true;
  }

  bool intersects(const Cuboid& other) const
  {
    for (auto i = 0; i < 3; ++i)
      if ((*this)[2 * i] >= other[2 * i + 1] or (*this)[2 * i + 1] <= other[2 * i])
        return false;
    return true;
  }

  UInt volume() const
  {
    auto vol = UInt{1};
    for (auto i = 0; i < 3; ++i)
      vol *= static_cast<Int>((*this)[2 * i + 1] - (*this)[2 * i]);
    return vol;
  }

  bool m_on{true};
};

struct RebootStep : Cuboid
{
  RebootStep(const QString& input) : Cuboid()
  {
    auto rx = QRegExp("^(on|off) x=(-*)(\\d+)\\.\\.(-*)(\\d+),y=(-*)(\\d+)\\.\\.(-*)(\\d+),z=(-*)(\\d+)\\.\\.(-*)(\\d+)$");
    m_valid = rx.exactMatch(input);
    if (not m_valid)
      return;
    m_on = (rx.cap(1) == "on");
    for (auto i = 0; i < 6; ++i) {
      (*this)[i] = rx.cap(2 * (i + 1) + 1).toLongLong(&m_valid);
      if (not m_valid)
        return;
      if (rx.cap(2 * (i + 1)) == "-")
        (*this)[i] = -(*this)[i];
    }
    for (auto i = 0; i < 3; ++i)
      if ((*this)[2 * i] > (*this)[2 * i + 1])
        std::swap((*this)[2 * i], (*this)[2 * i + 1]);
    for (auto i = 0; i < 3; ++i)
      ++(*this)[2 * i + 1];
    m_valid = true;
  }

  bool m_valid;
};

struct Cuboids : public std::list<Cuboid>
{
  Cuboids() : std::list<Cuboid>{} {}

  void add(const Cuboid& cuboid)
  {
    for (auto it = begin(); it != end();) {
      if (it->intersects(cuboid)) {
        if (not cuboid.contains(*it)) {
          auto coords = std::array<std::set<Int>, 3>{};
          for (auto i = 0; i < 3; ++i) {
            coords[i].insert((*it)[2 * i]);
            coords[i].insert((*it)[2 * i + 1]);
            coords[i].insert(cuboid[2 * i]);
            coords[i].insert(cuboid[2 * i + 1]);
          }
          for (auto x = std::begin(coords[0]); std::next(x) != std::end(coords[0]); ++x) {
            for (auto y = std::begin(coords[1]); std::next(y) != std::end(coords[1]); ++y) {
              for (auto z = std::begin(coords[2]); std::next(z) != std::end(coords[2]); ++z) {
                auto new_cuboid = Cuboid{{*x, *std::next(x), *y, *std::next(y), *z, *std::next(z)}};
                if (it->contains(new_cuboid) and not cuboid.contains(new_cuboid))
                  it = std::next(insert(it, new_cuboid));
              }
            }
          }
        }
        it = erase(it);
      } else
        ++it;
    }
    if (cuboid.m_on)
      push_back(cuboid);
  }
};

class Reactor
{
public:
  Reactor(const QString& input, bool initialization)
  {
    const auto lines = common::splitLines(input);
    m_reboot_steps.reserve(lines.size());
    for (const auto& line : lines) {
      if (line.isEmpty())
        continue;
      m_reboot_steps.emplace_back(line);
      if (not m_reboot_steps.back().m_valid) {
        m_invalid_input = "Invalid input: " + line;
        return;
      }
    }
    const auto init_cube = Cuboid({-50, 51, -50, 51, -50, 51});
    for (auto& step : m_reboot_steps)
      if (not initialization or init_cube.contains(step))
        m_cuboids.add(step);
    for (const auto& c : m_cuboids)
      m_volume_on += c.volume();
  }

  const QString& invalidInput() const { return m_invalid_input; }
  QString volumeOn() const { return QString("%1").arg(m_volume_on); }

private:
  QString m_invalid_input{};
  UInt m_volume_on{0};
  std::vector<RebootStep> m_reboot_steps{};
  Cuboids m_cuboids;
};

}

void Solver_2021_22_1::solve(const QString& input)
{
  const puzzle_2021_22::Reactor reactor(input, true);
  if (not reactor.invalidInput().isEmpty()) {
    emit finished(reactor.invalidInput());
    return;
  }
  emit finished(reactor.volumeOn());
}

void Solver_2021_22_2::solve(const QString& input)
{
  const puzzle_2021_22::Reactor reactor(input, false);
  if (not reactor.invalidInput().isEmpty()) {
    emit finished(reactor.invalidInput());
    return;
  }
  emit finished(reactor.volumeOn());
}
