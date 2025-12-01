#include <set>
#include <solvers/2022/puzzle_2022_13.h>
#include <solvers/common.h>

namespace puzzle_2022_13 {

enum class Order { LOWER, EQUAL, GREATER };

struct Packet {
  Packet(const QString &data) : str{data} {
    if (data.isEmpty())
      throw std::invalid_argument("Packet: empty data");
    if (data.startsWith('[') and data.endsWith(']')) {
      auto counter = 0u;
      auto start = 1;
      for (auto end = 1; end < data.size() - 1; ++end) {
        if (data[end] == ',' and counter == 0u) {
          contents.emplace_back(data.mid(start, end - start));
          start = end + 1;
        } else if (data[end] == '[') {
          ++counter;
        } else if (data[end] == ']') {
          if (counter == 0u)
            throw std::invalid_argument(
                QString("Packet: ill formed data \"%1\"")
                    .arg(data)
                    .toStdString());
          --counter;
        }
      }
      if (start < data.size() - 1)
        contents.emplace_back(data.mid(start, data.size() - start - 1));
      return;
    }
    bool ok;
    value = data.toUInt(&ok);
    if (not ok)
      throw std::invalid_argument(
          QString("Packet: cannot convert \"%1\" to unsigned int")
              .arg(data)
              .toStdString());
  }

  std::vector<Packet> contents{};
  std::optional<uint> value{std::nullopt};
  QString str;
};

inline Order compare(const Packet &left, const Packet &right) {
  if (left.value.has_value() and right.value.has_value()) {
    if (*left.value < *right.value)
      return Order::LOWER;
    if (*left.value > *right.value)
      return Order::GREATER;
    return Order::EQUAL;
  }

  if (left.value.has_value() and not right.value.has_value())
    return compare(Packet{QString("[%1]").arg(left.str)}, right);

  if (not left.value.has_value() and right.value.has_value())
    return compare(left, Packet{QString("[%1]").arg(right.str)});

  for (auto i = 0u; i < std::min(left.contents.size(), right.contents.size());
       ++i) {
    const auto order = compare(left.contents[i], right.contents[i]);
    if (order != Order::EQUAL)
      return order;
  }

  if (left.contents.size() < right.contents.size())
    return Order::LOWER;

  if (left.contents.size() > right.contents.size())
    return Order::GREATER;

  return Order::EQUAL;
}

bool operator==(const Packet &lhs, const Packet &rhs) {
  return compare(lhs, rhs) == Order::EQUAL;
}

bool operator!=(const Packet &lhs, const Packet &rhs) {
  return compare(lhs, rhs) != Order::EQUAL;
}

bool operator<(const Packet &lhs, const Packet &rhs) {
  return compare(lhs, rhs) == Order::LOWER;
}

bool operator>(const Packet &lhs, const Packet &rhs) {
  return compare(lhs, rhs) == Order::GREATER;
}

bool operator<=(const Packet &lhs, const Packet &rhs) {
  return compare(lhs, rhs) != Order::GREATER;
}

bool operator>=(const Packet &lhs, const Packet &rhs) {
  return compare(lhs, rhs) != Order::LOWER;
}

struct PuzzleInput {
  PuzzleInput(const QString &input) {
    const auto lines = common::splitLines(input);
    packets.reserve(2 * (lines.size() + 1) / 3);
    for (const auto &line : lines)
      if (not line.isEmpty())
        packets.emplace_back(line);
  }

  QString solveOne() const {
    auto sum = 0u;
    for (auto i = 0u; 2 * i + 1 < packets.size(); ++i)
      if (packets[2 * i] <= packets[2 * i + 1])
        sum += i + 1;
    return QString("%1").arg(sum);
  }

  QString solveTwo() const {
    const auto divider_packets_A = Packet{"[[2]]"};
    const auto divider_packets_B = Packet{"[[6]]"};
    auto ordered = std::multiset<Packet>();
    ordered.insert(divider_packets_A);
    ordered.insert(divider_packets_B);
    for (const auto &packet : packets)
      ordered.insert(packet);
    auto index = 1u;
    auto product = 1u;
    auto first_found = false;
    for (const auto &packet : ordered) {
      if (packet.str == divider_packets_A.str or
          packet.str == divider_packets_B.str) {
        product *= index;
        if (first_found)
          break;
        first_found = true;
      }
      ++index;
    }
    return QString("%1").arg(product);
  }

  std::vector<Packet> packets;
};

} // namespace puzzle_2022_13

void Solver_2022_13_1::solve(const QString &input) {
  emit finished(puzzle_2022_13::PuzzleInput(input).solveOne());
}

void Solver_2022_13_2::solve(const QString &input) {
  emit finished(puzzle_2022_13::PuzzleInput(input).solveTwo());
}
