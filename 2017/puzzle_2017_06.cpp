#include <2017/puzzle_2017_06.h>
#include <common.h>

namespace puzzle_2017_06 {

inline QString solve(const QString &input, bool v2) {
  auto banks = common::toUInt(common::splitLines(input, true).front(), '\t');
  if (banks.empty()) {
    common::throwInvalidArgumentError("puzzle_2017_06::solve: empty input");
  }

  const auto get_state = [&banks]() {
    auto result = QString("%1").arg(banks[0]);
    for (auto i = 1; i < banks.size(); ++i) {
      result += QString(",%1").arg(banks[i]);
    }
    return result;
  };

  auto history = QHash<QString, uint>{};
  history[get_state()] = 0u;

  for (auto nb_steps = 1u;; ++nb_steps) {
    auto current_index = 0;
    auto nb_max = banks[0];
    for (auto index = 1; index < banks.size(); ++index) {
      if (banks[index] > nb_max) {
        current_index = index;
        nb_max = banks[index];
      }
    }
    banks[current_index] = 0;
    for (; nb_max > 0u; --nb_max) {
      current_index = (current_index + 1) % banks.size();
      ++banks[current_index];
    }
    const auto state = get_state();
    if (history.contains(state)) {
      return QString("%1").arg(v2 ? nb_steps - history[state] : nb_steps);
    }
    history[state] = nb_steps;
  }
}

} // namespace puzzle_2017_06

void Solver_2017_06_1::solve(const QString &input) {
  emit finished(puzzle_2017_06::solve(input, false));
}

void Solver_2017_06_2::solve(const QString &input) {
  emit finished(puzzle_2017_06::solve(input, true));
}
