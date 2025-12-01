#include <solvers/2022/puzzle_2022_06.h>
#include <solvers/common.h>

namespace puzzle_2022_06 {

inline int getMarkerIndex(const QString &data, uint marker_lenght) {
  if (marker_lenght == 0u)
    return -1;
  const auto length = static_cast<int>(marker_lenght);
  const auto is_marker = [length, &data](int start_index) {
    for (auto i = 0; i < length; ++i)
      for (auto j = i + 1; j < length; ++j)
        if (data[i + start_index] == data[j + start_index])
          return false;
    return true;
  };
  for (auto i = 0; i + length - 1 < data.size(); ++i)
    if (is_marker(i))
      return i + length;
  return -1;
}

inline int getStartOfPacketIndex(const QString &data) {
  return getMarkerIndex(data, 4u);
}

inline int getStartOfMessageIndex(const QString &data) {
  return getMarkerIndex(data, 14u);
}

} // namespace puzzle_2022_06

void Solver_2022_06_1::solve(const QString &input) {
  emit finished(
      QString("%1").arg(puzzle_2022_06::getStartOfPacketIndex(input)));
}

void Solver_2022_06_2::solve(const QString &input) {
  emit finished(
      QString("%1").arg(puzzle_2022_06::getStartOfMessageIndex(input)));
}
