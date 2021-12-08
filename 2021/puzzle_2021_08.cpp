#include <2021/puzzle_2021_08.h>
#include <common.h>
#include <QMap>
#include <set>

#include <iostream>

using Int = unsigned long long int;

class Entry
{
public:
  Entry(const QString& input)
  {
    auto patterns = common::splitValues(input, QChar{' '});
    for (auto& c : patterns)
      std::sort(std::begin(c), std::end(c));
    auto input_patterns = std::array<QString, 10>{};
    for (auto i = 0; i < 10; ++i)
      input_patterns[i] = patterns[i];
    std::sort(std::begin(input_patterns),
              std::end(input_patterns),
              [](const QString& lhs, const QString& rhs) { return lhs.size() < rhs.size(); });

    // Obvious patterns
    auto pattern_to_digit = QMap<QString, Int>{};
    pattern_to_digit[input_patterns[0]] = Int{1};
    pattern_to_digit[input_patterns[1]] = Int{7};
    pattern_to_digit[input_patterns[2]] = Int{4};
    pattern_to_digit[input_patterns[9]] = Int{8};

    // Identify the 3 (only pattern of size 5 that contains both segments that the 1 also contains)
    auto size_5 = std::set<int>{3, 4, 5};
    auto pattern_3 = QString{};
    for (auto it = std::begin(size_5); it != std::end(size_5); ++it) {
      if (input_patterns[*it].contains(input_patterns[0].at(0)) and
          input_patterns[*it].contains(input_patterns[0].at(1))) {
        pattern_3 = input_patterns[*it];
        pattern_to_digit[input_patterns[*it]] = Int{3};
        size_5.erase(it);
        break;
      }
    }

    // Identify the 6 (only pattern of size 6 that contains only one of the segments that the 1 also contains)
    // Recover the translation of the 'c' segment (i.e. the missing segment)
    auto c_segment = QChar{'*'};
    auto size_6 = std::set<int>{6, 7, 8};
    for (auto it = std::begin(size_6); it != std::end(size_6); ++it) {
      if (not input_patterns[*it].contains(input_patterns[0].at(0)) or
          not input_patterns[*it].contains(input_patterns[0].at(1))) {
        if (not input_patterns[*it].contains(input_patterns[0].at(0)))
          c_segment = input_patterns[0].at(0);
        else
          c_segment = input_patterns[0].at(1);
        pattern_to_digit[input_patterns[*it]] = Int{6};
        size_6.erase(it);
        break;
      }
    }

    // Identify the 5 (only remaing pattern of size 5 that do not contain the 'c' segment)
    for (auto it = std::begin(size_5); it != std::end(size_5); ++it) {
      if (not input_patterns[*it].contains(c_segment)) {
        pattern_to_digit[input_patterns[*it]] = Int{5};
        size_5.erase(it);
        break;
      }
    }

    // Identify the 2 (only remaing pattern of size 5)
    auto pattern_2 = input_patterns[*std::begin(size_5)];
    pattern_to_digit[pattern_2] = Int{2};

    // Identify the translation of the 'g' segment (only segment that is in 2 but not in 3)
    for (const auto& chr : pattern_3)
      pattern_2.remove(chr);
    auto g_segment = pattern_2.at(0);

    // Identify the 9 (only remaining pattern of size 6 that do not contain the 'g' segment)
    for (auto it = std::begin(size_6); it != std::end(size_6); ++it) {
      if (not input_patterns[*it].contains(g_segment)) {
        pattern_to_digit[input_patterns[*it]] = Int{9};
        size_6.erase(it);
        break;
      }
    }

    // Identify the 0 (only remaing pattern of size 5)
    pattern_to_digit[input_patterns[*std::begin(size_6)]] = Int{0};

    // Decode the output (and count the number of obvious patterns)
    const auto obvious_patterns = std::set<Int>{Int{1}, Int{4}, Int{7}, Int{8}};
    auto ten_power = Int{1};
    for (auto i = patterns.size() - 1; i > 10; --i) {
      const auto& digit = pattern_to_digit[patterns[i]];
      if (obvious_patterns.find(digit) != std::end(obvious_patterns))
        ++m_nb_obvious_patterns;
      m_output += ten_power * digit;
      ten_power *= Int{10};
    }
  }

  const uint& nbObviousInOutput() const {
    return m_nb_obvious_patterns;
  }

  const Int& output() const
  {
    return m_output;
  }

private:
  uint m_nb_obvious_patterns{0};
  Int m_output{0};
};

class Entries
{
public:
  Entries(const QString& input)
  {
    const auto lines = common::splitLines(input);
    m_entries.reserve(lines.size());
    for (const auto& line : lines)
      m_entries.emplace_back(line);
  }

  QString solvePuzzleOne() const
  {
    auto total = 0u;
    for (const auto& entry : m_entries)
      total += entry.nbObviousInOutput();
    return QString{"%1"}.arg(total);
  }

  QString solvePuzzleTwo() const
  {
    auto total = 0u;
    for (const auto& entry : m_entries)
      total += entry.output();
    return QString{"%1"}.arg(total);
  }

private:
  std::vector<Entry> m_entries;
};

void Solver_2021_08_1::solve(const QString& input)
{
  emit finished(Entries{input}.solvePuzzleOne());
}

void Solver_2021_08_2::solve(const QString& input)
{
  emit finished(Entries{input}.solvePuzzleTwo());
}
