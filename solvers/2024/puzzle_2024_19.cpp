#include <solvers/2024/puzzle_2024_19.h>
#include <solvers/common.h>

namespace puzzle_2024_19 {

using Int = unsigned long long;
using Cache = QHash<QStringRef, Int>;

inline Int getNbOptions(const QStringList &patterns, const QStringRef &design,
                        Cache &cache) {
  if (design.isEmpty()) {
    return Int{1};
  }
  const auto it = cache.constFind(design);
  if (it != cache.cend()) {
    return *it;
  }
  auto nb_options = Int{0};
  for (const auto &pattern : patterns) {
    if (design.startsWith(pattern)) {
      const auto tail = design.mid(pattern.size());
      const auto tail_nb_options = getNbOptions(patterns, tail, cache);
      nb_options += tail_nb_options;
    }
  }
  cache.insert(design, nb_options);
  return nb_options;
}

class Towels {
public:
  Towels(const QString &input) {
    m_designs = common::splitLines(input, true);
    if (m_designs.empty()) {
      common::throwInvalidArgumentError("puzzle_2024_19::Towels: empty input");
    }
    m_patterns = common::splitValues(m_designs.front());
    for (auto &pattern : m_patterns) {
      pattern.remove(QChar(' '));
    }
    m_designs.pop_front();
  }

  QString solve(bool v2) const {
    auto cache = Cache();
    auto nb_possible = Int{0};
    auto total_nb_options = Int{0};
    for (const auto &design : m_designs) {
      const auto nb_options =
          getNbOptions(m_patterns, QStringRef(&design), cache);
      if (nb_options != Int{0}) {
        ++nb_possible;
        total_nb_options += nb_options;
      }
    }
    return QString("%1").arg(v2 ? total_nb_options : nb_possible);
  }

private:
  QStringList m_patterns;
  QStringList m_designs;
};

} // namespace puzzle_2024_19

void Solver_2024_19_1::solve(const QString &input) {
  const auto towels = puzzle_2024_19::Towels(input);
  emit finished(towels.solve(false));
}

void Solver_2024_19_2::solve(const QString &input) {
  const auto towels = puzzle_2024_19::Towels(input);
  emit finished(towels.solve(true));
}
