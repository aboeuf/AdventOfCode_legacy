#include <solvers/common.h>
#include <solvers/2021/puzzle_2021_01.h>

class SonarSweepReport {
public:
  SonarSweepReport(const QString &input) {
    const auto lines = common::splitLines(input);
    m_values.reserve(lines.size());
    bool conversion_ok;
    for (const auto &line : lines) {
      const auto value = line.toUInt(&conversion_ok);
      if (conversion_ok)
        m_values.push_back(value);
    }
  }

  QString nbIncreasesSimple() const {
    auto nb_increases = 0u;
    for (auto i = 1; i < m_values.size(); ++i)
      if (m_values[i] > m_values[i - 1])
        ++nb_increases;
    return QString{"%1"}.arg(nb_increases);
  }

  QString nbIncreasesWindowed() const {
    if (m_values.empty())
      return 0u;
    auto nb_increases = 0u;
    const auto window_sum = [this](const auto i) {
      return m_values[i] + m_values[i + 1] + m_values[i + 2];
    };
    auto prev_sum = window_sum(0u);
    for (auto i = 1u; i + 2 < m_values.size(); ++i) {
      auto curr_sum = window_sum(i);
      if (curr_sum > prev_sum)
        ++nb_increases;
      std::swap(prev_sum, curr_sum);
    }
    return QString{"%1"}.arg(nb_increases);
  }

private:
  std::vector<uint> m_values{};
};

void Solver_2021_01_1::solve(const QString &input) {
  emit finished(SonarSweepReport{input}.nbIncreasesSimple());
}

void Solver_2021_01_2::solve(const QString &input) {
  emit finished(SonarSweepReport{input}.nbIncreasesWindowed());
}
