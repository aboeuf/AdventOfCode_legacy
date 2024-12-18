#include <QDebug>
#include <common.h>

namespace common {

void throwRunTimeError(const QString &message) {
  throw std::runtime_error(message.toStdString());
}

void throwInvalidArgumentError(const QString &message) {
  throw std::invalid_argument(message.toStdString());
}

QStringList splitLines(const QString &input, bool filter_empty_lines) {
  QStringList res = input.split('\n');
  if (filter_empty_lines) {
    for (auto it = std::begin(res); it != std::end(res);) {
      if (it->isEmpty()) {
        it = res.erase(it);
      } else {
        ++it;
      }
    }
  } else {
    while (!res.isEmpty() && res.back().isEmpty()) {
      res.pop_back();
    }
  }
  return res;
}

QStringList splitValues(const QString &input, const QChar &split_char) {
  QStringList res = input.split(split_char);
  while (!res.isEmpty() && (res.back().isEmpty() || res.back() == "\n"))
    res.pop_back();
  return res;
}

template <typename DataType>
QVector<DataType> splitAndConvert(
    const QString &input, const QChar &split_char,
    const std::function<DataType(const QString &, bool &)> &converter) {
  const auto tokens = splitValues(input, split_char);
  auto values = QVector<DataType>{};
  values.reserve(tokens.size());
  auto ok = true;
  for (const auto &token : tokens) {
    if (not token.isEmpty()) {
      const auto value = converter(token, ok);
      if (ok) {
        values << value;
      } else {
        throwRunTimeError(token);
      }
    }
  }
  return values;
}

QVector<short> toShort(const QString &input, const QChar &split_char) {
  return splitAndConvert<short>(
      input, split_char,
      [](const QString &token, bool &ok) { return token.toShort(&ok); });
}

QVector<ushort> toUShort(const QString &input, const QChar &split_char) {
  return splitAndConvert<ushort>(
      input, split_char,
      [](const QString &token, bool &ok) { return token.toUShort(&ok); });
}

QVector<int> toInt(const QString &input, const QChar &split_char) {
  return splitAndConvert<int>(
      input, split_char,
      [](const QString &token, bool &ok) { return token.toInt(&ok); });
}

QVector<uint> toUInt(const QString &input, const QChar &split_char) {
  return splitAndConvert<uint>(
      input, split_char,
      [](const QString &token, bool &ok) { return token.toUInt(&ok); });
}

QVector<long> toLong(const QString &input, const QChar &split_char) {
  return splitAndConvert<long>(
      input, split_char,
      [](const QString &token, bool &ok) { return token.toLong(&ok); });
}

QVector<ulong> toULong(const QString &input, const QChar &split_char) {
  return splitAndConvert<ulong>(
      input, split_char,
      [](const QString &token, bool &ok) { return token.toULong(&ok); });
}

QVector<long long> toLongLong(const QString &input, const QChar &split_char) {
  return splitAndConvert<long long>(
      input, split_char,
      [](const QString &token, bool &ok) { return token.toLongLong(&ok); });
}

QVector<unsigned long long> toULongLong(const QString &input,
                                        const QChar &split_char) {
  return splitAndConvert<unsigned long long>(
      input, split_char,
      [](const QString &token, bool &ok) { return token.toULongLong(&ok); });
}

} // namespace common
