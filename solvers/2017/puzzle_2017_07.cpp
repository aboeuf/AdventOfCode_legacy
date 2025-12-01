#include <solvers/2017/puzzle_2017_07.h>
#include <solvers/common.h>

namespace puzzle_2017_07 {

struct Program {
  Program() = default;

  Program(const QString &input) {
    const auto throw_exception = [&input]() {
      common::throwInvalidArgumentError(
          QString("puzzle_2017_07::Program: cannot parse input \"%1\"")
              .arg(input));
    };
    const auto tokens = common::splitValues(input, ' ');
    if (tokens.size() < 2) {
      throw_exception();
    }
    name = tokens[0];
    auto ok = true;
    weight = tokens[1].mid(1, tokens[1].size() - 2).toUInt(&ok);
    if (not ok) {
      throw_exception();
    }
    for (auto i = 3; i < tokens.size(); ++i) {
      if (i + 1 < tokens.size()) {
        children << tokens[i].mid(0, tokens[i].size() - 1);
      } else {
        children << tokens[i];
      }
    }
  }

  QString name{""};
  uint weight{0u};

  std::optional<QString> parent{std::nullopt};
  QStringList children{};

  uint total_weight;
  bool balanced;
  uint depth;
};

class Tower {
public:
  Tower(const QString &input) {
    const auto lines = common::splitLines(input, true);
    for (const auto &line : lines) {
      auto program = Program(line);
      m_programs[program.name] = std::move(program);
    }
    for (auto program = m_programs.begin(); program != m_programs.end();
         ++program) {
      for (const auto &child : program.value().children) {
        if (m_programs[child].parent.has_value()) {
          common::throwRunTimeError(
              QString("puzzle_2017_07::Tower: node \"%1\" has more than one "
                      "parent: \"%2\" and \"%3\"")
                  .arg(child)
                  .arg(*m_programs[child].parent)
                  .arg(program.key()));
        }
        m_programs[child].parent = program.key();
      }
    }
    auto root_round = false;
    for (auto program = m_programs.begin(); program != m_programs.end();
         ++program) {
      if (not program.value().parent.has_value()) {
        if (root_round) {
          common::throwRunTimeError(
              QString(
                  "puzzle_2017_07::Tower: multiple roots: \"%1\" and \"%2\"")
                  .arg(m_root)
                  .arg(program.key()));
        }
        m_root = program.key();
        root_round = true;
      }
    }
    if (not root_round) {
      common::throwRunTimeError("puzzle_2017_07::Tower: cannot find root");
    }
    setData(m_root, 0u);
  }

  const QString root() const { return m_root; }

  QString solve() const {
    auto max_unbalanced_depth = std::optional<uint>{std::nullopt};
    auto deepest_unbalanced = QString{""};
    for (auto program = m_programs.begin(); program != m_programs.end();
         ++program) {
      if (not program.value().balanced and
          (not max_unbalanced_depth.has_value() or
           program.value().depth > *max_unbalanced_depth)) {
        max_unbalanced_depth = program.value().depth;
        deepest_unbalanced = program.key();
      }
    }
    if (deepest_unbalanced.isEmpty()) {
      return "Cannot find deepest unbalanced program";
    }
    auto children_weights = QHash<uint, QStringList>{};
    for (const auto &child : m_programs[deepest_unbalanced].children) {
      children_weights[m_programs[child].total_weight] << child;
    }
    if (children_weights.size() != 2) {
      return "Bad size for children weights";
    }
    const auto [bad_program_name, expected_total_weight] =
        [&children_weights]() -> std::pair<QString, uint> {
      for (auto it = children_weights.begin(); it != children_weights.end();
           ++it) {
        if (it.value().size() == 1) {
          if (std::next(it) == children_weights.end()) {
            return std::make_pair(it.value().front(), std::prev(it).key());
          }
          return std::make_pair(it.value().front(), std::next(it).key());
        }
      }
      common::throwRunTimeError(
          "puzzle_2017_07::Tower: cannot find bad program name");
      return std::make_pair("", 0u);
    }();
    const auto &bad_program = m_programs[bad_program_name];
    const auto good_weight =
        bad_program.weight + expected_total_weight - bad_program.total_weight;
    return QString("%1").arg(good_weight);
  }

private:
  void setData(const QString &name, uint depth) {
    auto &program = m_programs[name];
    program.depth = depth;
    auto children_weights = QSet<uint>{};
    for (const auto &child : program.children) {
      setData(child, depth + 1u);
      children_weights.insert(m_programs[child].total_weight);
    }
    program.balanced = children_weights.size() < 2;
    program.total_weight = program.weight;
    for (const auto &child : program.children) {
      program.total_weight += m_programs[child].total_weight;
    }
  }

  QHash<QString, Program> m_programs{};
  QString m_root{""};
};

} // namespace puzzle_2017_07

void Solver_2017_07_1::solve(const QString &input) {
  const auto tower = puzzle_2017_07::Tower(input);
  emit finished(tower.root());
}

void Solver_2017_07_2::solve(const QString &input) {
  const auto tower = puzzle_2017_07::Tower(input);
  emit finished(tower.solve());
}
