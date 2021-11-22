#include <2018/puzzle_2018_05.h>
#include <common.h>
#include <set>

bool operator /(const QChar& lhs, const QChar& rhs) {
  return (lhs.isLower() ^ rhs.isLower()) and (lhs.toLower() == rhs.toLower());
}

class Polymer : public std::list<QChar>
{
public:
  Polymer(const QString& input)
  {
    for (const auto& c : input) {
      if (c.isLetter())
        this->push_back(c);
    }
  }

  Polymer(const Polymer& other, const QChar& remove_char) {
    for (const auto& c : other) {
      if (c.toLower() != remove_char)
        this->push_back(c);
    }
  }

  void reduct()
  {
    for (auto it = std::begin(*this); std::next(it) != std::end(*this);)
    {
      if (*it / *std::next(it)) {
        it = this->erase(it, std::next(std::next(it)));
        if (it != std::begin(*this))
          --it;
      } else {
        ++it;
      }
    }
  }
};

void Solver_2018_05_1::solve(const QString& input)
{
  auto polymer = Polymer{input};
  polymer.reduct();
  emit finished(QString{"%1"}.arg(polymer.size()));
}

void Solver_2018_05_2::solve(const QString& input)
{
  const auto polymer = Polymer{input};
  auto characters = std::set<QChar>{};
  for (const auto& c : polymer)
    characters.insert(c.toLower());
  auto smallest = std::numeric_limits<std::size_t>::max();
  for (const auto& c : characters) {
    auto candidate = Polymer{polymer, c};
    candidate.reduct();
    smallest = std::min(smallest, candidate.size());
  }
  emit finished(QString{"%1"}.arg(smallest));
}

