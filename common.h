#pragma once

#include <QStringList>
#include <QVector>
#include <iso646.h>

namespace common {

QStringList splitLines(const QString& input);
QStringList splitValues(const QString& input, const QChar& split_char = ',');
QVector<int> toIntValues(const QString& input, const QChar& split_char = ',');
QVector<long long int> toLongLongIntValues(const QString& input, const QChar& split_char = ',');
QVector<uint> toUIntValues(const QString& input, const QChar &split_char = ',');

}
