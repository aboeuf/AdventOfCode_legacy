#include <cmath>
#include <solvers/2017/puzzle_2017_03.h>
#include <solvers/common.h>
#include <solvers/qpoint_hash.hpp>

namespace puzzle_2017_03 {

inline uint getInfiniteNorm(const uint memory_address) {
  if (memory_address <= 1u) {
    return 0u;
  }
  auto closest_odd =
      static_cast<uint>(std::sqrt(static_cast<double>(memory_address - 1u)));
  if (closest_odd % 2u == 0u) {
    --closest_odd;
  }
  return (closest_odd / 2u) + 1u;
}

inline QPoint getCoordinates(const uint memory_address) {
  if (memory_address <= 1u) {
    return QPoint(0, 0);
  }
  const auto n = getInfiniteNorm(memory_address);
  if (memory_address == 4u * n * (n + 1u) + 1u) {
    return QPoint(static_cast<int>(n), -static_cast<int>(n));
  }
  auto prev = 4u * n * (n - 1u) + 1u;
  for (auto i = 0; i < 8; ++i) {
    const auto next = prev + n;
    if (memory_address < next) {
      switch (i) {
      case 0:
        return QPoint(static_cast<int>(n),
                      -static_cast<int>(next - memory_address));
      case 1:
        return QPoint(static_cast<int>(n),
                      static_cast<int>(memory_address - prev));
      case 2:
        return QPoint(static_cast<int>(next - memory_address),
                      static_cast<int>(n));
      case 3:
        return QPoint(-static_cast<int>(memory_address - prev),
                      static_cast<int>(n));
      case 4:
        return QPoint(-static_cast<int>(n),
                      static_cast<int>(next - memory_address));
      case 5:
        return QPoint(-static_cast<int>(n),
                      -static_cast<int>(memory_address - prev));
      case 6:
        return QPoint(-static_cast<int>(next - memory_address),
                      -static_cast<int>(n));
      case 7:
        return QPoint(static_cast<int>(memory_address - prev),
                      -static_cast<int>(n));
      }
    }
    prev = next;
  }
  common::throwRunTimeError("puzzle_2017_03::getCoordinates: bad end");
  return QPoint{};
}

inline uint getManhatanDistance(const QPoint &point) {
  return static_cast<uint>(std::abs(point.x()) + std::abs(point.y()));
}

inline uint getOneNorm(const uint memory_address) {
  return getManhatanDistance(getCoordinates(memory_address));
}

} // namespace puzzle_2017_03

void Solver_2017_03_1::solve(const QString &input) {
  const auto input_value = common::splitLines(input, true).front().toUInt();
  const auto nb_steps = puzzle_2017_03::getOneNorm(input_value);
  return finished(QString("%1").arg(nb_steps));
}

void Solver_2017_03_2::solve(const QString &input) {
  const auto input_value = common::splitLines(input, true).front().toUInt();
  auto memory = QHash<QPoint, uint>{};
  memory[QPoint(0, 0)] = 1u;
  auto current_value = 1u;
  for (auto memory_address = 2u; current_value <= input_value;
       ++memory_address) {
    const auto coordinates = puzzle_2017_03::getCoordinates(memory_address);
    current_value = 0u;
    for (auto dx = -1; dx < 2; ++dx) {
      for (auto dy = -1; dy < 2; ++dy) {
        if (dx != 0 or dy != 0) {
          const auto neighbour_coordinates = coordinates + QPoint(dx, dy);
          if (memory.contains(neighbour_coordinates)) {
            current_value += memory[neighbour_coordinates];
          }
        }
      }
    }
    memory[coordinates] = current_value;
  }
  emit finished(QString("%1").arg(current_value));
}
