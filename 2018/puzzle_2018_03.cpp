#include <2018/puzzle_2018_03.h>
#include <common.h>
#include <QRegExp>
#include <unordered_map>
#include <set>

namespace {

class Claim
{
public:
  Claim() = default;

  Claim(const QString& input) {
    auto rx = QRegExp{"^#([0-9]+) @ ([0-9]+),([0-9]+): ([0-9]+)x([0-9]+)$"};
    m_valid = rx.exactMatch(input);
    if (m_valid)
      for (auto i = 0; i < 5; ++i)
        m_values[i] = rx.cap(i + 1).toUInt();
  }

  bool valid() const { return m_valid; }
  uint id() const { return m_values[0]; }
  uint x() const { return m_values[1]; }
  uint y() const { return m_values[2]; }
  uint width() const { return m_values[3]; }
  uint height() const { return m_values[4]; }

private:
  bool m_valid{false};
  std::array<uint, 5> m_values;
};

class Fabric
{
public:
  Fabric(const QString& input) {
    const auto lines = common::splitLines(input);
    for (const auto& line : lines) {
      auto claim = Claim{line};
      if (!claim.valid()) {
        m_invalidInput = line;
        return;
      }
      m_xmax = std::max(m_xmax, claim.x() + claim.width());
      m_ymax = std::max(m_ymax, claim.y() + claim.height());
      m_claims[claim.id()] = std::move(claim);
    }
    m_fabric.reserve(m_xmax);
    for (auto x = 0u; x < m_xmax; ++x)
      m_fabric.push_back(std::vector<Pixel>{m_ymax, Pixel{}});
    for (const auto& c : m_claims) {
      for (auto x = c.second.x(); x < c.second.x() + c.second.width(); ++x) {
        for (auto y = c.second.y(); y < c.second.y() + c.second.height(); ++y) {
          m_fabric[x][y].m_claims.insert(c.first);
        }
      }
    }
  }

  const QString& invalidInput() const { return m_invalidInput; }

  uint nbTwoOrMore() const
  {
    auto res = 0u;
    for (const auto& row : m_fabric)
      for (const auto& pixel : row)
        if (pixel.m_claims.size() > 1)
          ++res;
    return res;
  }

  QString noOverlapIds() const {
    auto res = QString{""};
    for (const auto& c : m_claims) {
      auto no_overlap = true;
      for (auto x = c.second.x(); no_overlap && x < c.second.x() + c.second.width(); ++x) {
        for (auto y = c.second.y(); no_overlap && y < c.second.y() + c.second.height(); ++y) {
          no_overlap = m_fabric[x][y].m_claims.size() == 1;
        }
      }
      if (no_overlap) {
        if (!res.isEmpty())
          res += ", ";
        res += QString{"%1"}.arg(c.first);
      }
    }
    return res;
  }

private:
  struct Pixel {
    std::set<uint> m_claims{};
  };

  std::unordered_map<uint, Claim> m_claims;
  QString m_invalidInput{""};
  uint m_xmax{0u};
  uint m_ymax{0u};
  std::vector<std::vector<Pixel>> m_fabric;
};

}

void Solver_2018_03_1::solve(const QString& input)
{
  const auto fabric = Fabric{input};
  if (!fabric.invalidInput().isEmpty()) {
    emit finished("Invalid input: " + fabric.invalidInput());
    return;
  }
  emit finished(QString{"%1"}.arg(fabric.nbTwoOrMore()));
}

void Solver_2018_03_2::solve(const QString& input)
{
  const auto fabric = Fabric{input};
  if (!fabric.invalidInput().isEmpty()) {
    emit finished("Invalid input: " + fabric.invalidInput());
    return;
  }
  emit finished(fabric.noOverlapIds());
}

