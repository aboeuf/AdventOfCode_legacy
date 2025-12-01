#include <solvers/2025/puzzle_2025_01.h>
#include <solvers/common.h>

namespace puzzle_2025_01 {

class Line {
public:
  Line(const QString &input) : m_line{input} {}

private:
  QString m_line;
};

class Document {
public:
  Document(const QString &input) {
    auto dial_position = 50;
    auto ok = true;
    const auto lines = common::splitLines(input);
    for (const auto &line : lines) {
      const auto sign =
          line.startsWith("R") ? 1 : (line.startsWith("L") ? -1 : 0);
      if (sign == 0) {
        throw std::invalid_argument(
            QString("invalid line \"%1\" (bad direction)")
                .arg(line)
                .toStdString());
      }
      const auto increment = line.mid(1).toInt(&ok);
      if (not ok) {
        throw std::invalid_argument(QString("invalid line \"%1\" (bad number)")
                                        .arg(line)
                                        .toStdString());
      }
      for (auto i = 0; i < increment; ++i) {
        dial_position = (dial_position + sign) % 100;
        if (dial_position == 0) {
          ++m_second_secret_code;
        }
      }
      if (dial_position == 0) {
        ++m_first_secret_code;
      }
    }
  }

  QString firstSecretCode() const {
    return QString("%1").arg(m_first_secret_code);
  }

  QString secondSecretCode() const {
    return QString("%1").arg(m_second_secret_code);
  }

private:
  uint m_first_secret_code{0u};
  uint m_second_secret_code{0u};
};

} // namespace puzzle_2025_01

void Solver_2025_01_1::solve(const QString &input) {
  const auto document = puzzle_2025_01::Document(input);
  emit finished(document.firstSecretCode());
}

void Solver_2025_01_2::solve(const QString &input) {
  const auto document = puzzle_2025_01::Document(input);
  emit finished(document.secondSecretCode());
}
