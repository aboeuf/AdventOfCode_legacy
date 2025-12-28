#include <solvers.hpp>

#include <limits>

namespace puzzle_2025_02 {

using Int = long long unsigned int;

class ID {
public:
  ID(Int value)
      : m_integer_value{value}, m_string_value{std::to_string(value)},
        m_size{m_string_value.size()} {}

  Int value() const { return m_integer_value; }

  bool isInvalid(bool twice_only) const {
    const auto max_nb_groups =
        twice_only ? std::min(std::size_t(2), m_size) : m_size;
    for (auto nb_groups = 2u; nb_groups <= max_nb_groups; ++nb_groups) {
      if (m_size % nb_groups != 0) {
        continue;
      }
      const auto group_size = m_size / nb_groups;
      const auto first = m_string_value.substr(0, group_size);
      auto all_equal = true;
      for (auto i = 1u; all_equal and i < nb_groups; ++i) {
        if (m_string_value.substr(i * group_size, group_size) != first) {
          all_equal = false;
        }
      }
      if (all_equal) {
        return true;
      }
    }
    return false;
  }

  Int getNextInvalidID(bool twice_only) const {
    if (m_integer_value < 11) {
      return 11;
    }
    auto min_value = std::numeric_limits<Int>::max();

    const auto check_candidate =
        [this, &min_value, twice_only](const std::string &candidate_pattern,
                                       int nb_groups) {
          if (twice_only and nb_groups != 2) {
            return;
          }
          const auto candidate = aoc::toULongLongInt(
              aoc::repeatedString(candidate_pattern, nb_groups));
          if (candidate > m_integer_value and candidate < min_value) {
            min_value = candidate;
          }
        };

    const auto max_nb_groups =
        twice_only ? std::min(std::size_t(2), m_size) : m_size;
    for (auto nb_groups = 2u; nb_groups <= max_nb_groups; ++nb_groups) {
      const auto group_size = m_size / nb_groups;
      const auto first_group = m_string_value.substr(0, group_size);
      if (first_group == "9") {
        check_candidate("1", m_size + 1);
      }
      check_candidate(first_group, nb_groups);
      check_candidate(std::to_string(aoc::toInt(first_group) + 1), nb_groups);
    }
    if (m_size > 1 and m_size % 2 == 1) {
      const auto nb_zeros = (static_cast<int>(m_size) + 1) / 2 - 1;
      check_candidate("1" + aoc::repeatedString("0", nb_zeros), 2);
    }

    return min_value;
  }

private:
  Int m_integer_value;
  std::string m_string_value;
  std::size_t m_size;
};

class IDRange {
public:
  IDRange(const std::string &input) {
    const auto values = aoc::splitToULongLongInt(input, '-');
    if (std::size(values) != 2u) {
      auto values_str = std::string("(");
      for (auto it = std::cbegin(values); it != std::cend(values); ++it) {
        values_str += std::to_string(*it);
        if (std::next(it) != std::cend(values)) {
          values_str += ", ";
        }
      }
      values_str += ")";
      throw std::invalid_argument("cannot parse range \"" + input +
                                  "\": parsed values are " + values_str);
    }
    m_begin = values.front();
    m_end = values.back();
  }

  Int getInvalidIDsSum(bool twice_only) const {
    auto sum = Int(0);
    auto current = ID(m_begin);
    if (not current.isInvalid(twice_only)) {
      current = current.getNextInvalidID(twice_only);
    }
    while (current.value() <= m_end) {
      sum += current.value();
      current = current.getNextInvalidID(twice_only);
    }
    return sum;
  }

  // private:
  Int m_begin;
  Int m_end;
};

class GiftShopDatabase {
public:
  GiftShopDatabase(const std::string &input) {
    const auto lines = aoc::splitLines(input);
    if (std::size(lines) != 1u) {
      throw std::invalid_argument("invalid number of lines (expected 1, got " +
                                  std::to_string(std::size(lines)) + ")");
    }
    const auto ranges_inputs = aoc::split(lines.front());
    m_ID_ranges.reserve(std::size(ranges_inputs));
    for (const auto &range_input : ranges_inputs) {
      m_ID_ranges.emplace_back(range_input);
    }
  }

  Int solve(bool twice_only) const {
    auto sum = Int(0);
    for (const auto &range : m_ID_ranges) {
      sum += range.getInvalidIDsSum(twice_only);
    }
    return sum;
  }

private:
  std::vector<IDRange> m_ID_ranges;
};

} // namespace puzzle_2025_02

namespace aoc {

Result solve_2025_02(const std::string &input) {
  const auto db = puzzle_2025_02::GiftShopDatabase(input);
  auto res = Result();
  res.part_one_solution = std::to_string(db.solve(true));
  res.part_two_solution = std::to_string(db.solve(false));
  return res;
}

} // namespace aoc
