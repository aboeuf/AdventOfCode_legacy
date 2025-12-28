#include <aoc.hpp>

namespace aoc {

const auto solvers = std::unordered_map<int, std::unordered_map<int, Solver>>{
    {2025, {{1, solve_2025_01}, {2, solve_2025_02}, {8, solve_2025_08}}}};

bool hasSolver(int year, int day) {
    const auto year_it = solvers.find(year);
    if (year_it == solvers.cend()) {
        return false;
    }
    return year_it->second.find(day) != year_it->second.cend();
}

Result solve(int year, int day, const std::string &input) {
  const auto year_it = solvers.find(year);
  if (year_it == solvers.cend()) {
    return Result("cannot find solvers for year " + toString(year));
  }
  const auto day_it = year_it->second.find(day);
  if (day_it == year_it->second.cend()) {
    return Result("cannot find solver for year " + toString(year) +
                  " and day " + toString(day));
  }
  return day_it->second(input);
}

} // namespace aoc
