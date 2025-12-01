#include <solvers/2024/puzzle_2024_25.h>
#include <solvers/common.h>

namespace puzzle_2024_25 {

inline bool matches(const QVector<int> &lock, const QVector<int> &key,
                    const int depth) {
  for (auto i = 0; i < lock.size(); ++i) {
    if (lock[i] + key[i] > depth) {
      return false;
    }
  }
  return true;
}

class LockAndKeys {
public:
  LockAndKeys(const QString &input) {
    const auto lines = common::splitLines(input);
    if (lines.isEmpty()) {
      common::throwInvalidArgumentError(
          "puzzle_2024_25::LockAndKeys: empty input");
    }
    m_nb_pins = lines.front().size();
    auto current_char = lines.front().front();
    const auto add_new_object = [this, &current_char]() {
      if (current_char == '#') {
        m_locks << QVector<int>(m_nb_pins, -1);
      } else if (current_char == '.') {
        m_keys << QVector<int>(m_nb_pins, -1);
      } else {
        common::throwInvalidArgumentError(
            QString("puzzle_2024_25::LockAndKeys: unrecognized character '%1'")
                .arg(current_char));
      }
    };
    add_new_object();
    auto current_depth = -2;
    for (auto line = std::cbegin(lines); line != std::cend(lines); ++line) {
      if (line->isEmpty() and std::next(line) != std::cend(lines)) {
        if (std::next(line)->isEmpty()) {
          common::throwInvalidArgumentError(
              "puzzle_2024_25::LockAndKeys: two consecutive empty lines");
        }
        if (m_tumblers_depth.has_value()) {
          if (current_depth != *m_tumblers_depth) {
            common::throwInvalidArgumentError(
                QString("puzzle_2024_25::LockAndKeys: incoherent tumbler "
                        "depth: actual = %1, expected = %2")
                    .arg(current_depth)
                    .arg(*m_tumblers_depth));
          }
        } else {
          m_tumblers_depth = current_depth;
        }
        current_depth = -2;
        current_char = std::next(line)->front();
        add_new_object();
      } else {
        if (line->size() != m_nb_pins) {
          common::throwInvalidArgumentError(
              "puzzle_2024_25::LockAndKeys: incoherent number of pins");
        }
        for (auto i = 0; i < m_nb_pins; ++i) {
          if (line->at(i) == '#') {
            if (current_char == '.') {
              ++m_keys.back()[i];
            } else {
              ++m_locks.back()[i];
            }
          } else if (line->at(i) != '.') {
            common::throwInvalidArgumentError(
                QString(
                    "puzzle_2024_25::LockAndKeys: unrecognized character '%1'")
                    .arg(line->at(i)));
          }
        }
        ++current_depth;
      }
    }
    if (not m_tumblers_depth.has_value()) {
      common::throwInvalidArgumentError(
          "puzzle_2024_25::LockAndKeys: failed to determine tumblers depth");
    }
    if (*m_tumblers_depth < 1) {
      common::throwInvalidArgumentError("puzzle_2024_25::LockAndKeys: tumblers "
                                        "depth must be strictly positive");
    }
  }

  QString solve() const {
    auto nb_matches = 0u;
    for (const auto &lock : m_locks) {
      for (const auto &key : m_keys) {
        if (matches(lock, key, *m_tumblers_depth)) {
          ++nb_matches;
        }
      }
    }
    return QString("%1").arg(nb_matches);
  }

private:
  int m_nb_pins{0};
  QList<QVector<int>> m_locks{};
  QList<QVector<int>> m_keys{};
  std::optional<int> m_tumblers_depth{std::nullopt};
};

} // namespace puzzle_2024_25

void Solver_2024_25_1::solve(const QString &input) {
  const auto locks_and_keys = puzzle_2024_25::LockAndKeys(input);
  emit finished(locks_and_keys.solve());
}

void Solver_2024_25_2::solve(const QString & /*input*/) {
  emit finished("Merry Christmas!");
}
