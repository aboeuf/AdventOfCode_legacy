#include <solvers/common.h>
#include <solvers/2015/puzzle_2015_08.h>

inline QString reduct(const QString &str) {
  auto res = QString{};
  auto escaped = false;
  for (auto i = 0; i < str.size(); ++i) {
    if (str[i] == '\\') {
      if (escaped) {
        res.push_back('\\');
        escaped = false;
      } else {
        escaped = true;
      }
    } else if (str[i] == '"') {
      if (escaped) {
        res.push_back('"');
        escaped = false;
      }
    } else if (str[i] == 'x') {
      if (escaped) {
        if (i + 2 < str.size()) {
          const auto chr = str.mid(i + 1, 2);
          res.push_back(QChar{static_cast<char>(chr.toUInt(nullptr, 16))});
        }
        i += 2;
        escaped = false;
      } else {
        res.push_back('x');
      }
    } else {
      res.push_back(str[i]);
      escaped = false;
    }
  }
  return res;
}

inline QString extend(const QString &str) {
  auto res = QString{};
  res.push_back('"');
  for (const auto &c : str) {
    if (c == '"' or c == '\\')
      res.push_back('\\');
    res.push_back(c);
  }
  res.push_back('"');
  return res;
}

void Solver_2015_08_1::solve(const QString &input) {
  auto lines = common::splitLines(input);
  auto sum = 0;
  for (auto &line : lines) {
    line.remove(' ');
    sum += line.size() - reduct(line).size();
  }
  emit finished(QString("%1").arg(sum));
}

void Solver_2015_08_2::solve(const QString &input) {
  auto lines = common::splitLines(input);
  auto sum = 0;
  for (auto &line : lines) {
    line.remove(' ');
    sum += extend(line).size() - line.size();
  }
  emit finished(QString("%1").arg(sum));
}
