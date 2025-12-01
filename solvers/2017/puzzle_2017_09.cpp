#include <solvers/common.h>
#include <solvers/2017/puzzle_2017_09.h>

namespace puzzle_2017_09 {

struct Group {
  Group(const QString &data, Group *parent = nullptr, int start = -1)
      : parent{parent}, start{start}, depth{parent ? parent->depth + 1u : 0u} {
    auto inside_garbage = false;
    for (auto i = start + 1; i < data.size();) {
      if (data[i] == '!') {
        i += 2;
      } else if (data[i] == '<') {
        if (inside_garbage) {
          ++garbage_size;
        } else {
          inside_garbage = true;
        }
        ++i;
      } else if (data[i] == '>') {
        inside_garbage = false;
        ++i;
      } else if (inside_garbage) {
        ++garbage_size;
        ++i;
      } else {
        if (data[i] == '{') {
          children << new Group(data, this, i);
          i = children.back()->stop + 1;
        } else if (data[i] == '}') {
          stop = i;
          return;
        } else {
          ++i;
        }
      }
    }
  }

  ~Group() {
    for (auto *child : children) {
      delete child;
    }
  }

  uint totalScore() const {
    auto score = depth;
    for (const auto &child : children) {
      score += child->totalScore();
    }
    return score;
  }

  uint garbageSize() const {
    auto size = garbage_size;
    for (const auto &child : children) {
      size += child->garbageSize();
    }
    return size;
  }

  Group *parent{nullptr};
  QList<Group *> children{};
  int start{0};
  uint depth{0};
  int stop{0};
  uint garbage_size{0};
};

} // namespace puzzle_2017_09

void Solver_2017_09_1::solve(const QString &input) {
  const auto root =
      puzzle_2017_09::Group(common::splitLines(input, true).front());
  emit finished(QString("%1").arg(root.totalScore()));
}

void Solver_2017_09_2::solve(const QString &input) {
  const auto root =
      puzzle_2017_09::Group(common::splitLines(input, true).front());
  emit finished(QString("%1").arg(root.garbageSize()));
}
