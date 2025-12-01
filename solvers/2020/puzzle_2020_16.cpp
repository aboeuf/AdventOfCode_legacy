#include <QSet>
#include <QVector>
#include <solvers/common.h>
#include <solvers/2020/puzzle_2020_16.h>

namespace puzzle_2020_16 {

using Ticket = QVector<uint>;

struct Field {
  Field(const QStringList &input) {
    m_name = input[1];
    m_first_range.first = input[2].toUInt();
    m_first_range.second = input[3].toUInt();
    m_second_range.first = input[4].toUInt();
    m_second_range.second = input[5].toUInt();
  }

  bool isWithinRange(uint value) const {
    return (m_first_range.first <= value && value <= m_first_range.second) ||
           (m_second_range.first <= value && value <= m_second_range.second);
  }

  QString m_name;
  QPair<uint, uint> m_first_range;
  QPair<uint, uint> m_second_range;
  QSet<int> m_indexes{};
};

struct Problem {
  using Int = unsigned long long int;
  Int m_solution{0};

  Problem(const QString &input, bool puzzle_2) {
    QList<Field> fields;
    QList<Ticket> tickets;
    const QStringList lines = common::splitLines(input);
    QRegExp rx("(.+):\\s(\\d+)\\-(\\d+)\\sor\\s(\\d+)\\-(\\d+)");
    int index_in = 0;
    auto line = lines.begin();
    for (; index_in >= 0 && line != lines.end(); ++line) {
      index_in = rx.indexIn(*line);
      if (index_in >= 0)
        fields << Field(rx.capturedTexts());
    }
    for (; line != lines.end(); ++line) {
      const Ticket ticket = common::toVecUInt(*line);
      if (!ticket.empty())
        tickets << ticket;
    };

    int min_size = std::numeric_limits<int>::max();
    auto ticket_it = tickets.begin();
    while (ticket_it != tickets.end()) {
      auto value_it = std::find_if_not(
          ticket_it->begin(), ticket_it->end(), [&fields](const uint &value) {
            return std::any_of(fields.begin(), fields.end(),
                               [&value](const Field &field) {
                                 return field.isWithinRange(value);
                               });
          });
      if (value_it == ticket_it->end()) {
        min_size = std::min(ticket_it->size(), min_size);
        ++ticket_it;
      } else {
        if (!puzzle_2)
          m_solution += static_cast<Int>(*value_it);
        ticket_it = tickets.erase(ticket_it);
      }
    }
    if (!puzzle_2 || fields.empty() || tickets.empty())
      return;

    QSet<Field *> departures;
    for (Field &field : fields) {
      if (field.m_name.startsWith("departure"))
        departures.insert(&field);
      for (int i = 0; i < min_size; ++i)
        if (std::all_of(tickets.begin(), tickets.end(),
                        [&i, &field](const Ticket &ticket) {
                          return field.isWithinRange(ticket[i]);
                        }))
          field.m_indexes.insert(i);
    }

    m_solution = 1;
    auto field_it = fields.begin();
    while (field_it != fields.end()) {
      if (field_it->m_indexes.size() == 1) {
        uint index = *(field_it->m_indexes.begin());
        if (departures.contains(&(*field_it))) {
          m_solution *= static_cast<Int>(tickets.front()[index]);
          departures.remove(&(*field_it));
          if (departures.empty())
            return;
        }
        for (Field &field : fields)
          field.m_indexes.remove(index);
        field_it = fields.begin();
      } else
        ++field_it;
    }
  }
};

} // namespace puzzle_2020_16

void Solver_2020_16_1::solve(const QString &input) {
  emit finished(
      QString::number(puzzle_2020_16::Problem(input, false).m_solution));
}

void Solver_2020_16_2::solve(const QString &input) {
  emit finished(
      QString::number(puzzle_2020_16::Problem(input, true).m_solution));
}
