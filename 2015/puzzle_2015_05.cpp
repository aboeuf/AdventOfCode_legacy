#include <2015/puzzle_2015_05.h>
#include <common.h>

const QString vowels("aeiou");
const QStringList disallowed({"ab", "cd", "pq", "xy"});

inline bool isNice_v1(const QString& input)
{
  if (input.isEmpty())
    return false;
  auto nb_voyels = vowels.contains(input.front()) ? 1 : 0;
  auto contains_double = false;
  for (auto i = 1; i < input.size(); ++i) {
    for (const auto& d : disallowed)
      if (d[0] == input[i - 1] and d[1] == input[i])
        return false;
    if (input[i - 1] == input[i])
      contains_double = true;
    if (vowels.contains(input[i]))
      ++nb_voyels;
  }
  return contains_double and nb_voyels > 2;
}

inline bool isNice_v2(const QString& input)
{
  auto valid = false;
  for (auto i = 1; not valid and i < input.size(); ++i) {
    for (auto j = i + 2; not valid and j < input.size(); ++j)
      if (input[j - 1] == input[i - 1] and input[j] == input[i])
        valid = true;
  }
  if (not valid)
    return false;
  for (auto i = 2; i < input.size(); ++i)
    if (input[i - 2] == input[i])
      return true;
  return false;
}

void Solver_2015_05_1::solve(const QString& input)
{
  const auto lines = common::splitLines(input);
  auto nb_nice = 0;
  for (const auto& line : lines) {
    if (isNice_v1(line))
      ++nb_nice;
  }
  emit finished(QString("%1").arg(nb_nice));
}

void Solver_2015_05_2::solve(const QString& input)
{
  const auto lines = common::splitLines(input);
  auto nb_nice = 0;
  for (const auto& line : lines)
    if (isNice_v2(line))
      ++nb_nice;
  emit finished(QString("%1").arg(nb_nice));
}
