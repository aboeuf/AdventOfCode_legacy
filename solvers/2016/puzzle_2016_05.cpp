#include <solvers/common.h>
#include <solvers/qchar_hash.hpp>
#include <solvers/2016/puzzle_2016_05.h>

#include <QCryptographicHash>

namespace puzzle_2016_05 {

using Int = unsigned long long;

const auto hexchar_to_position = std::unordered_map<QChar, ushort>{
    {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3},
    {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7},
};

inline std::optional<ushort> getPosition(const QChar &c) {
  const auto it = hexchar_to_position.find(c);
  if (it == std::cend(hexchar_to_position)) {
    return std::nullopt;
  }
  return it->second;
}

inline QString hash(const QString &door_id, Int index) {
  return QString(QCryptographicHash::hash(
                     QString("%1%2").arg(door_id).arg(index).toLatin1(),
                     QCryptographicHash::Md5)
                     .toHex());
}

inline QString getPasswordV1(const QString &door_id, int password_size) {
  auto password = QString();
  for (auto index = Int{0}; password.size() < password_size; ++index) {
    const auto md5_hash = hash(door_id, index);
    if (md5_hash.startsWith("00000")) {
      password.push_back(md5_hash[5]);
    }
  }
  return password;
}

inline QString getPasswordV2(const QString &door_id,
                             std::size_t password_size) {
  auto password = std::map<ushort, QChar>{};
  for (auto index = Int{0}; std::size(password) < password_size; ++index) {
    const auto md5_hash = hash(door_id, index);
    if (md5_hash.startsWith("00000")) {
      const auto position = getPosition(md5_hash[5]);
      if (position.has_value()) {
        password.insert(std::make_pair(*position, md5_hash[6]));
      }
    }
  }
  auto result = QString{};
  for (const auto &[_, c] : password) {
    result.push_back(c);
  }
  return result;
}

} // namespace puzzle_2016_05

void Solver_2016_05_1::solve(const QString &input) {
  emit finished(
      puzzle_2016_05::getPasswordV1(common::splitLines(input).front(), 8));
}

void Solver_2016_05_2::solve(const QString &input) {
  emit finished(
      puzzle_2016_05::getPasswordV2(common::splitLines(input).front(), 8));
}
