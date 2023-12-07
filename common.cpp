#include <common.h>
#include <QDebug>

namespace common {

void throwRunTimeError(const QString& message)
{
    throw std::runtime_error(message.toStdString());
}

void throwInvalidArgumentError(const QString& message)
{
    throw std::invalid_argument(message.toStdString());
}

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

QVector<int> toIntValues(const QString& input, const QChar& split_char)
{
  const auto string_values = splitValues(input, split_char);
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

QVector<long long int> toLongLongIntValues(const QString& input, const QChar& split_char)
{
  using Int = long long int;
  const auto string_values = splitValues(input, split_char);
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

QVector<unsigned long long int> toULongLongIntValues(const QString& input, const QChar& split_char)
{
  using Int = unsigned long long int;
  const auto string_values = splitValues(input, split_char);
  QVector<Int> int_values;
  int_values.reserve(string_values.size());
  bool conversion_ok;
  for (const QString& string_value : string_values) {
    if (!string_value.isEmpty()) {
      Int int_value = string_value.toULongLong(&conversion_ok);
      if (conversion_ok)
        int_values << int_value;
    }
  }
  return int_values;
}

QVector<uint> toUIntValues(const QString& input, const QChar& split_char)
{
  const auto string_values = splitValues(input, split_char);
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
