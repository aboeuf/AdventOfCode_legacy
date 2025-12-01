#include <QDebug>
#include <deque>
#include <solvers/2020/puzzle_2020_09.h>
#include <solvers/common.h>

namespace puzzle_2020_09 {

using Data = std::vector<int>;
using Heap = std::deque<int>;

Data readXmasData(const QString &input) {
  const QStringList lines = common::splitLines(input);
  Data data;
  data.reserve(input.size());
  bool ok;
  for (const QString &line : lines) {
    int value = line.toInt(&ok);
    if (ok)
      data.emplace_back(value);
  }
  return data;
}

Heap getPreamble(const Data &data, std::size_t length) {
  Heap preamble;
  for (auto i = 0u; i < std::min(length, data.size()); ++i)
    preamble.push_back(data[i]);
  return preamble;
}

void pushInHeap(const Data &data, Heap &heap, std::size_t pos) {
  if (pos < data.size()) {
    heap.pop_front();
    heap.push_back(data[pos]);
  }
}

bool isValid(const Data &data, const Heap &heap, std::size_t pos) {
  if (pos >= data.size())
    return false;
  for (auto i = heap.begin(); i != heap.end(); ++i) {
    for (auto j = std::next(i); j != heap.end(); ++j)
      if (*i + *j == data[pos])
        return true;
  }
  return false;
}

bool findFirstInvalid(const QString &input, std::size_t preamble_length,
                      int &result) {
  const Data data = readXmasData(input);
  Heap heap = getPreamble(data, preamble_length);
  for (size_t i = preamble_length; i < data.size(); ++i) {
    if (!isValid(data, heap, i)) {
      result = data[i];
      return true;
    }
    pushInHeap(data, heap, i);
  }
  return false;
}

} // namespace puzzle_2020_09

void Solver_2020_09_1::solve(const QString &input) {
  using namespace puzzle_2020_09;
  int result;
  if (findFirstInvalid(input, 25, result))
    emit finished(QString::number(result));
  else
    emit finished("All valid");
}

void Solver_2020_09_2::solve(const QString &input) {
  using namespace puzzle_2020_09;
  const Data data = readXmasData(input);
  int result;
  if (!findFirstInvalid(input, 25, result)) {
    emit finished("All valid");
    return;
  }
  for (std::size_t i = 0; i + 1 < data.size(); ++i) {
    int sum = data[i];
    int min = sum;
    int max = sum;
    for (std::size_t j = 1; i + j < data.size(); ++j) {
      sum += data[i + j];
      min = std::min(min, data[i + j]);
      max = std::max(max, data[i + j]);
      if (sum == result) {
        emit finished(QString::number(min + max));
        return;
      }
    }
  }
  emit finished("FAILURE");
}
