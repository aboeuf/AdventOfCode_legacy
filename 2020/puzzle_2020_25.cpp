#include <2020/puzzle_2020_25.h>
#include <common.h>
#include <qDebug>

namespace puzzle_2020_25
{

using Int = unsigned long long int;

constexpr Int MOD = 20201227;
constexpr Int SUBJECT = 7;

QPair<Int, Int> readPublicKeys(const QString& input, bool& ok)
{
  QPair<Int, Int> public_keys(0, 0);
  ok = false;
  const QStringList lines = common::splitLines(input);
  if (lines.size() != 2)
    return public_keys;
  public_keys.first = lines.front().toULongLong(&ok);
  if (!ok)
    return public_keys;
  public_keys.second = lines.back().toULongLong(&ok);
  return public_keys;
}

QPair<Int, Int> findLoopsSizes(const QPair<Int, Int>& public_keys,
                               Int subject = SUBJECT, Int mod = MOD)
{
  QPair<Int, Int> loops_sizes(0, 0);
  Int value = 1;
  for (Int n = 1; loops_sizes.first == 0 || loops_sizes.second == 0; ++n) {
    value = (subject * value) % mod;
    if (loops_sizes.first == 0 && public_keys.first == value)
      loops_sizes.first = n;
    if (loops_sizes.second == 0 && public_keys.second == value)
      loops_sizes.second = n;
  }
  return loops_sizes;
}

Int transformSubject(Int subject, Int loop_size, Int mod = MOD)
{
  Int value = 1;
  for (Int i = 0; i < loop_size; ++i)
    value = subject * value % mod;
  return value;
}

} // namespace puzzle_2020_25

void Solver_2020_25_1::solve(const QString& input)
{
  using namespace puzzle_2020_25;
  bool ok;
  QPair<Int, Int> public_keys = readPublicKeys(input, ok);
  if (!ok) {
    emit finished("Cannot read public keys");
    return;
  }
  QPair<Int, Int> loop_sizes = findLoopsSizes(public_keys);
  Int encryption_key_from_card = transformSubject(public_keys.second, loop_sizes.first);
  Int encryption_key_from_door = transformSubject(public_keys.first, loop_sizes.second);
  if (encryption_key_from_card != encryption_key_from_door) {
    emit finished(QString("Different encryption keys:\nCard: %1\nDoor: %2")
                  .arg(encryption_key_from_card).arg(encryption_key_from_door));
    return;
  }
  emit finished(QString::number(encryption_key_from_card));
}

void Solver_2020_25_2::solve(const QString&)
{
  emit finished("No second puzzle on day 25");
}

