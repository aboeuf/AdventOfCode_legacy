#include <2021/puzzle_2021_04.h>
#include <common.h>

struct BingoCell {
  uint value{0};
  bool marked{false};
};

class BingoBoard {
public:
  BingoBoard() = default;

  void setRow(uint i, const QVector<uint> &values) {
    if (i >= 5u or values.size() != 5)
      return;
    for (auto j = 0; j < 5; ++j)
      m_grid[i][j].value = values[j];
  }

  uint mark(uint value) {
    auto nb_marked = 0u;
    for (auto i = 0; i < 5; ++i) {
      for (auto j = 0; j < 5; ++j) {
        if (not m_grid[i][j].marked and m_grid[i][j].value == value) {
          m_grid[i][j].marked = true;
          ++nb_marked;
        }
      }
    }
    return nb_marked;
  }

  bool isFullRow(uint i) const {
    if (i >= 5u)
      return false;
    for (auto j = 0; j < 5; ++j)
      if (not m_grid[i][j].marked)
        return false;
    return true;
  }

  bool isFullColumn(uint j) const {
    if (j >= 5u)
      return false;
    for (auto i = 0; i < 5; ++i)
      if (not m_grid[i][j].marked)
        return false;
    return true;
  }

  bool hasWon() const {
    for (auto i = 0; i < 5; ++i)
      if (isFullRow(i))
        return true;
    for (auto j = 0; j < 5; ++j)
      if (isFullColumn(j))
        return true;
    return false;
  }

  uint getScore() const {
    auto score = 0u;
    for (auto i = 0; i < 5; ++i)
      for (auto j = 0; j < 5; ++j)
        if (not m_grid[i][j].marked)
          score += m_grid[i][j].value;
    return score;
  }

private:
  std::array<std::array<BingoCell, 5>, 5> m_grid;
};

class BingoGame {
public:
  BingoGame(const QString &input) {
    const auto lines = common::splitLines(input);
    if (lines.empty())
      return;
    auto it = std::begin(lines);
    m_called_numbers = common::toVecUInt(*it, QChar(','));
    auto current_row = 5u;
    ++it;
    for (; it != std::end(lines); ++it) {
      if (not it->isEmpty()) {
        if (current_row >= 5u) {
          current_row = 0u;
          m_boards.push_back(BingoBoard{});
        }
        m_boards.back().setRow(current_row, common::toVecUInt(*it, QChar(' ')));
        ++current_row;
      }
    }
  }

  QString play_V1() {
    for (const auto &n : m_called_numbers) {
      for (auto &board : m_boards) {
        if (board.mark(n) > 0u and board.hasWon())
          return QString{"%1"}.arg(n * board.getScore());
      }
    }
    return "No winner";
  }

  QString play_V2() {
    for (const auto &n : m_called_numbers) {
      for (auto it = std::begin(m_boards); it != std::end(m_boards);) {
        if (it->mark(n) > 0u and it->hasWon()) {
          if (m_boards.size() == 1)
            return QString{"%1"}.arg(n * it->getScore());
          else
            it = m_boards.erase(it);
        } else
          ++it;
      }
    }
    return "No winner";
  }

private:
  QVector<uint> m_called_numbers;
  std::list<BingoBoard> m_boards;
};

void Solver_2021_04_1::solve(const QString &input) {
  emit finished(BingoGame{input}.play_V1());
}

void Solver_2021_04_2::solve(const QString &input) {
  emit finished(BingoGame{input}.play_V2());
}
