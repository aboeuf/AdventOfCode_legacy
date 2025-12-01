#include <QDate>
#include <QDateTime>
#include <QRegExp>
#include <QTime>
#include <solvers/common.h>
#include <deque>
#include <optional>
#include <set>
#include <solvers/2018/puzzle_2018_04.h>
#include <unordered_map>

namespace {

class Entry {

public:
  enum Type { BEGINS_SHIFT, FALLS_ASLEEP, WAKES_UP };

  Entry(const QString &input) {
    auto rx =
        QRegExp{"^\\[([0-9]+)-([0-9]+)-([0-9]+) ([0-9]+):([0-9]+)\\] (.+)$"};
    if (not rx.exactMatch(input))
      return;
    m_timestamp = QDateTime{
        QDate{rx.cap(1).toInt(), rx.cap(2).toInt(), rx.cap(3).toInt()},
        QTime{rx.cap(4).toInt(), rx.cap(5).toInt()}};
    const auto log_str = rx.cap(6);
    m_valid = m_timestamp.isValid();
    if (not m_valid)
      return;
    if (log_str == "falls asleep") {
      m_type = FALLS_ASLEEP;
      return;
    }
    if (log_str == "wakes up") {
      m_type = WAKES_UP;
      return;
    }
    auto rx_action = QRegExp{"^Guard #([0-9]+) begins shift$"};
    if (not rx_action.exactMatch(log_str))
      return;
    m_type = BEGINS_SHIFT;
    m_guard_id = rx_action.cap(1).toUInt();
  }

  bool operator<(const Entry &other) const {
    return m_timestamp < other.timestamp();
  }

  QString toString() const {
    if (!m_valid)
      return "INVALID";
    auto res = m_timestamp.toString(Qt::ISODate);
    switch (m_type) {
    case BEGINS_SHIFT:
      res += QString{" GUARD #%1 BEGINS_SHIFT"}.arg(m_guard_id);
      break;
    case FALLS_ASLEEP:
      res += " FALLS_ASLEEP";
      break;
    case WAKES_UP:
      res += " WAKES_UP";
      break;
    }
    return res;
  }

  bool valid() const { return m_valid; }
  const QDateTime &timestamp() const { return m_timestamp; }
  QDate date() const { return m_timestamp.date(); }
  QTime time() const { return m_timestamp.time(); }
  Type type() const { return m_type; }
  uint guardID() const { return m_guard_id; }

  void setGuardID(uint id) { m_guard_id = id; }

private:
  bool m_valid{false};
  QDateTime m_timestamp{};
  Type m_type;
  uint m_guard_id;
};

class Shift {
public:
  Shift(std::vector<Entry> &entries, std::size_t &index) {
    if (index >= entries.size())
      return;
    if (entries[index].type() != Entry::BEGINS_SHIFT)
      return;
    m_guard_id = entries[index].guardID();
    m_date = entries[index].date();
    ++index;
    auto current_minute = 0;
    auto asleep = false;
    for (; index < entries.size() and
           entries[index].type() != Entry::BEGINS_SHIFT;
         ++index) {
      entries[index].setGuardID(m_guard_id);
      auto minute = entries[index].time().minute();
      for (; current_minute < 60 and current_minute < minute; ++current_minute)
        m_minutes[current_minute] = asleep;
      asleep = (entries[index].type() == Entry::FALLS_ASLEEP);
    }
    for (; current_minute < 60; ++current_minute)
      m_minutes[current_minute] = asleep;
    m_valid = true;
  }

  bool valid() const { return m_valid; }
  const QDate &date() const { return m_date; }
  uint guardID() const { return m_guard_id; }
  const std::array<bool, 60> &minutes() const { return m_minutes; }

  uint sleepingTime() const {
    auto res = 0u;
    for (auto asleep : m_minutes)
      if (asleep)
        ++res;
    return res;
  }

  QString toString() const {
    auto res = QString{"%1-%2 #%3   "}
                   .arg(m_date.month(), 2, 10, QChar('0'))
                   .arg(m_date.day(), 2, 10, QChar('0'))
                   .arg(m_guard_id);
    for (auto i = 0; i < 60; ++i)
      res += (m_minutes[i] ? "#" : ".");
    return res;
  }

private:
  bool m_valid{false};
  QDate m_date;
  uint m_guard_id;
  std::array<bool, 60> m_minutes;
};

struct Solution {
  QString toString() const { return QString{"%1"}.arg(m_guard_id * m_minute); }
  uint m_guard_id;
  uint m_minute;
};

struct Guard {
  Guard() = default;
  Guard(uint id) : m_id{id} {}

  uint sleepingTime() const {
    auto res = 0u;
    for (const auto *shift : m_shifts) {
      if (shift)
        res += shift->sleepingTime();
    }
    return res;
  }

  std::pair<uint, uint> mostSleptMinute() const {
    auto sleeping_counts = std::vector<uint>{};
    sleeping_counts.resize(60);
    for (auto i = 0; i < 60; ++i)
      sleeping_counts[i] = 0u;
    for (const auto *shift : m_shifts)
      if (shift)
        for (auto i = 0; i < 60; ++i)
          if (shift->minutes()[i])
            ++sleeping_counts[i];
    auto max_count = sleeping_counts[0];
    auto most_slept = 0u;
    for (auto i = 1u; i < 60u; ++i) {
      if (sleeping_counts[i] > max_count) {
        max_count = sleeping_counts[i];
        most_slept = i;
      }
    }
    return std::make_pair(most_slept, max_count);
  }

  uint m_id{std::numeric_limits<uint>::infinity()};
  std::deque<Shift *> m_shifts{};
};

class Record {
public:
  Record(const QString &input) {
    const auto lines = common::splitLines(input);
    m_entries.reserve(lines.size());
    auto nb_shifts = 0u;
    auto guards_is = std::set<uint>{};
    for (const auto &line : lines) {
      m_entries.emplace_back(line);
      if (not m_entries.back().valid()) {
        m_error = "Invalid input: " + line;
        return;
      }
      if (m_entries.back().type() == Entry::BEGINS_SHIFT) {
        ++nb_shifts;
        guards_is.insert(m_entries.back().guardID());
      }
    }

    for (auto id : guards_is)
      m_guards[id] = Guard{id};

    std::sort(std::begin(m_entries), std::end(m_entries));
    m_shifts.reserve(nb_shifts);
    for (auto index = std::size_t{0}; index < m_entries.size();) {
      m_shifts.emplace_back(m_entries, index);
      if (not m_shifts.back().valid())
        m_error = "Invalid shift: " + m_shifts.back().toString();
      m_guards[m_shifts.back().guardID()].m_shifts.push_back(&m_shifts.back());
    }
  }

  const QString &error() const { return m_error; }
  const std::vector<Entry> &entries() const { return m_entries; }
  const std::vector<Shift> &shifts() const { return m_shifts; }

  std::optional<Solution> strategy_1() const {
    auto max_sleeping_time = 0u;
    const Guard *sleepiest_guard = nullptr;
    for (const auto &g : m_guards) {
      const auto sleeping_time = g.second.sleepingTime();
      if (not sleepiest_guard or sleeping_time > max_sleeping_time) {
        max_sleeping_time = sleeping_time;
        sleepiest_guard = &g.second;
      }
    }
    if (not sleepiest_guard)
      return std::nullopt;
    return Solution{sleepiest_guard->m_id,
                    sleepiest_guard->mostSleptMinute().first};
  }

  std::optional<Solution> strategy_2() const {
    auto max_most_slept = std::pair<uint, uint>{0u, 0u};
    const Guard *sleepiest_guard = nullptr;
    for (const auto &g : m_guards) {
      const auto most_slept = g.second.mostSleptMinute();
      if (not sleepiest_guard or most_slept.second > max_most_slept.second) {
        max_most_slept = most_slept;
        sleepiest_guard = &g.second;
      }
    }
    if (not sleepiest_guard)
      return std::nullopt;
    return Solution{sleepiest_guard->m_id, max_most_slept.first};
  }

private:
  QString m_error{""};
  std::vector<Entry> m_entries{};
  std::vector<Shift> m_shifts{};
  std::unordered_map<uint, Guard> m_guards{};
};

} // namespace

void Solver_2018_04_1::solve(const QString &input) {
  const auto record = Record{input};
  if (not record.error().isEmpty()) {
    emit finished(record.error());
    return;
  }
  const auto solution = record.strategy_1();
  if (solution)
    emit finished(solution->toString());
  else
    emit finished("FAILURE");
}

void Solver_2018_04_2::solve(const QString &input) {
  const auto record = Record{input};
  if (not record.error().isEmpty()) {
    emit finished(record.error());
    return;
  }
  const auto solution = record.strategy_2();
  if (solution)
    emit finished(solution->toString());
  else
    emit finished("FAILURE");
}
