#include <solvers/2025/puzzle_2025_03.h>
#include <solvers/common.h>

namespace puzzle_2025_03 {

using Int = unsigned long long;

class BatteryBank {
public:
  BatteryBank(const QString &input) {
    m_data.reserve(input.size());
    auto ok = true;
    for (const auto &c : input) {
      m_data.emplace_back(QString(c).toUShort(&ok));
      if (not ok) {
        throw std::invalid_argument(
            QString("cannot parse input \"%1\" (specifically character '%2')")
                .arg(input)
                .arg(c)
                .toStdString());
      }
    }
  }

  Int getMaxJoltage(std::size_t nb_batteries) const {
    return getMaxJoltageRecursive(nb_batteries, 0, 0);
  }

private:
  Int getMaxJoltageRecursive(std::size_t nb_batteries, std::size_t start,
                             Int joltage) const {
    auto max_value = ushort{0};
    auto max_index = std::size_t{0};
    for (auto i = start; i < m_data.size() - nb_batteries + 1; ++i) {
      if (m_data[i] > max_value) {
        max_value = m_data[i];
        max_index = i;
      }
    }
    joltage = Int{10} * joltage + static_cast<Int>(max_value);
    if (nb_batteries == 1u) {
      return joltage;
    }
    return getMaxJoltageRecursive(nb_batteries - 1, max_index + 1, joltage);
  }

  std::vector<ushort> m_data;
};

class BatteryBanks {
public:
  BatteryBanks(const QString &input) {
    const auto lines = common::splitLines(input);
    m_banks.reserve(lines.size());
    for (const auto &line : lines) {
      m_banks.emplace_back(line);
    }
  }

  QString getMaxJoltage(std::size_t nb_batteries) const {
    auto sum = ulong{0};
    for (const auto &bank : m_banks) {
      sum += bank.getMaxJoltage(nb_batteries);
    }
    return QString("%1").arg(sum);
  }

private:
  std::vector<BatteryBank> m_banks;
};

} // namespace puzzle_2025_03

void Solver_2025_03_1::solve(const QString &input) {
  const auto banks = puzzle_2025_03::BatteryBanks(input);
  emit finished(banks.getMaxJoltage(2));
}

void Solver_2025_03_2::solve(const QString &input) {
  const auto banks = puzzle_2025_03::BatteryBanks(input);
  emit finished(banks.getMaxJoltage(12));
}
