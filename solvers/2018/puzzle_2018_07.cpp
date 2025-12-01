#include <solvers/common.h>
#include <set>
#include <solvers/2018/puzzle_2018_07.h>

#include <iostream>

namespace puzzle_2018_07 {

class Node;
using NodePtr = std::shared_ptr<Node>;

constexpr auto minimum_step_duration = 60u;
constexpr auto nb_workers = 5u;

const auto steps_durations = []() {
  const auto alphabet = QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  auto durations = QHash<QChar, uint>{};
  auto duration = minimum_step_duration + 1u;
  for (const auto &c : alphabet) {
    durations[c] = duration;
    ++duration;
  }
  return durations;
}();

class Node {
public:
  Node(QChar label) : m_label{label} {}

  const QChar &label() const { return m_label; }
  const QSet<NodePtr> &parents() const { return m_parents; }
  const QSet<NodePtr> &children() const { return m_children; }

  void addParent(const NodePtr &parent) { m_parents.insert(parent); }
  void addChild(const NodePtr &child) { m_children.insert(child); }

  bool done() const { return m_done; }
  void setDone(bool done) { m_done = done; }

  bool ready() const {
    for (const auto &parent : m_parents) {
      if (not parent->done()) {
        return false;
      }
    }
    return true;
  }

private:
  QChar m_label;
  QSet<NodePtr> m_parents;
  QSet<NodePtr> m_children;
  bool m_done{false};
};

inline bool operator<(const NodePtr &lhs, const NodePtr &rhs) {
  return lhs->label() < rhs->label();
}

inline bool operator==(const NodePtr &lhs, const NodePtr &rhs) {
  return lhs->label() == rhs->label();
}

inline uint qHash(const NodePtr &node) { return qHash(node->label()); }

struct Worker {
  NodePtr is_working_on{nullptr};
  uint remaining_time{0u};
};

class Graph {
public:
  Graph(const QString &input) {
    const auto lines = common::splitLines(input, true);
    auto line_index = 1u;
    for (const auto &line : lines) {
      auto rx = QRegExp(
          "^Step ([A-Z]) must be finished before step ([A-Z]) can begin\\.$");
      const auto valid = rx.exactMatch(line);
      if (not valid) {
        common::throwInvalidArgumentError(
            QString("puzzle_2018_07::Graph: cannot parse line %1: \"%2\"")
                .arg(line_index)
                .arg(line));
      }
      const auto parent_label = QChar(rx.cap(1).front());
      const auto child_label = QChar(rx.cap(2).front());
      auto &parent_node =
          m_nodes
              .insert(std::make_pair(parent_label,
                                     std::make_shared<Node>(parent_label)))
              .first->second;
      auto &child_node =
          m_nodes
              .insert(std::make_pair(child_label,
                                     std::make_shared<Node>(child_label)))
              .first->second;
      parent_node->addChild(child_node);
      child_node->addParent(parent_node);
      ++line_index;
    }
  }

  QString solveOne() { return solve().first; }

  QString solveTwo() { return QString("%1").arg(solve().second); }

private:
  std::pair<QString, uint> solve() {
    auto ready_nodes = std::set<NodePtr>{};
    for (auto &[_, node] : m_nodes) {
      if (node->ready()) {
        ready_nodes.insert(node);
      }
    }
    auto workers = std::array<Worker, nb_workers>{};

    const auto assign_work = [&ready_nodes, &workers]() {
      for (auto &worker : workers) {
        if (ready_nodes.empty()) {
          break;
        }
        if (not worker.is_working_on) {
          worker.is_working_on = *std::begin(ready_nodes);
          ready_nodes.erase(std::begin(ready_nodes));
          worker.remaining_time =
              steps_durations[worker.is_working_on->label()];
        }
      }
    };

    auto steps = QString{};
    auto time = 0u;
    const auto do_work = [&ready_nodes, &workers, &steps, &time]() {
      auto nb_steps_done = steps.size();
      for (auto &worker : workers) {
        if (worker.is_working_on) {
          if (worker.remaining_time == 1u) {
            worker.is_working_on->setDone(true);
            steps.push_back(worker.is_working_on->label());
            for (const auto &child : worker.is_working_on->children()) {
              if (child->ready()) {
                ready_nodes.insert(child);
              }
            }
            worker.is_working_on = nullptr;
          } else {
            --worker.remaining_time;
          }
        }
      }
      ++time;
      return steps.size() > nb_steps_done;
    };

    assign_work();
    const auto nb_steps = static_cast<int>(std::size(m_nodes));
    while (steps.size() < nb_steps) {
      if (do_work()) {
        assign_work();
        if (steps.size() < nb_steps and
            std::all_of(std::cbegin(workers), std::cend(workers),
                        [](const auto &worker) {
                          return worker.is_working_on == nullptr;
                        })) {
          common::throwRunTimeError("puzzle_2018_07::Graph::solve: all "
                                    "workers are idle after work assignation");
        }
      }
    }
    return std::make_pair(steps, time);
  }

  std::map<QChar, NodePtr> m_nodes;
};

} // namespace puzzle_2018_07

void Solver_2018_07_1::solve(const QString &input) {
  auto graph = puzzle_2018_07::Graph(input);
  emit finished(graph.solveOne());
}

void Solver_2018_07_2::solve(const QString &input) {
  auto graph = puzzle_2018_07::Graph(input);
  emit finished(graph.solveTwo());
}
