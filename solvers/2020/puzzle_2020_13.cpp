#include <QVector>
#include <solvers/common.h>
#include <solvers/2020/puzzle_2020_13.h>

namespace puzzle_2020_13 {

using Int = unsigned long long int;

Int gcd(Int a, Int b) {
  if (b == 0)
    return a;
  return gcd(b, a % b);
}

Int lcm(int a, int b) { return (a / gcd(a, b)) * b; }

Int commonCongruence(const QPair<Int, Int> &a, const QPair<Int, Int> &b) {
  const QPair<Int, Int> *A, *B;
  if (a.first < b.first) {
    A = &b;
    B = &a;
  } else {
    A = &a;
    B = &b;
  }
  Int diff = A->first - B->first;
  Int i = 0;
  while ((B->second * i) % A->second != diff) {
    ++i;
  }
  return B->second * i + B->first;
}

void findCongruences(QMultiMap<Int, Int> &input) {
  if (input.isEmpty())
    return;
  auto it = input.begin();
  Int first_id = it.value();
  it = input.erase(it);
  QMultiMap<Int, Int> output;
  for (Int i = 1; it != input.end(); ++i) {
    while (it != input.end()) {
      if ((i * first_id + it.key()) % it.value() == 0) {
        output.insert(it.value(), i);
        it = input.erase(it);
      } else
        ++it;
    }
    it = input.begin();
  }
  std::swap(input, output);
}

struct ShuttleService {
  ShuttleService() = default;
  ShuttleService(const QString &input) {
    const QStringList values = common::splitValues(input);
    m_running.reserve(values.size());
    m_out_of_service.reserve(values.size());
    m_bus.reserve(values.size());
    Int int_val;
    bool ok;
    for (const QString &str_val : values) {
      ok = false;
      if (str_val != "x")
        int_val = str_val.toUInt(&ok);
      if (ok) {
        m_running << static_cast<Int>(m_bus.size());
        m_bus << int_val;
      } else {
        m_out_of_service << static_cast<Int>(m_bus.size());
        m_bus << 0u;
      }
    }
    m_running.shrink_to_fit();
    m_out_of_service.shrink_to_fit();
  }

  Int minWaitFrom(Int timestamp, Int &bus_id) const {
    Int min_wait = std::numeric_limits<Int>::max();
    bus_id = 1u;
    for (Int index : m_running) {
      Int n = timestamp / m_bus[index] + 1;
      Int wait = n * m_bus[index] - timestamp;
      if (wait < min_wait) {
        min_wait = wait;
        bus_id = m_bus[index];
      }
    }
    return min_wait;
  }

  Int companyContest() const {
    QMultiMap<Int, Int> map;
    for (Int index : m_running)
      map.insert(index, m_bus[index]);
    Int first_id = map.begin().value();
    findCongruences(map);
    while (map.size() > 1) {
      auto first = map.begin();
      auto second = std::next(first);
      Int key = lcm(first.key(), second.key());
      Int value = commonCongruence({first.value(), first.key()},
                                   {second.value(), second.key()});
      map.erase(first);
      map.erase(second);
      map.insert(key, value);
    }
    return first_id * map.begin().value();
  }

  QVector<Int> m_running{};
  QVector<Int> m_out_of_service{};
  QVector<Int> m_bus{};
};

ShuttleService readIput(const QString &input, Int &estimate) {
  const QStringList lines = common::splitLines(input);
  bool ok = false;
  if (lines.size() > 0)
    estimate = lines[0].toUInt(&ok);
  if (!ok)
    estimate = std::numeric_limits<Int>::max();
  if (lines.size() > 1)
    return ShuttleService(lines[1]);
  return ShuttleService();
}

} // namespace puzzle_2020_13

void Solver_2020_13_1::solve(const QString &input) {
  using namespace puzzle_2020_13;
  Int estimate, bus_id;
  const ShuttleService service = readIput(input, estimate);
  Int min_wait = service.minWaitFrom(estimate, bus_id);
  emit finished(QString::number(min_wait * bus_id));
}

void Solver_2020_13_2::solve(const QString &input) {
  using namespace puzzle_2020_13;
  Int estimate;
  const ShuttleService service = readIput(input, estimate);
  emit finished(QString::number(service.companyContest()));
}
