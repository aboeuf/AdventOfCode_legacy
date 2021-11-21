#include <2018/puzzle_2018_02.h>
#include <common.h>
#include <map>

namespace {

class Scan
{
public:
  Scan(const QString& line) {
    for (const auto& c : line) {
      auto it = m_map.insert({c, 0}).first;
      ++(it->second);
    }
  }

  bool hasCount(uint n) const
  {
    for (const auto& p : m_map)
      if (p.second == n)
        return true;
    return false;
  }

private:
  std::map<QChar, uint> m_map;
};

struct Distance
{
  Distance(const QString& id_A, const QString& id_B)
  {
    auto sA = id_A.size();
    auto sB = id_B.size();
    m_count = static_cast<uint>(abs(sA - sB));
    for (auto i = 0; i < std::min(sA, sB); ++i) {
      if (id_A[i] != id_B[i])
        ++m_count;
      else
        m_common.push_back(id_A[i]);
    }
  }

  QString m_common{""};
  uint m_count{0u};
};



}

void Solver_2018_02_1::solve(const QString& input)
{
  const auto lines = common::splitLines(input);
  auto nb_twos = 0u, nb_threes = 0u;
  for (const auto& line : lines) {
    const auto& scan = Scan{line};
    if (scan.hasCount(2))
      ++nb_twos;
    if (scan.hasCount(3))
      ++nb_threes;
  }
  emit finished(QString("%1").arg(nb_twos * nb_threes));
}

void Solver_2018_02_2::solve(const QString& input)
{
  const auto ids = common::splitLines(input);
  for (auto id_A = std::begin(ids); id_A != std::end(ids); ++id_A) {
    for (auto id_B = std::next(id_A); id_B != std::end(ids); ++id_B) {
      const auto dist = Distance{*id_A, *id_B};
      if (dist.m_count == 1) {
        emit finished(dist.m_common);
        return;
      }
    }
  }
  emit finished("Not found");
}

