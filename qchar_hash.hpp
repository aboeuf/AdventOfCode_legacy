#pragma once

#include <QChar>
#include <QHash>

template <> struct std::hash<QChar> {
  std::size_t operator()(const QChar &c) const noexcept {
    return static_cast<std::size_t>(qHash(c));
  }
};
