#include <solvers/common.h>
#include <deque>
#include <solvers/2021/puzzle_2021_10.h>

#include <iostream>

using Int = unsigned long long int;

const QMap<QChar, QChar> match_characters = {
    {'(', ')'},
    {'[', ']'},
    {'{', '}'},
    {'<', '>'},
};

const QMap<QChar, Int> syntax_error_scores = {
    {')', Int{3}},
    {']', Int{57}},
    {'}', Int{1197}},
    {'>', Int{25137}},
};

const QMap<QChar, Int> auto_complete_scores = {
    {')', Int{1}},
    {']', Int{2}},
    {'}', Int{3}},
    {'>', Int{4}},
};

struct Chunk {
  Chunk(QString &input) {
    if (input.isEmpty())
      return;
    m_opening = input.front();
    input.remove(0, 1);
    while (not m_corrupted and not input.isEmpty()) {
      if (not match_characters.contains(input.front())) {
        m_closing = input.front();
        input.remove(0, 1);
        m_corrupted = (match_characters[m_opening] != m_closing);
        if (m_corrupted)
          m_syntax_error_score = syntax_error_scores[m_closing];
        return;
      }
      m_children.push_back(new Chunk{input});
      if (m_children.back()->m_corrupted) {
        m_corrupted = true;
        m_syntax_error_score = m_children.back()->m_syntax_error_score;
        return;
      }
    }
  }

  ~Chunk() {
    for (auto *c : m_children)
      delete c;
  }

  void autoComplete(QString &completion_string) {
    for (auto *c : m_children)
      c->autoComplete(completion_string);
    if (m_closing == '*') {
      m_closing = match_characters[m_opening];
      completion_string.push_back(m_closing);
    }
  }

  QChar m_opening{'*'};
  QChar m_closing{'*'};
  bool m_corrupted{false};
  Int m_syntax_error_score{0};
  std::deque<Chunk *> m_children;
};

struct ChunkLine {
  ChunkLine(QString &input) {
    while (not m_corrupted and not input.isEmpty()) {
      m_chunks.emplace_back(input);
      if (m_chunks.back().m_corrupted) {
        m_corrupted = true;
        m_syntax_error_score = m_chunks.back().m_syntax_error_score;
      }
    }
    if (not m_corrupted) {
      QString completion_string;
      m_chunks.back().autoComplete(completion_string);
      for (const auto &c : completion_string)
        m_auto_complete_score =
            Int{5} * m_auto_complete_score + auto_complete_scores[c];
    }
  }

  std::deque<Chunk> m_chunks;
  Int m_syntax_error_score{0};
  Int m_auto_complete_score{0};
  bool m_corrupted{false};
};

struct NavigationSubsystem {
  NavigationSubsystem(const QString &input) {
    auto lines = common::splitLines(input);
    m_lines.reserve(lines.size());
    for (auto &line : lines) {
      m_lines.emplace_back(line);
      if (m_lines.back().m_corrupted) {
        m_syntax_error_score += m_lines.back().m_syntax_error_score;
        m_lines.pop_back();
      }
    }
    std::sort(std::begin(m_lines), std::end(m_lines),
              [](const auto &lhs, const auto &rhs) {
                return lhs.m_auto_complete_score < rhs.m_auto_complete_score;
              });
    m_middle_auto_complete_score =
        m_lines[m_lines.size() / 2].m_auto_complete_score;
  }

  QString solvePuzzleOne() const {
    return QString("%1").arg(m_syntax_error_score);
  }

  QString solvePuzzleTwo() const {
    return QString("%1").arg(m_middle_auto_complete_score);
  }

  std::vector<ChunkLine> m_lines;
  Int m_syntax_error_score{0};
  Int m_middle_auto_complete_score{0};
};

void Solver_2021_10_1::solve(const QString &input) {
  emit finished(NavigationSubsystem{input}.solvePuzzleOne());
}

void Solver_2021_10_2::solve(const QString &input) {
  emit finished(NavigationSubsystem{input}.solvePuzzleTwo());
}
