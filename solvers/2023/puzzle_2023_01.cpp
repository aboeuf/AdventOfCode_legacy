#include <solvers/2023/puzzle_2023_01.h>
#include <solvers/common.h>

#include <QRegExp>
#include <fstream>

namespace puzzle_2023_01 {

const auto map = QMap<QString, unsigned int>{
    {"one", 1u}, {"two", 2u},   {"three", 3u}, {"four", 4u}, {"five", 5u},
    {"six", 6u}, {"seven", 7u}, {"eight", 8u}, {"nine", 9u},
};

inline unsigned int stringToInt(const QString &s) {
  if (map.contains(s))
    return map.value(s);
  return s.toUInt();
}

class CalibrationEntry {
public:
  CalibrationEntry(const QString &input, bool v1) : m_raw{input} {
    if (v1) {
      for (auto it = std::begin(input); it != std::end(input); ++it) {
        if (it->isDigit()) {
          m_first = QString(*it).toUInt();
          break;
        }
      }
      for (auto it = std::rbegin(input); it != std::rend(input); ++it) {
        if (it->isDigit()) {
          m_last = QString(*it).toUInt();
          break;
        }
      }
    } else {
      auto rx = QRegExp{"(one|two|three|four|five|six|seven|eight|nine|\\d)"};
      rx.indexIn(input);
      m_first = stringToInt(*std::next(std::begin(rx.capturedTexts())));
      for (auto position = input.size() - 1; position > -1; --position) {
        if (rx.indexIn(input.mid(position)) > -1) {
          m_last = stringToInt(*std::next(std::begin(rx.capturedTexts())));
          break;
        }
      }
    }
  }

  unsigned int value() const { return 10u * m_first + m_last; }

  QString output() const {
    auto out = m_raw + QString(" -> %1\n").arg(value());
    return out;
  }

private:
  QString m_raw;
  unsigned int m_first{0u};
  unsigned int m_last{0u};
};

class CalibrationDocument {
public:
  CalibrationDocument(const QString &input, bool v1) {
    const auto lines = common::splitLines(input);
    m_entries.reserve(lines.size());
    for (const auto &line : lines)
      m_entries.emplace_back(line, v1);
  }

  unsigned int value() const {
    auto sum = 0u;
    for (const auto &entry : m_entries)
      sum += entry.value();
    return sum;
  }

  QString output() const {
    auto out = QString{};
    for (const auto &entry : m_entries)
      out += entry.output();
    return out;
  }

private:
  std::vector<CalibrationEntry> m_entries;
};

} // namespace puzzle_2023_01

void Solver_2023_01_1::solve(const QString &input) {
  const auto document = puzzle_2023_01::CalibrationDocument{input, true};
  emit output(document.output());
  emit finished(QString{"%1"}.arg(document.value()));
}

void Solver_2023_01_2::solve(const QString &input) {
  const auto document = puzzle_2023_01::CalibrationDocument{input, false};
  emit output(document.output());
  emit finished(QString{"%1"}.arg(document.value()));
}
