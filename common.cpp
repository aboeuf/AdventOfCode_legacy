#include <common.h>
#include <QDebug>

namespace common {

QStringList splitLines(const QString& input)
{
  QStringList res = input.split('\n');
  while (!res.isEmpty() && res.back().isEmpty())
    res.pop_back();
  return res;
}

QStringList splitValues(const QString& input, const QChar& split_char)
{
  QStringList res = input.split(split_char);
  while (!res.isEmpty() && (res.back().isEmpty() || res.back() == "\n"))
    res.pop_back();
  return res;
}

QVector<int> toIntValues(const QString& input)
{
  const auto string_values = splitValues(input);
  QVector<int> int_values;
  int_values.reserve(string_values.size());
  bool conversion_ok;
  for (const QString& string_value : string_values) {
    if (!string_value.isEmpty()) {
      int int_value = string_value.toInt(&conversion_ok);
      if (conversion_ok)
        int_values << int_value;
    }
  }
  return int_values;
}

QVector<long long int> toLongLongIntValues(const QString& input)
{
  using Int = long long int;
  const auto string_values = splitValues(input);
  QVector<Int> int_values;
  int_values.reserve(string_values.size());
  bool conversion_ok;
  for (const QString& string_value : string_values) {
    if (!string_value.isEmpty()) {
      Int int_value = string_value.toLongLong(&conversion_ok);
      if (conversion_ok)
        int_values << int_value;
    }
  }
  return int_values;
}

QVector<uint> toUIntValues(const QString& input)
{
  const auto string_values = splitValues(input);
  QVector<uint> int_values;
  int_values.reserve(string_values.size());
  bool conversion_ok;
  for (const QString& string_value : string_values) {
    if (!string_value.isEmpty()) {
      uint int_value = string_value.toUInt(&conversion_ok);
      if (conversion_ok)
        int_values << int_value;
    }
  }
  return int_values;
}

}
