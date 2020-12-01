#pragma once

#include <QStringList>
#include <vector>

namespace common {

QStringList splitLines(const QString& input);
QStringList splitValues(const QString& input);
std::vector<int> toIntValues(const QString& input);

}
