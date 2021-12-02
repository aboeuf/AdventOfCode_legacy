#include <2018/puzzle_2018_07.h>
#include <common.h>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <optional>

#include <QDebug>

const auto extra_time = std::unordered_map<char, uint>
{
  {'A', 1u},
  {'B', 2u},
  {'C', 3u},
  {'D', 4u},
  {'E', 5u},
  {'F', 6u},
  {'G', 7u},
  {'H', 8u},
  {'I', 9u},
  {'J', 10u},
  {'K', 11u},
  {'L', 12u},
  {'M', 13u},
  {'N', 14u},
  {'O', 15u},
  {'P', 16u},
  {'Q', 17u},
  {'R', 18u},
  {'S', 19u},
  {'T', 20u},
  {'U', 21u},
  {'V', 22u},
  {'W', 23u},
  {'X', 24u},
  {'Y', 25u},
  {'Z', 26u}
};

struct Step
{
  std::unordered_set<char> m_predecessors{};
  std::unordered_set<char> m_successors{};
  uint m_starting_time{0};
  uint m_ending_time{0};
};

class PERT
{
public:
  PERT(const QString& /*input*/,
       uint /*nb_workers*/,
       uint /*nb_seconds_per_step*/)
  {
//    auto waiting_steps = std::unordered_map<char, Step>{};
//    auto rx = QRegExp{"^Step (.) must be finished before step (.) can begin\\.$"};
//    const auto lines = common::splitLines(input);
//    for (const auto& line : lines) {
//      if (not rx.exactMatch(line)) {
//        m_error = "Invalid input: " + line;
//        return;
//      }
//      const auto first = rx.cap(1).front().toLatin1();
//      const auto second = rx.cap(2).front().toLatin1();
//      auto prev = waiting_steps.insert({first, Step{}}).first;
//      auto next = waiting_steps.insert({second, Step{}}).first;
//      prev->second.m_successors.insert(second);
//      next->second.m_predecessors.insert(first);
//    }
    
//    auto ready_steps = std::set<char>{};
//    for (const auto& s : steps)
//      if (s.second.m_predecessors.empty())
//        ready_steps.insert(s.first);

//    using Comp = std::function<bool(const char&, const char&)>;
//    auto ongoing_steps = std::set<char, Comp>{[&steps](const char& lhs, const char& rhs) {
//        const auto left = steps.find(lhs);
//        if (left == std::end(steps))
//          return false;
//        const auto right = steps.find(rhs);
//        if (right == std::end(steps))
//          return true;
//        if (left->second.m_ending_time == right->second.m_ending_time)
//          return lhs < rhs;
//        return left->second.m_ending_time < right->second.m_ending_time;
//      }};

//    const auto debug = [&]()
//    {
//      qDebug() << "------------------------------";
//      qDebug() << "Time:" << current_time;
//      qDebug() << "Nb workers:" << nb_available_workers;
//      qDebug() << "Done:" << m_sequence;
//      auto ready = QString{"Ready:"};
//      for (const auto& c : ready_steps)
//        ready.push_back(c);
//      qDebug() <<  ready.toStdString().c_str();
//      auto ongoing = QString{"Ongoing:"};
//      for (const auto& c : ongoing_steps) {
//        const auto it = steps.find(c);
//        if (it == steps.end())
//          ongoing += QString{" (%1, not found)"}.arg(c);
//        else
//          ongoing += QString{" (%1, %2, %3)"}.arg(c)
//              .arg(it->second.m_starting_time)
//              .arg(it->second.m_ending_time);
//      }
//      qDebug() << ongoing.toStdString().c_str();
//    };

//    auto current_time = 0u;
//    auto nb_available_workers = nb_workers;
    
//    for (;;) {
//      debug();
//      if (not ongoing_steps.empty() and (ready_steps.empty() or nb_available_workers == 0)) {
//        const auto label = *std::begin(ongoing_steps);
//        auto step = waiting_steps.find(label);
//        if (step == )
//        qDebug() << "------------------------------";
//        qDebug() << "--> END" << step.m_label << "at" << step.m_ending_time;
//        ongoing_steps.erase(std::begin(ongoing_steps));
//        step.m_status = Step::DONE;
//        m_sequence.push_back(step.m_label);
//        ++nb_available_workers;
//        current_time = step.m_ending_time + 1;
//        update_ready();
//      } else if (not ready_steps.empty()) {
//        if (nb_available_workers == 0) {
//          m_error = "No more available workers";
//          return;
//        }
//        auto& step = steps[*std::begin(ready_steps)];
//        step.m_starting_time = current_time;
//        step.m_ending_time = current_time + nb_seconds_per_step + extraTime(step.m_label);
//        qDebug() << "------------------------------";
//        qDebug() << "--> START" << step.m_label << "at" << step.m_starting_time;
//        ready_steps.erase(std::begin(ready_steps));
//        ongoing_steps.insert(step.m_label);
//        --nb_available_workers;
//      } else {
//        return;
//      }
//    }
  }

  const QString& error() const { return m_error; }
  const QString& sequence() const { return m_sequence; }

private:
  QString m_sequence{""};
  QString m_error{""};
};

void Solver_2018_07_1::solve(const QString& input)
{
  const auto pert = PERT{input, 1, 0};
  if (not pert.error().isEmpty())
    emit output(pert.error());
  emit finished(pert.sequence());
}

void Solver_2018_07_2::solve(const QString& input)
{
  const auto pert = PERT{input, 5, 60};
  if (not pert.error().isEmpty())
    emit output(pert.error());
  emit finished(pert.sequence());
}

