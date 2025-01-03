#include <QDebug>
#include <common.h>

namespace common {

void throwRunTimeError(const QString &message) {
  throw std::runtime_error(message.toStdString());
}

void throwInvalidArgumentError(const QString &message) {
  throw std::invalid_argument(message.toStdString());
}

/******************************************************************************/

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

/******************************************************************************/

template <typename IntType>
using Converter = std::function<IntType(const QString &, bool &)>;

template <typename IntType>
IntType convert(const QString &input, const Converter<IntType> &converter) {
  auto ok = true;
  const auto value = converter(input, ok);
  if (not ok) {
    throwInvalidArgumentError(
        QString("common::convert: cannot convert string \"%1\" to integer")
            .arg(input));
  }
  return value;
}

template <typename IntType>
QVector<IntType> splitAndConvert(const QString &input, const QChar &split_char,
                                 const Converter<IntType> &converter) {
  const auto tokens = splitValues(input, split_char);
  auto values = QVector<IntType>();
  values.reserve(tokens.size());
  for (const auto &token : tokens) {
    if (not token.isEmpty()) {
      values << convert<IntType>(token, converter);
    }
  }
  return values;
}

/******************************************************************************/

const auto short_converter = [](const QString &input, bool &ok) -> short {
  return input.toShort(&ok);
};

short toShort(const QString &input) {
  return convert<short>(input, short_converter);
}

QVector<short> toVecShort(const QString &input, const QChar &split_char) {
  return splitAndConvert<short>(input, split_char, short_converter);
}

/******************************************************************************/

const auto ushort_converter = [](const QString &input, bool &ok) -> ushort {
  return input.toUShort(&ok);
};

ushort toUShort(const QString &input) {
  return convert<ushort>(input, ushort_converter);
}

QVector<ushort> toVecUShort(const QString &input, const QChar &split_char) {
  return splitAndConvert<ushort>(input, split_char, ushort_converter);
}

/******************************************************************************/

const auto int_converter = [](const QString &input, bool &ok) -> int {
  return input.toInt(&ok);
};

int toInt(const QString &input) { return convert<int>(input, int_converter); }

QVector<int> toVecInt(const QString &input, const QChar &split_char) {
  return splitAndConvert<int>(input, split_char, int_converter);
}

/******************************************************************************/

const auto uint_converter = [](const QString &input, bool &ok) -> uint {
  return input.toUInt(&ok);
};

uint toUInt(const QString &input) {
  return convert<uint>(input, uint_converter);
}

QVector<uint> toVecUInt(const QString &input, const QChar &split_char) {
  return splitAndConvert<uint>(input, split_char, uint_converter);
}

/******************************************************************************/

const auto long_converter = [](const QString &input, bool &ok) -> long {
  return input.toLong(&ok);
};

long toLong(const QString &input) {
  return convert<long>(input, long_converter);
}

QVector<long> toVecLong(const QString &input, const QChar &split_char) {
  return splitAndConvert<long>(input, split_char, long_converter);
}

/******************************************************************************/

const auto ulong_converter = [](const QString &input, bool &ok) -> ulong {
  return input.toULong(&ok);
};

ulong toULong(const QString &input) {
  return convert<ulong>(input, ulong_converter);
}

QVector<ulong> toVecULong(const QString &input, const QChar &split_char) {
  return splitAndConvert<ulong>(input, split_char, ulong_converter);
}

/******************************************************************************/

const auto long_long_converter = [](const QString &input,
                                    bool &ok) -> long long {
  return input.toLongLong(&ok);
};

long long toLongLong(const QString &input) {
  return convert<long long>(input, long_long_converter);
}

QVector<long long> toVecLongLong(const QString &input,
                                 const QChar &split_char) {
  return splitAndConvert<long long>(input, split_char, long_long_converter);
}

/******************************************************************************/

const auto ulong_long_converter = [](const QString &input,
                                     bool &ok) -> unsigned long long {
  return input.toULongLong(&ok);
};

unsigned long long toULongLong(const QString &input) {
  return convert<unsigned long long>(input, ulong_long_converter);
}

QVector<unsigned long long> toVecULongLong(const QString &input,
                                           const QChar &split_char) {
  return splitAndConvert<unsigned long long>(input, split_char,
                                             ulong_long_converter);
}

} // namespace common
