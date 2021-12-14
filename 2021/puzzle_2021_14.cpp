#include <2021/puzzle_2021_14.h>
#include <common.h>

using Int = unsigned long long int;

class Polymer
{
public:
  Polymer(const QString& input, uint nb_steps)
  {
    const auto lines = common::splitLines(input);
    if (lines.empty())
      return;
    auto occurences = QMap<QChar, Int>{};
    for (const auto& chr : lines.front()) {
      if (not occurences.contains(chr))
        occurences[chr] = Int(1u);
      else
        ++occurences[chr];
    }
    if (nb_steps == 0)
      return;
    auto rules = QMap<QString, QChar>{};
    for (auto it = std::next(std::begin(lines)); it != std::end(lines); ++it) {
      const auto values = common::splitValues(*it, ' ');
      if (values.size() > 2 and not values[2].isEmpty()) {
        rules[values[0]] = values[2][0];
        if (not occurences.contains(values[2][0]))
          occurences[values[2][0]] = Int(0u);
      }
    }
    if (rules.empty())
      return;
    const auto add_pairs = [&rules](
        QMap<QString, Int>& record,
        const QChar& first,
        const QChar& second,
        Int nb) {
      const auto pair =  QString{"%1%2"}.arg(first).arg(second);
      if (rules.contains(pair)) {
        auto it = record.find(pair);
        if (it == record.end())
          record[pair] = nb;
        else
          it.value() += nb;
      }
    };
    auto current = QMap<QString, Int>{};
    for (auto i = 1; i < lines.front().size(); ++i)
      add_pairs(current, lines.front()[i - 1], lines.front()[i], Int(1));
    for (auto i = 0u; i < nb_steps; ++i) {
      auto next = QMap<QString, Int>{};
      for (auto pair = std::begin(current); pair != std::end(current); ++pair) {
        auto& chr = rules[pair.key()];
        occurences[chr] += pair.value();
        add_pairs(next, pair.key()[0], chr, pair.value());
        add_pairs(next, chr, pair.key()[1], pair.value());
      }
      std::swap(current, next);
    }
    auto nb_occurences = occurences.values();
    for (auto it = std::begin(nb_occurences); it != std::end(nb_occurences);) {
      if (*it == Int(0))
        it = nb_occurences.erase(it);
      else
        ++it;
    }
    if (nb_occurences.empty())
      return;
    if (nb_occurences.size() == 1) {
      m_ocurences_range = nb_occurences.front();
      return;
    }
    std::sort(std::begin(nb_occurences), std::end(nb_occurences));
    m_ocurences_range = nb_occurences.back() - nb_occurences.front();
  }

  QString ocurencesRange() const
  {
    return QString("%1").arg(m_ocurences_range);
  }

private:
  Int m_ocurences_range{0};
};

void Solver_2021_14_1::solve(const QString& input)
{
  emit finished(Polymer(input, 10).ocurencesRange());
}

void Solver_2021_14_2::solve(const QString& input)
{
  emit finished(Polymer(input, 40).ocurencesRange());
}
