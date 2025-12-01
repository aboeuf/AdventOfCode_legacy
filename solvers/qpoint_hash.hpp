#pragma once

#include <QPoint>

inline uint normalizeInt(int value) {
  return value < 0 ? 2u * static_cast<uint>(-value) + 1u
                   : 2u * static_cast<uint>(value);
}

inline uint qHash(const QPoint &point) {
  const auto normalized_x = normalizeInt(point.x());
  const auto normalized_y = normalizeInt(point.y());
  const auto sum = normalized_x + normalized_y;
  return sum * (sum + 1u) / 2u + normalized_y;
}
