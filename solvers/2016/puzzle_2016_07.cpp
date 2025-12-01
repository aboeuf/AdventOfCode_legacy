#include <solvers/2016/puzzle_2016_07.h>
#include <solvers/common.h>

namespace puzzle_2016_07 {

inline bool supportsABBA(const QString &address) {
  auto at_least_one_abba = false;
  auto nb_brackets = 0;
  for (auto i = 0; i + 3 < address.size(); ++i) {
    if (address[i] == '[') {
      ++nb_brackets;
    } else if (address[i] == ']') {
      --nb_brackets;
    } else if (address[i] != address[i + 1] and address[i] == address[i + 3] and
               address[i + 1] == address[i + 2]) {
      if (nb_brackets != 0) {
        return false;
      }
      at_least_one_abba = true;
    }
  }
  return at_least_one_abba;
}

inline bool supportsSSL(const QString &address) {
  auto area_broadcast_accessors = QSet<QString>{};
  auto byte_allocation_blocks = QSet<QString>{};
  auto nb_brackets = 0;
  for (auto i = 0; i + 2 < address.size(); ++i) {
    if (address[i] == '[') {
      ++nb_brackets;
    } else if (address[i] == ']') {
      --nb_brackets;
    } else if (address[i] != address[i + 1] and address[i] == address[i + 2]) {
      const auto token =
          nb_brackets == 0 ? QString("%1%2%1").arg(address[i], address[i + 1])
                           : QString("%1%2%1").arg(address[i + 1], address[i]);
      if (not token.contains('[') and not token.contains(']')) {
        if (nb_brackets == 0) {
          if (byte_allocation_blocks.contains(token)) {
            return true;
          }
          area_broadcast_accessors.insert(token);
        } else {
          if (area_broadcast_accessors.contains(token)) {
            return true;
          }
          byte_allocation_blocks.insert(token);
        }
      }
    }
  }
  return false;
}

} // namespace puzzle_2016_07

void Solver_2016_07_1::solve(const QString &input) {
  auto nb_valid = 0u;
  for (const auto &address : common::splitLines(input)) {
    if (puzzle_2016_07::supportsABBA(address)) {
      ++nb_valid;
    }
  }
  emit finished(QString("%1").arg(nb_valid));
}

void Solver_2016_07_2::solve(const QString &input) {
  auto nb_valid = 0u;
  for (const auto &address : common::splitLines(input)) {
    if (puzzle_2016_07::supportsSSL(address)) {
      ++nb_valid;
    }
  }
  emit finished(QString("%1").arg(nb_valid));
}
