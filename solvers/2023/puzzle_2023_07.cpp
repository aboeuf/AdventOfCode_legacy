#include <solvers/2023/puzzle_2023_07.h>
#include <solvers/common.h>

namespace puzzle_2023_07 {

using Int = std::size_t;

enum class Type : int {
  None = -1,
  HightCard = 0,
  OnePair = 1,
  TwoPair = 2,
  ThreeOfAKind = 3,
  FullHouse = 4,
  FourOfAKind = 5,
  FiveOfAKind = 6
};

class Helper {
public:
  static const std::vector<QChar> &labels() { return m_index_to_label; }

  static Int nbLabels() { return m_index_to_label.size(); }

  static QChar getLabel(Int index) {
    if (index >= Helper::nbLabels())
      common::throwInvalidArgumentError(
          QString("Helper::getLabel: out-of-range index %1 (nbLabels = )")
              .arg(index)
              .arg(Helper::nbLabels()));
    return m_index_to_label[index];
  }

  static bool isValidLabel(const QChar &label) {
    return m_label_to_index.contains(label);
  }

  static Int getIndex(const QChar &label, bool v1) {
    if (not isValidLabel(label))
      common::throwInvalidArgumentError(
          QString("Helper::getIndex: label '%1' is not valid").arg(label));
    if (v1 or label != 'J')
      return m_label_to_index[label] + 1;
    return 0u;
  }

private:
  static std::vector<QChar> m_index_to_label;
  static QMap<QChar, Int> m_label_to_index;
};

std::vector<QChar> Helper::m_index_to_label = std::vector<QChar>{
    '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};

QMap<QChar, Int> Helper::m_label_to_index = []() {
  auto map = QMap<QChar, Int>{};
  for (auto i = 0u; i < Helper::nbLabels(); ++i)
    map[Helper::getLabel(i)] = i;
  return map;
}();

Type getType(const QString &hand, bool v1) {
  if (v1 or not hand.contains('J')) {
    auto occurences_map = QMap<QChar, uint>{};
    for (const auto &c : hand) {
      if (occurences_map.contains(c))
        occurences_map[c] += 1u;
      else
        occurences_map[c] = 1u;
    }
    auto occurences_vector = std::vector<uint>{};
    occurences_vector.reserve(occurences_map.size());
    for (auto it = occurences_map.begin(); it != occurences_map.end(); ++it)
      occurences_vector.push_back(it.value());
    std::sort(occurences_vector.begin(), occurences_vector.end());
    auto occurences_string = QString("");
    for (const auto occurence : occurences_vector)
      occurences_string += QString("%1").arg(occurence);
    if (occurences_string == "5")
      return Type::FiveOfAKind;
    if (occurences_string == "14")
      return Type::FourOfAKind;
    if (occurences_string == "23")
      return Type::FullHouse;
    if (occurences_string == "113")
      return Type::ThreeOfAKind;
    if (occurences_string == "122")
      return Type::TwoPair;
    if (occurences_string == "1112")
      return Type::OnePair;
    if (occurences_string == "11111")
      return Type::HightCard;
    common::throwInvalidArgumentError(
        QString("getType: bad occurences \"%1\" for hand \"%2\"")
            .arg(occurences_string, hand));
    return Type::None;
  }

  auto highest_type = Type::None;
  for (const auto &label : Helper::labels()) {
    if (label == 'J')
      continue;
    auto new_hand = hand;
    new_hand.replace('J', label);
    const auto type = getType(new_hand, true);
    if (type > highest_type)
      highest_type = type;
  }
  if (highest_type == Type::None)
    common::throwRunTimeError(
        QString("getType: failed to determine highest type for hand \"%1\"")
            .arg(hand));
  return highest_type;
}

class Hand {
public:
  Hand(const QString &input, bool v1) : m_v1{v1} {
    const auto splitted = common::splitValues(input, ' ');
    if (splitted.size() != 2) {
      QString splitted_str;
      QDebug dbg(&splitted_str);
      dbg << splitted;
      common::throwInvalidArgumentError(
          QString("Hand::constructor: bad split %1").arg(splitted_str));
    }
    m_hand = splitted[0];
    if (m_hand.size() != 5)
      common::throwInvalidArgumentError(
          QString("Hand::constructor: invalid hand size \"%1\"").arg(m_hand));
    for (const auto &c : m_hand)
      if (not Helper::isValidLabel(c))
        common::throwInvalidArgumentError(
            QString("Hand::constructor: label '%1' is not valid").arg(c));
    bool ok;
    m_bid = splitted[1].toUInt(&ok);
    if (not ok)
      common::throwInvalidArgumentError(
          QString("Hand::constructor: cannot convert \"%1\" to uint")
              .arg(splitted[1]));
    m_type = getType(m_hand, v1);
  }

  bool v1() const { return m_v1; }
  const QString &hand() const { return m_hand; }
  Int bid() const { return m_bid; }
  Type type() const { return m_type; }

private:
  bool m_v1;
  QString m_hand{};
  Int m_bid{0};
  Type m_type{Type::None};
};

bool operator<(const Hand &lhs, const Hand &rhs) {
  const auto v1 = lhs.v1();
  if (v1 != rhs.v1())
    common::throwInvalidArgumentError("operator < : not the same version");
  if (lhs.type() == rhs.type()) {
    for (auto i = 0; i < 5; ++i) {
      const auto lhs_index = Helper::getIndex(lhs.hand().at(i), v1);
      const auto rhs_index = Helper::getIndex(rhs.hand().at(i), v1);
      if (lhs_index < rhs_index)
        return true;
      if (lhs_index > rhs_index)
        return false;
    }
    return false;
  }
  return lhs.type() < rhs.type();
}

class Hands {
public:
  Hands(const QString &input, bool v1) {
    auto lines = common::splitLines(input);
    m_hands.reserve(lines.size());
    for (const auto &line : lines)
      m_hands.emplace_back(line, v1);
    std::sort(m_hands.begin(), m_hands.end());
  }

  QString getTotalBid() const {
    auto total_bid = 0u;
    for (auto i = 0u; i < m_hands.size(); ++i)
      total_bid += (i + 1u) * m_hands[i].bid();
    return QString("%1").arg(total_bid);
  }

private:
  std::vector<Hand> m_hands;
};

} // namespace puzzle_2023_07

void Solver_2023_07_1::solve(const QString &input) {
  const auto hands = puzzle_2023_07::Hands{input, true};
  emit finished(hands.getTotalBid());
}

void Solver_2023_07_2::solve(const QString &input) {
  const auto hands = puzzle_2023_07::Hands{input, false};
  emit finished(hands.getTotalBid());
}
