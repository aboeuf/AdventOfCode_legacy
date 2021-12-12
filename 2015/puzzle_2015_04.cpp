#include <2015/puzzle_2015_04.h>
#include <common.h>
#include <QCryptographicHash>

#include <iostream>

using Int = unsigned long long int;

inline QString hash(const QString& input)
{
  return QString(QCryptographicHash::hash(QByteArray(input.toStdString().c_str()),
                                          QCryptographicHash::Md5).toHex());
}

inline bool isValid(const QString& input, Int value, const QString& starts_with)
{
  return hash(input + QString("%1").arg(value)).startsWith(starts_with);
}

inline QString smallestValid(const QString& input, const QString& starts_with) {
  auto copy = input;
  copy.remove('\n');
  auto i = Int{0};
  while (not isValid(copy, i, starts_with)) { ++i; }
  return QString("%1").arg(i);
}

void Solver_2015_04_1::solve(const QString& input)
{
  emit finished(smallestValid(input, "00000"));
}

void Solver_2015_04_2::solve(const QString& input)
{
  emit finished(smallestValid(input, "000000"));
}
