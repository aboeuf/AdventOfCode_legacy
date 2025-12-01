#include <solvers/common.h>
#include <set>
#include <solvers/2023/puzzle_2023_03.h>

namespace puzzle_2023_03 {

using Int = unsigned long long int;
using Gear = std::pair<int, int>;

class EngineSchematic {
public:
  EngineSchematic(const QString &input) {
    const auto lines = common::splitLines(input);
    auto data = std::vector<std::vector<QChar>>{};
    auto number_map = std::vector<std::vector<int>>{};
    data.reserve(lines.size());
    number_map.reserve(lines.size());
    for (const auto &line : lines) {
      data.emplace_back();
      number_map.emplace_back();
      data.back().reserve(line.size());
      number_map.back().reserve(line.size());
      for (const auto &c : line) {
        data.back().emplace_back(c);
        number_map.back().emplace_back(-1);
      }
    }
    int nb_rows = static_cast<int>(data.size());
    int nb_columns = static_cast<int>(data.front().size());

    const auto is_adjacent_to_symbol = [&data, nb_rows,
                                        nb_columns](int row, int column) {
      for (auto i = std::max(0, row - 1); i <= std::min(row + 1, nb_rows - 1);
           ++i) {
        for (auto j = std::max(0, column - 1);
             j <= std::min(column + 1, nb_columns - 1); ++j) {
          if (i == row and j == column)
            continue;
          const auto c = data[i][j];
          if (c != '.' and not c.isDigit())
            return true;
        }
      }
      return false;
    };

    const auto end_of_number = [&data, nb_rows, nb_columns](int row,
                                                            int column) {
      return column + 1 >= nb_columns or not data[row][column + 1].isDigit();
    };

    auto parsing_number = false;
    auto is_a_part = false;
    auto number_string = QString("");
    auto start_column = 0;
    for (auto row = 0; row < nb_rows; ++row) {
      for (auto column = 0; column < nb_columns; ++column) {
        const auto c = data[row][column];
        if (c.isDigit()) {
          number_string.push_back(c);
          if (not parsing_number) {
            parsing_number = true;
            start_column = column;
          }
          if (not is_a_part and is_adjacent_to_symbol(row, column))
            is_a_part = true;
        }
        if (end_of_number(row, column)) {
          if (parsing_number and is_a_part) {
            for (auto i = start_column; i <= column; ++i)
              number_map[row][i] = static_cast<int>(std::size(m_numbers));
            m_numbers.emplace_back(number_string.toULongLong());
          }
          parsing_number = false;
          is_a_part = false;
          number_string.clear();
        }
      }
    }

    const auto make_gear = [&data, &number_map, nb_rows, nb_columns](
                               int row, int column) -> std::optional<Gear> {
      if (data[row][column] != '*')
        return std::nullopt;
      auto adjacent_numbers = std::set<int>{};
      for (auto i = std::max(0, row - 1); i <= std::min(row + 1, nb_rows - 1);
           ++i) {
        for (auto j = std::max(0, column - 1);
             j <= std::min(column + 1, nb_columns - 1); ++j) {
          if (i == row and j == column)
            continue;
          const auto number_index = number_map[i][j];
          if (number_index > -1)
            adjacent_numbers.insert(number_index);
        }
      }
      if (adjacent_numbers.size() != 2)
        return std::nullopt;
      auto it = adjacent_numbers.begin();
      return std::make_pair(*it, *std::next(it));
    };

    for (auto row = 0; row < nb_rows; ++row) {
      for (auto column = 0; column < nb_columns; ++column) {
        auto gear = make_gear(row, column);
        if (gear.has_value())
          m_gears.emplace_back(*gear);
      }
    }
  }

  Int sumNumberParts() const {
    auto sum = Int{0};
    for (const auto &number : m_numbers)
      sum += number;
    return sum;
  }

  Int sumGearPowers() const {
    auto sum = Int{0};
    for (const auto &gear : m_gears)
      sum += m_numbers[gear.first] * m_numbers[gear.second];
    return sum;
  }

public:
  std::vector<Int> m_numbers;
  std::vector<Gear> m_gears;
};

} // namespace puzzle_2023_03

void Solver_2023_03_1::solve(const QString &input) {
  const auto schematic = puzzle_2023_03::EngineSchematic(input);
  const auto sum = schematic.sumNumberParts();
  emit finished(QString("%1").arg(sum));
}

void Solver_2023_03_2::solve(const QString &input) {
  const auto schematic = puzzle_2023_03::EngineSchematic(input);
  const auto sum = schematic.sumGearPowers();
  emit finished(QString("%1").arg(sum));
}
