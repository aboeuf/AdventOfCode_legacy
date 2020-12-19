#include <2020/puzzle_2020_19.h>
#include <common.h>
#include <QMap>
#include <QRegExp>
#include <QDebug>

namespace puzzle_2020_19
{

using Int = unsigned int;

QRegExp rx_pattern_only = QRegExp("^(\\d+)\\:\\s\\\"([a-zA-Z]+)\\\"$");

struct Rule
{
  Rule() = default;
  Rule(QMap<Int, Rule>* rules, const QString& input) : m_rules{rules}
  {
    if (rx_pattern_only.indexIn(input) == 0) {
      QStringList captured = rx_pattern_only.capturedTexts();
      if (captured.size() > 1)
        m_index = captured[1].toUInt();
      if (captured.size() > 2)
        m_pattern = captured[2];
    } else {
      QStringList line = input.split(' ');
      if (!line.empty()) {
        line[0].chop(1);
        m_index = line[0].toUInt();
      }
      QList<Int>* p = &m_pattern_a_int;
      for (int i = 1; i < line.size(); ++i) {
        if (line[i] == "|")
          p = &m_pattern_b_int;
        else
          *p << line[i].toUInt();
      }
    }
    m_pattern_a_str.fill("", m_pattern_a_int.size());
    m_pattern_b_str.fill("", m_pattern_b_int.size());
  }

  void debug()
  {
    QString a="[", b="[";
    for (int i = 0; i < m_pattern_a_int.size(); ++i) {
      if (m_pattern_a_str[i].isEmpty())
        a += QString::number(m_pattern_a_int[i]);
      else
        a += m_pattern_a_str[i];
      a += ", ";
    }
    if (a.endsWith(", "))
      a.chop(2);
    a += "]";
    for (int i = 0; i < m_pattern_b_int.size(); ++i) {
      if (m_pattern_b_str[i].isEmpty())
        b += QString::number(m_pattern_b_int[i]);
      else
        b += m_pattern_b_str[i];
      b += ", ";
    }
    if (b.endsWith(", "))
      b.chop(2);
    b += "]";
    qDebug() << "Index:" <<m_index
             << "Pattern:" << m_pattern
             << "A:" << a.toStdString().c_str()
             << "B:" << b.toStdString().c_str()
             << "Children:" << m_children
             << "All set:" << allSet();
  }

  bool allSet() const
  {
    for (const QString& str : m_pattern_a_str)
      if (str.isEmpty())
        return false;
    for (const QString& str : m_pattern_b_str)
      if (str.isEmpty())
        return false;
    return true;
  }

  void set(Int parent, const QString& pattern)
  {
    if (allSet())
      return;
    for (int i = 0; i < m_pattern_a_int.size(); ++i)
      if (m_pattern_a_int[i] == parent)
        m_pattern_a_str[i] = pattern;
    for (int i = 0; i < m_pattern_b_int.size(); ++i)
      if (m_pattern_b_int[i] == parent)
        m_pattern_b_str[i] = pattern;
    if (allSet()) {
      bool is_or = !m_pattern_b_str.isEmpty();
      if (is_or)
        m_pattern = "(";
      for (const QString& str : m_pattern_a_str)
        m_pattern += str;
      if (is_or) {
        m_pattern += "|";
        for (const QString& str : m_pattern_b_str)
          m_pattern += str;
        m_pattern += ")";
      }
      setChildren();
    }
  }

  void setChildren()
  {
    if (m_rules)
      for (const Int& child : m_children)
        (*m_rules)[child].set(m_index, m_pattern);
  }

  bool match(const QString& message) const
  {
    return QRegExp("^" + m_pattern + "$").indexIn(message) == 0;
  }

  Int m_index{0};
  QList<Int> m_pattern_a_int{};
  QVector<QString> m_pattern_a_str{};
  QList<Int> m_pattern_b_int{};
  QVector<QString> m_pattern_b_str{};
  QString m_pattern{};
  QSet<Int> m_children{};
  QMap<Int, Rule>* m_rules{nullptr};
};

struct Puzzle
{
  Puzzle(const QString& input)
  {
    QSet<Int> roots;
    m_messages = common::splitLines(input);
    while (!m_messages.empty()) {
      QString line = m_messages.front();
      m_messages.pop_front();
      if (line.isEmpty())
        break;
      Rule rule(&m_rules, line);
      m_rules[rule.m_index] = rule;
      if (!rule.m_pattern.isEmpty())
        roots.insert(rule.m_index);
    }
    for (Rule& rule : m_rules.values()) {
      for (Int& i : rule.m_pattern_a_int)
        m_rules[i].m_children.insert(rule.m_index);
      for (Int& i : rule.m_pattern_b_int)
        m_rules[i].m_children.insert(rule.m_index);
    }
    for (const Int& root : roots)
      m_rules[root].setChildren();
  }

  void debug() {
    for (Rule& rule : m_rules.values())
      rule.debug();
  }

  Int nbMatches(Int rule)
  {
    Int count = 0;
    if (!m_rules.contains(rule))
      return count;
    Rule& r = m_rules[rule];
    for (const QString& message: m_messages)
      if (r.match(message))
        ++count;
    return count;
  }

  QMap<Int, Rule> m_rules;
  QStringList m_messages;
};

} // namespace puzzle_2020_19

void Solver_2020_19_1::solve(const QString& input)
{
  using namespace puzzle_2020_19;
  Puzzle in(input);
  emit finished(QString::number(in.nbMatches(0)));
}

void Solver_2020_19_2::solve(const QString& input)
{
  using namespace puzzle_2020_19;
  Puzzle in(input);
  QRegExp r42(in.m_rules[42].m_pattern);
  QRegExp r31(in.m_rules[31].m_pattern);
  const auto is_a_match = [&r42, &r31](QString message) -> bool {
    uint i = 0;
    while (r42.indexIn(message) == 0) {
      message.remove(0, r42.capturedTexts()[1].size());
      ++i;
    }
    if (i == 0)
      return false;
    uint j = 0;
    while (r31.indexIn(message) == 0) {
      message.remove(0, r31.capturedTexts()[1].size());
      ++j;
    }
    return message.isEmpty() && j != 0 && i > j;
  };
  uint count = 0;
  for (const QString& message : in.m_messages)
    if (is_a_match(message))
      ++count;
  emit finished(QString::number(count));
}

