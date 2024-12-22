#include <2024/puzzle_2024_22.h>
#include <common.h>
#include <deque>

namespace puzzle_2024_22 {

using Int = unsigned long long;

constexpr auto sequence_size = std::size_t{4u};
constexpr auto nb_secret_numbers = 2000u;

inline Int multiply(const Int secret_number, Int operand) {
  return ((secret_number * operand) ^ secret_number) % Int{16777216};
}

inline Int divide(const Int secret_number, Int operand) {
  return ((secret_number / operand) ^ secret_number) % Int{16777216};
}

inline Int getNextSecretNumber(const Int secret_number) {
  return multiply(divide(multiply(secret_number, Int{64}), Int{32}), Int{2048});
}

inline int getLastDigit(const Int n) {
  const auto str = QString("%1").arg(n);
  return str.midRef(str.length() - 1).toInt();
}

class Sequence {
public:
  std::size_t size() const { return std::size(m_sequence); }

  std::deque<int>::const_iterator begin() const {
    return std::cbegin(m_sequence);
  }
  std::deque<int>::const_iterator end() const { return std::cend(m_sequence); }

  void append(const int value) {
    m_sequence.push_back(value);
    if (std::size(m_sequence) > sequence_size) {
      m_sequence.pop_front();
    }
  }

private:
  using Int = unsigned long long;
  std::deque<int> m_sequence;
};

inline bool operator==(const Sequence &lhs, const Sequence &rhs) {
  if (lhs.size() == rhs.size()) {
    auto lhs_it = lhs.begin();
    auto rhs_it = rhs.begin();
    while (lhs_it != lhs.end() and rhs_it != rhs.end()) {
      if (*lhs_it != *rhs_it) {
        return false;
      }
      ++lhs_it;
      ++rhs_it;
    }
  }
  return true;
}

inline uint qHash(const Sequence &sequence) {
  auto str = QString("");
  for (auto it = sequence.begin(); it != sequence.end(); ++it) {
    str += QString("%1").arg(*it);
    if (std::next(it) != sequence.end()) {
      str += ",";
    }
  }
  return qHash(str);
}

struct Price {
  int nb_bananas{0};
  QSet<std::size_t> buyers{};
};

class Prices {
public:
  Prices(const QString &input) {
    auto prices = QHash<Sequence, Price>();
    const auto lines = common::splitLines(input);
    auto buyer_index = 0u;
    auto ok = true;
    for (const auto &line : lines) {
      auto secret_number = line.toULongLong(&ok);
      auto previous = getLastDigit(secret_number);
      if (ok) {
        auto sequence = Sequence();
        for (auto i = 0u; i < nb_secret_numbers; ++i) {
          secret_number = getNextSecretNumber(secret_number);
          const auto current = getLastDigit(secret_number);
          sequence.append(current - previous);
          previous = current;
          if (sequence.size() == sequence_size) {
            auto &price = prices[sequence];
            if (not price.buyers.contains(buyer_index)) {
              price.nb_bananas += current;
              price.buyers.insert(buyer_index);
            }
          }
        }
        m_secret_numbers_sum += secret_number;
        ++buyer_index;
      }
    }
    for (auto it = std::cbegin(prices); it != std::cend(prices); ++it) {
      m_max_nb_bananas = std::max(m_max_nb_bananas, it.value().nb_bananas);
    }
  }

  Int secretNumbersSum() const { return m_secret_numbers_sum; }
  int maxNbBananas() const { return m_max_nb_bananas; }

private:
  Int m_secret_numbers_sum{0};
  int m_max_nb_bananas{0};
};

} // namespace puzzle_2024_22

void Solver_2024_22_1::solve(const QString &input) {
  const auto prices = puzzle_2024_22::Prices(input);
  emit finished(QString("%1").arg(prices.secretNumbersSum()));
}

void Solver_2024_22_2::solve(const QString &input) {
  const auto prices = puzzle_2024_22::Prices(input);
  emit finished(QString("%1").arg(prices.maxNbBananas()));
}
