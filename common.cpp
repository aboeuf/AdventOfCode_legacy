#include <common.h>
#include <QDebug>

namespace common {

QStringList splitLines(const QString& input) {
  QStringList res = input.split('\n');
  while (!res.isEmpty() && res.back().isEmpty())
    res.pop_back();
  return res;
}

QStringList splitValues(const QString& input) {
  QStringList res = input.split(',');
  while (!res.isEmpty() && (res.back().isEmpty() || res.back() == "\n"))
    res.pop_back();
  return res;
}

std::vector<int> toIntValues(const QString& input)
{
  const auto string_values = splitValues(input);
  std::vector<int> int_values;
  int_values.reserve(string_values.size());
  bool conversion_ok;
  for (const QString& string_value : string_values) {
    if (!string_value.isEmpty()) {
      int int_value = string_value.toInt(&conversion_ok);
      if (conversion_ok)
        int_values.push_back(int_value);
    }
  }
  return int_values;
}

}
