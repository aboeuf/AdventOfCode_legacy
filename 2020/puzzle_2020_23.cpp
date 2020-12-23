#include <2020/puzzle_2020_23.h>
#include <common.h>
#include <QDebug>

namespace puzzle_2020_23
{

using Int = unsigned long long int;

struct Cup
{
  Cup(Int value) : m_value{value} {}
  Int m_value{0};
  Cup* m_previous{nullptr};
  Cup* m_next{nullptr};

  void debug() const
  {
    QString prev = m_previous ? QString::number(m_previous->m_value) : QString("null");
    QString next = m_previous ? QString::number(m_next->m_value) : QString("null");
    qDebug() << "Cup" << m_value
             << "Prev" << prev.toStdString().c_str()
             << "Next" << next.toStdString().c_str();
  }
};

struct CupCircle
{
  CupCircle(const QString& input, bool puzzle_2)
  {
    const QStringList lines = common::splitLines(input);
    if (lines.isEmpty())
      return;
    for (const QChar& c : lines.front()) {
      m_cups << Cup(QString(c).toUInt());
      m_value_max = std::max(m_cups.back().m_value, m_value_max);
    }
    if (puzzle_2) {
      ++m_value_max;
      for (;m_value_max <= 1e6; ++m_value_max)
        m_cups << Cup(m_value_max);
      --m_value_max;
    }
    if (m_cups.size() < 4)
      return;
    m_current = &m_cups.front();
    m_current->m_previous = &m_cups.back();
    m_cups.back().m_next = m_current;
    m_cups_by_values.resize(m_value_max + 1);
    m_cups_by_values[m_cups.front().m_value] = m_current;
    for (auto it = std::next(m_cups.begin()); it != m_cups.end(); ++it) {
      Cup *curr = &(*it);
      Cup *prev = &(*std::prev(it));
      prev->m_next = curr;
      curr->m_previous = prev;
      m_cups_by_values[curr->m_value] = curr;
    }
  }

  Int previousValue(Int value)
  {
    return value == 1 ? m_value_max : value - 1;
  }

  void pickup(Cup* start, Int pickup_size)
  {
    if (pickup_size == 0)
      return;
    --pickup_size;
    Cup* end = start;
    for (Int i = 0; i < pickup_size; ++i)
      end = end->m_next;
    start->m_previous->m_next = end->m_next;
    end->m_next->m_previous = start->m_previous;
    start->m_previous = nullptr;
    end->m_next = nullptr;
  }

  Cup* find(Int value)
  {
    if (value < 1 || value >= static_cast<Int>(m_cups_by_values.size()))
      return nullptr;
    return m_cups_by_values[value];
  }

  void insert(Cup* source, Cup* destination)
  {
    if (!source || !destination)
      return;
    Cup* dest_next = destination->m_next;
    destination->m_next = source;
    source->m_previous = destination;
    Cup* curr = source;
    while (curr->m_next && curr->m_next != source) { curr = curr->m_next; }
    curr->m_next = dest_next;
    dest_next->m_previous = curr;
  }

  bool contains(Cup* search, Cup* source)
  {
    if (!search || !source)
      return false;
    Cup* curr = source;
    do
    {
      if (curr == search)
        return true;
      curr = curr->m_next;
    } while (curr && curr != source);
    return false;
  }

  bool move(Int nb_moves, Int pickup_size = 3, bool debug = false) {
    for (Int i = 0; i < nb_moves; ++i) {
      if (debug) {
        qDebug() << "-- move" << i + 1 << "--";
        print("cups:", m_current);
      }
      Cup* pickup_start = m_current->m_next;
      pickup(pickup_start, pickup_size);
      if (debug)
        print("pickup:", pickup_start);
      Cup* destination = nullptr;
      Int dest_value = m_current->m_value;
      while (!destination) {
        dest_value = previousValue(dest_value);
        if (dest_value == m_current->m_value)
          return false;
        destination = find(dest_value);
        if (contains(destination, pickup_start))
          destination = nullptr;
      }
      if (debug) {
        qDebug() << "destination:" << destination->m_value;
        qDebug() << " ";
      }
      insert(pickup_start, destination);
      m_current = m_current->m_next;
    }
    if (debug) {
      qDebug() << "-- final --";
      print("cups:", m_current);
    }
    return true;
  }

  void print(QString header, Cup *curr) const
  {
    qDebug() << header.toStdString().c_str()
             << toString(curr ? curr : m_current).toStdString().c_str();
  }

  QString toString(Cup *curr) const
  {
    if (!curr)
      return "null";
    QString s;
    Cup *first = curr;
    do
    {
      if (curr != first)
        s += " ";
      if (curr == m_current)
        s += "(";
      s += QString::number(curr->m_value);
      if (curr == m_current)
        s += ")";
      curr = curr->m_next;
    } while (curr && curr != first);
    return s;
  }

  QString state(bool puzzle_2)
  {
    Cup* one = find(1);
    if (!one)
      return "No 1 cup";
    if (puzzle_2) {
      if (!one->m_next || !one->m_next->m_next)
        return "No cups after 1";
      return QString::number(one->m_next->m_value * one->m_next->m_next->m_value);
    }
    QString s;
    Cup* curr = one->m_next;
    while (curr && curr != one) {
      s += QString::number(curr->m_value);
      curr = curr->m_next;
    }
    return s;
  }

  Cup* m_current{nullptr};
  QList<Cup> m_cups{};
  QVector<Cup*> m_cups_by_values{};
  Int m_value_max{0};
};

} // namespace puzzle_2020_23

void Solver_2020_23_1::solve(const QString& input)
{
  using namespace puzzle_2020_23;
  CupCircle circle(input, false);
  circle.move(100);
  emit finished(circle.state(false));
}

void Solver_2020_23_2::solve(const QString& input)
{
  using namespace puzzle_2020_23;
  CupCircle circle(input, true);
  circle.move(1e7);
  emit finished(circle.state(true));
}

