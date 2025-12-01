#include <memory>
#include <solvers/2021/puzzle_2021_16.h>
#include <solvers/common.h>

using Int = unsigned long long int;

class Packet {
public:
  Packet() = default;

  Packet(QString &binary)
      : m_version{binary.mid(0, 3).toUInt(nullptr, 2)},
        m_type_ID{binary.mid(3, 3).toUInt(nullptr, 2)} {
    binary.remove(0, 6);
    if (m_type_ID == 4) {
      auto last = false;
      auto value = QString{};
      while (not last and binary.size() > 4) {
        last = binary.front() == '0';
        value += binary.mid(1, 4);
        binary.remove(0, 5);
      }
      if (not value.isEmpty())
        m_value = value.toULongLong(nullptr, 2);
    } else {
      const auto length_mode = binary.front() == '0';
      binary.remove(0, 1);
      if (length_mode) {
        const auto sub_packets_length = binary.mid(0, 15).toUInt(nullptr, 2);
        binary.remove(0, 15);
        auto sub_binary = binary.mid(0, sub_packets_length);
        binary.remove(0, sub_packets_length);
        while (sub_binary.size() > 5)
          m_sub_packets << std::make_shared<Packet>(sub_binary);
      } else {
        const auto nb_sub_packets = binary.mid(0, 11).toInt(nullptr, 2);
        binary.remove(0, 11);
        while (m_sub_packets.size() < nb_sub_packets and binary.size() > 5)
          m_sub_packets << std::make_shared<Packet>(binary);
      }
    }
  }

  uint versionSum() const {
    auto sum = m_version;
    for (const auto &p : m_sub_packets)
      sum += p->versionSum();
    return sum;
  }

  Int value() const {
    Int val;
    switch (m_type_ID) {
    case 0u:
      val = 0u;
      for (const auto &p : m_sub_packets)
        val += p->value();
      return val;
    case 1u:
      val = 1u;
      for (const auto &p : m_sub_packets)
        val *= p->value();
      return val;
    case 2u:
      val = std::numeric_limits<Int>::max();
      for (const auto &p : m_sub_packets)
        val = std::min(val, p->value());
      return val;
    case 3u:
      val = 0u;
      for (const auto &p : m_sub_packets)
        val = std::max(val, p->value());
      return val;
    case 4u:
      return m_value;
    case 5u:
      return (m_sub_packets.size() > 1 and
              m_sub_packets[0]->value() > m_sub_packets[1]->value())
                 ? 1u
                 : 0u;
    case 6u:
      return (m_sub_packets.size() > 1 and
              m_sub_packets[0]->value() < m_sub_packets[1]->value())
                 ? 1u
                 : 0u;
    case 7u:
      return (m_sub_packets.size() > 1 and
              m_sub_packets[0]->value() == m_sub_packets[1]->value())
                 ? 1u
                 : 0u;
    }
    return 0u;
  }

private:
  uint m_version{0};
  uint m_type_ID{0};
  Int m_value{0};
  QList<std::shared_ptr<Packet>> m_sub_packets;
};

class Transmission {
public:
  Transmission(const QString &input) {
    auto binary = QString{};
    for (const auto &chr : input) {
      if (chr != '\n')
        binary += QString("%1").arg(QString(chr).toInt(nullptr, 16), 4, 2,
                                    QChar('0'));
    }
    m_packet = Packet(binary);
  }

  QString versionSum() const {
    return QString("%1").arg(m_packet.versionSum());
  }
  QString value() const { return QString("%1").arg(m_packet.value()); }

private:
  Packet m_packet;
};

void Solver_2021_16_1::solve(const QString &input) {
  emit finished(Transmission(input).versionSum());
}

void Solver_2021_16_2::solve(const QString &input) {
  emit finished(Transmission(input).value());
}
