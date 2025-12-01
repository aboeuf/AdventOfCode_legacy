#include <solvers/2016/puzzle_2016_09.h>
#include <solvers/common.h>

namespace puzzle_2016_09 {

using Int = unsigned long long;

Int decompressedLength(const QString &data, bool v2) {
  auto rx = QRegExp("\\((\\d+)x(\\d+)\\)");
  auto decompressed_length = Int(0);
  auto current_pos = 0;
  auto pos = 0;
  while (current_pos < data.size() and
         (pos = rx.indexIn(data, current_pos)) != -1) {
    const auto unmatched_length = pos - current_pos;
    const auto matched_length = rx.matchedLength();
    const auto token_length = rx.cap(1).toInt();
    current_pos += unmatched_length + matched_length + token_length;
    const auto decompressed_token_length =
        v2 ? decompressedLength(data.mid(pos + matched_length, token_length),
                                true)
           : static_cast<Int>(token_length);
    decompressed_length += static_cast<Int>(unmatched_length) +
                           decompressed_token_length * rx.cap(2).toULongLong();
  }
  decompressed_length += static_cast<Int>(data.size() - current_pos);
  return decompressed_length;
}

} // namespace puzzle_2016_09

void Solver_2016_09_1::solve(const QString &input) {
  const auto data = common::splitLines(input).front().remove(' ');
  const auto decompressed_length =
      puzzle_2016_09::decompressedLength(data, false);
  emit finished(QString("%1").arg(decompressed_length));
}

void Solver_2016_09_2::solve(const QString &input) {
  const auto data = common::splitLines(input).front().remove(' ');
  const auto decompressed_length =
      puzzle_2016_09::decompressedLength(data, true);
  emit finished(QString("%1").arg(decompressed_length));
}
