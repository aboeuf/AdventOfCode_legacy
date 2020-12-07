#include <2019/puzzle_2019_4.h>
#include <common.h>
#include <functional>
#include <QDebug>

namespace puzzle_2019_4 {

using Rule = std::function<bool(const QString& password)>;

const auto adjacent_digits_rule = [](const int& nb_adjacent) -> Rule {
  return [&nb_adjacent](const QString& password) -> bool {
    int count = 1;
    for (auto it = std::next(password.begin()); count != nb_adjacent && it != password.end(); ++it) {
      if (*it == *std::prev(it))
        ++count;
      else
        count = 1;
    }
    return count == nb_adjacent;
  };
};

const auto adjacent_digits_rule_modified = [](const int& nb_adjacent) -> Rule {
  return [&nb_adjacent](const QString& password) -> bool {
    QList<int> counts = {1};
    for (auto it = std::next(password.begin()); it != password.end(); ++it) {
      if (*it == *std::prev(it))
        ++counts.back();
      else {
        if (counts.back() == nb_adjacent)
          return true;
        counts << 1;
      }
    }
    return counts.back() == nb_adjacent;
  };
};

const Rule non_decreasing_rule = [](const QString& password) -> bool {
  if (password.isEmpty())
    return false;
  auto it = password.begin();
  bool ok;
  int prev = QString(*it).toInt(&ok);
  if (!ok)
    return false;
  for (; it != password.end(); ++it) {
    int curr = QString(*it).toInt(&ok);
    if (!ok || prev > curr)
      return false;
    prev = curr;
  }
  return true;
};

bool getRange(const QString& input, int& min, int& max)
{
  const QStringList range = common::splitValues(input, '-');
  if (range.size() < 2)
    return false;
  bool ok;
  min = range[0].toInt(&ok);
  if (!ok)
    return false;
  max = range[1].toInt(&ok);
  if (!ok)
    return false;
  return true;
}

bool isValid(const QString& password, const std::vector<Rule>& rules) {
  for (const Rule& rule : rules)
    if (!rule(password))
      return false;
  return true;
}

int nbValid(const std::vector<Rule>& rules, int min, int max)
{
  int nb_valid = 0;
  for (int i = min; i <= max; ++i)
    if (isValid(QString("%1").arg(i, 6, 10, QChar('0')), rules))
      ++nb_valid;
  return nb_valid;
}

}

void Solver_2019_4_1::solve(const QString& input)
{
  using namespace puzzle_2019_4;
  int min, max;
  if (!getRange(input, min, max)) {
    emit finished("FAILURE");
    return;
  }
  const std::vector<Rule> rules = {
    adjacent_digits_rule(2),
    non_decreasing_rule
  };
  emit finished(QString::number(nbValid(rules, min, max)));
}

void Solver_2019_4_2::solve(const QString& input)
{
  using namespace puzzle_2019_4;
  int min, max;
  if (!getRange(input, min, max)) {
    emit finished("FAILURE");
    return;
  }
  const std::vector<Rule> rules = {
    adjacent_digits_rule_modified(2),
    non_decreasing_rule
  };
  emit finished(QString::number(nbValid(rules, min, max)));
}
