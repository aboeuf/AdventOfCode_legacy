#pragma once

#include <QStringList>
#include <QVector>

namespace common {

QStringList splitLines(const QString& input);
QStringList splitValues(const QString& input, const QChar& split_char = ',');
QVector<int> toIntValues(const QString& input);
QVector<long long int> toLongLongIntValues(const QString& input);
QVector<uint> toUIntValues(const QString& input);

}
