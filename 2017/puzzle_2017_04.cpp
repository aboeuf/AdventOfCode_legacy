#include <2017/puzzle_2017_04.h>
#include <common.h>

namespace puzzle_2017_04 {

class Passphrase {
public:
  Passphrase(const QString &input)
      : m_passphrase{common::splitValues(input, ' ')} {}

  bool isValid(bool v2) const {
    auto words = QSet<QString>();
    for (auto word : m_passphrase) {
      if (v2) {
        std::sort(std::begin(word), std::end(word));
      }
      if (words.contains(word)) {
        return false;
      }
      words.insert(word);
    }
    return true;
  }

private:
  QStringList m_passphrase;
};

class Passphrases {
public:
  Passphrases(const QString &input) {
    const auto lines = common::splitLines(input);
    m_passphrases.reserve(lines.size());
    for (const auto &line : lines) {
      m_passphrases.emplace_back(line);
    }
  }

  QString solve(bool v2) const {
    auto nb_valid = 0u;
    for (const auto &passphrase : m_passphrases) {
      if (passphrase.isValid(v2)) {
        ++nb_valid;
      }
    }
    return QString("%1").arg(nb_valid);
  }

private:
  std::vector<Passphrase> m_passphrases;
};

} // namespace puzzle_2017_04

void Solver_2017_04_1::solve(const QString &input) {
  const auto passphrases = puzzle_2017_04::Passphrases(input);
  emit finished(passphrases.solve(false));
}

void Solver_2017_04_2::solve(const QString &input) {
  const auto passphrases = puzzle_2017_04::Passphrases(input);
  emit finished(passphrases.solve(true));
}
