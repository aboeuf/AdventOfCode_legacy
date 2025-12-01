#include <solvers/common.h>
#include <solvers/2021/puzzle_2021_03.h>

int nbOnes(const QStringList &diagnostic, int bit_index) {
  auto count = 0;
  for (const auto &binary_entry : diagnostic) {
    if (bit_index < binary_entry.size() and
        binary_entry[bit_index] == QChar('1'))
      ++count;
  }
  return count;
}

struct Gamma : public QChar {
  Gamma(const QStringList &diagnostic, int bit_index)
      : QChar{diagnostic.size() > 2 * nbOnes(diagnostic, bit_index) ? '0'
                                                                    : '1'} {}
};

struct Epsilon : public QChar {
  Epsilon(const QStringList &diagnostic, int bit_index)
      : QChar{diagnostic.size() > 2 * nbOnes(diagnostic, bit_index) ? '1'
                                                                    : '0'} {}
};

template <typename ExpectedChar> QString getRating(QStringList diagnostic) {
  if (diagnostic.empty())
    return QString{};
  const auto nb_bits = diagnostic.front().size();
  for (auto current_bit = 0; diagnostic.size() > 1 and current_bit < nb_bits;
       ++current_bit) {
    const auto expected_char = ExpectedChar{diagnostic, current_bit};
    for (auto it = std::begin(diagnostic); it != std::end(diagnostic);) {
      if ((*it)[current_bit] == expected_char)
        ++it;
      else
        it = diagnostic.erase(it);
    }
  }
  if (diagnostic.empty())
    return QString{};
  return diagnostic.front();
}

struct GammaRate : public QString {
  GammaRate(const QStringList &diagnostic) {
    const auto nb_bits = diagnostic.empty() ? 0 : diagnostic.front().size();
    for (auto i = 0; i < nb_bits; ++i)
      push_back(Gamma{diagnostic, i});
  }
};

struct EpsilonRate : public QString {
  EpsilonRate(const GammaRate &gamma_rate) {
    for (const auto &c : gamma_rate)
      push_back(c == '1' ? '0' : '1');
  }
};

class BinaryDiagnostic {
public:
  BinaryDiagnostic(const QString &input) { m_raw = common::splitLines(input); }

  QString powerConsumption() const {
    if (m_raw.empty())
      return "Empty";

    bool conversion_ok;

    const auto gamma_rate = GammaRate{m_raw};
    const auto gamma_int = gamma_rate.toUInt(&conversion_ok, 2);
    if (not conversion_ok)
      return "Cannot convert gamma_rate: " + gamma_rate;

    const auto epsilon_rate = EpsilonRate{gamma_rate};
    const auto epsilon_int = epsilon_rate.toUInt(&conversion_ok, 2);
    if (not conversion_ok)
      return "Cannot convert epsilon_rate: " + epsilon_rate;

    return QString{"%1"}.arg(gamma_int * epsilon_int);
  }

  QString lifeSupportRating() const {
    if (m_raw.empty())
      return "Empty";

    bool conversion_ok;

    const auto oxygen_generator_rating = getRating<Gamma>(m_raw);
    const auto oxygen_int = oxygen_generator_rating.toUInt(&conversion_ok, 2);
    if (not conversion_ok)
      return "Cannot convert oxygen_generator_rating: " +
             oxygen_generator_rating;

    const auto CO2_srubber_rating = getRating<Epsilon>(m_raw);
    const auto CO2_int = CO2_srubber_rating.toUInt(&conversion_ok, 2);
    if (not conversion_ok)
      return "Cannot convert CO2_srubber_rating: " + CO2_srubber_rating;

    return QString{"%1"}.arg(oxygen_int * CO2_int);
  }

private:
  QStringList m_raw;
};

void Solver_2021_03_1::solve(const QString &input) {
  emit finished(BinaryDiagnostic{input}.powerConsumption());
}

void Solver_2021_03_2::solve(const QString &input) {
  emit finished(BinaryDiagnostic{input}.lifeSupportRating());
}
