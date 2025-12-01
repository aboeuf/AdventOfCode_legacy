#include <solvers/common.h>
#include <solvers/2016/puzzle_2016_08.h>

namespace puzzle_2016_08 {

constexpr auto screen_nb_rows = std::size_t{6u};
using Column = std::array<bool, screen_nb_rows>;

constexpr auto screen_nb_columns = std::size_t{50u};
using Row = std::array<bool, screen_nb_columns>;

class Screen {
public:
  Screen() {
    for (auto row_index = 0u; row_index < screen_nb_rows; ++row_index) {
      for (auto column_index = 0u; column_index < screen_nb_columns;
           ++column_index) {
        m_screen[row_index][column_index] = false;
      }
    }
  }

  std::size_t nbPixelsOn() const {
    auto nb_on = 0u;
    for (auto row_index = 0u; row_index < screen_nb_rows; ++row_index) {
      for (auto column_index = 0u; column_index < screen_nb_columns;
           ++column_index) {
        if (m_screen[row_index][column_index]) {
          ++nb_on;
        }
      }
    }
    return nb_on;
  }

  void runInstruction(const QString &instruction) {
    const auto tokens = common::splitValues(instruction, ' ');
    if (tokens.size() == 2 and tokens.front() == "rect") {
      const auto values = common::toVecULongLong(tokens.back(), 'x');
      if (values.size() == 2) {
        return rect(values[0], values[1]);
      }
    } else if (tokens.size() == 5 and tokens.front() == "rotate" and
               tokens[3] == "by") {
      const auto values = common::splitValues(tokens[2], '=');
      if (values.size() == 2) {
        auto ok = true;
        const auto nb_clicks = tokens.back().toULongLong(&ok);
        if (ok) {
          const auto index = values[1].toULongLong(&ok);
          if (ok) {
            if (tokens[1] == "row" and values[0] == "y") {
              return rotateRow(index, nb_clicks);
            } else if (tokens[1] == "column" and values[0] == "x") {
              return rotateColumn(index, nb_clicks);
            }
          }
        }
      }
    }
    common::throwInvalidArgumentError(
        QString("puzzle_2016_08::Screen::runInstruction: invalid instruction "
                "\"%1\"")
            .arg(instruction));
  }

  QString toString() const {
    auto result = QString{};
    for (auto row_index = 0u; row_index < screen_nb_rows; ++row_index) {
      for (auto column_index = 0u; column_index < screen_nb_columns;
           ++column_index) {
        result.push_back(m_screen[row_index][column_index] ? '#' : ' ');
      }
      result.push_back('\n');
    }
    return result;
  }

private:
  void rect(std::size_t nb_columns, std::size_t nb_rows) {
    for (auto row_index = 0u; row_index < std::min(nb_rows, screen_nb_rows);
         ++row_index) {
      for (auto column_index = 0u;
           column_index < std::min(nb_columns, screen_nb_columns);
           ++column_index) {
        m_screen[row_index][column_index] = true;
      }
    }
  }

  Row getRow(std::size_t row_index) {
    auto row = Row{};
    for (auto column_index = 0u; column_index < screen_nb_columns;
         ++column_index) {
      row[column_index] = m_screen[row_index][column_index];
    }
    return row;
  }

  void rotateRow(std::size_t row_index, std::size_t nb_columns) {
    const auto row_copy = getRow(row_index);
    for (auto column_index = 0u; column_index < screen_nb_columns;
         ++column_index) {
      m_screen[row_index][(column_index + nb_columns) % screen_nb_columns] =
          row_copy[column_index];
    }
  }

  Column getColumn(std::size_t column_index) {
    auto column = Column{};
    for (auto row_index = 0u; row_index < screen_nb_rows; ++row_index) {
      column[row_index] = m_screen[row_index][column_index];
    }
    return column;
  }

  void rotateColumn(std::size_t column_index, std::size_t nb_rows) {
    const auto column_copy = getColumn(column_index);
    for (auto row_index = 0u; row_index < screen_nb_rows; ++row_index) {
      m_screen[(row_index + nb_rows) % screen_nb_rows][column_index] =
          column_copy[row_index];
    }
  }

  std::array<Row, screen_nb_rows> m_screen;
};

} // namespace puzzle_2016_08

void Solver_2016_08_1::solve(const QString &input) {
  auto screen = puzzle_2016_08::Screen();
  for (const auto &instruction : common::splitLines(input)) {
    screen.runInstruction(instruction);
  }
  emit finished(QString("%1").arg(screen.nbPixelsOn()));
}

void Solver_2016_08_2::solve(const QString &input) {
  auto screen = puzzle_2016_08::Screen();
  for (const auto &instruction : common::splitLines(input)) {
    screen.runInstruction(instruction);
  }
  emit output(screen.toString());
  emit finished("");
}
