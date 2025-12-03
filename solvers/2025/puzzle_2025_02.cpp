#include <solvers/2025/puzzle_2025_02.h>
#include <solvers/common.h>

#include <limits>

namespace puzzle_2025_02 {

using Int = long long unsigned int;

inline QString toString(Int value) { return QString("%1").arg(value); }

inline Int toInt(const QString &str) {
  auto ok = true;
  const auto value = str.toULongLong(&ok);
  if (not ok) {
    throw std::invalid_argument{
        QString("invalid input value \"%1\"").arg(str).toStdString()};
  }
  return value;
}

class ID {
public:
  ID(Int value)
      : m_integer_value{value}, m_string_value{toString(value)},
        m_size{static_cast<uint>(m_string_value.size())} {}

  Int value() const { return m_integer_value; }

  bool isInvalid(bool twice_only) const {
    const auto max_nb_groups = twice_only ? std::min(2u, m_size) : m_size;
    for (auto nb_groups = 2u; nb_groups <= max_nb_groups; ++nb_groups) {
      if (m_size % nb_groups != 0) {
        continue;
      }
      const auto group_size = m_size / nb_groups;
      const auto first = m_string_value.mid(0, group_size);
      auto all_equal = true;
      for (auto i = 1u; all_equal and i < nb_groups; ++i) {
        if (m_string_value.mid(i * group_size, group_size) != first) {
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

    const auto check_candidate = [this, &min_value,
                                  twice_only](const QString &candidate_pattern,
                                              int nb_groups) {
      if (twice_only and nb_groups != 2) {
        return;
      }
      const auto candidate = toInt(candidate_pattern.repeated(nb_groups));
      if (candidate > m_integer_value and candidate < min_value) {
        min_value = candidate;
      }
    };

    const auto max_nb_groups = twice_only ? std::min(2u, m_size) : m_size;
    for (auto nb_groups = 2u; nb_groups <= max_nb_groups; ++nb_groups) {
      const auto group_size = m_size / nb_groups;
      const auto first_group = m_string_value.mid(0, group_size);
      if (first_group == "9") {
        check_candidate("1", m_size + 1);
      }
      check_candidate(first_group, nb_groups);
      check_candidate(toString(toInt(first_group) + 1), nb_groups);
    }
    if (m_size > 1 and m_size % 2 == 1) {
      const auto nb_zeros = (static_cast<int>(m_size) + 1) / 2 - 1;
      check_candidate(QString("1") + QString("0").repeated(nb_zeros), 2);
    }

    return min_value;
  }

private:
  Int m_integer_value;
  QString m_string_value;
  uint m_size;
};

class IDRange {
public:
  IDRange(const QString &input) {
    const auto values = common::toVecULongLong(input, '-');
    if (std::size(values) != 2u) {
      throw std::invalid_argument{
          QString("cannot parse range \"%1\"").arg(input).toStdString()};
    }
    m_begin = values.at(0);
    m_end = values.at(1);
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

private:
  Int m_begin;
  Int m_end;
};

class GiftShopDatabase {
public:
  GiftShopDatabase(const QString &input) {
    const auto lines = common::splitLines(input);
    if (std::size(lines) != 1) {
      throw std::invalid_argument{"invalid number of lines"};
    }
    const auto ranges_inputs = common::splitValues(lines.front(), ',');
    m_ID_ranges.reserve(std::size(ranges_inputs));
    for (const auto &range_input : ranges_inputs) {
      m_ID_ranges.emplace_back(range_input);
    }
  }

  QString solve(bool twice_only) const {
    auto sum = Int{0};
    for (const auto &range : m_ID_ranges) {
      sum += range.getInvalidIDsSum(twice_only);
    }
    return QString("%1").arg(sum);
  }

private:
  std::vector<IDRange> m_ID_ranges;
};

inline bool debug(Int start, Int end, bool twice_only) {
  auto next = ID(start).getNextInvalidID(twice_only);
  for (auto i = start; i <= end; ++i) {
    const auto id = ID(i);
    if (id.isInvalid(twice_only)) {
      if (i != next) {
        return false;
      }
      next = id.getNextInvalidID(twice_only);
    } else if (id.getNextInvalidID(twice_only) != next) {
      return false;
    }
  }
  return true;
}

} // namespace puzzle_2025_02

void Solver_2025_02_1::solve(const QString &input) {
  const auto db = puzzle_2025_02::GiftShopDatabase(input);
  emit finished(db.solve(true));
}

void Solver_2025_02_2::solve(const QString &input) {
  const auto db = puzzle_2025_02::GiftShopDatabase(input);
  emit finished(db.solve(false));
}
