#include <2020/puzzle_2020_18.h>
#include <common.h>

namespace puzzle_2020_18
{

using Int = unsigned long long int;

struct Expression
{
  Expression(QString input, const QString& op = "") : m_op{op}
  {
    input.remove(' ');
    QString exp, next_op;
    if (input.startsWith('('))
    {
      int n = 1;
      for (int i = 1; n != 0 && i < input.size(); ++i)
      {
        if (input[i] == '(')
          ++n;
        if (input[i] == ')')
          --n;
        if (n != 0)
          exp.push_back(input[i]);
        else {
          if (i + 1 < input.size())
            next_op = QString(input[i + 1]);
          if (i + 2 < input.size()) {
            m_next = new Expression(input.mid(i + 2), next_op);
          }
        }
      }
      if (!exp.isEmpty())
        m_child = new Expression(exp);
    } else {
      for (int i = 0; i < input.size(); ++i) {
        if (input[i].isDigit())
          exp.push_back(input[i]);
        else {
          next_op = QString(input[i]);
          if (i + 1 < input.size()) {
            m_next = new Expression(input.mid(i + 1), next_op);
          }
          break;
        }
      }
      if (!exp.isEmpty())
        m_value = exp.toULongLong();
    }
  }

  ~Expression()
  {
    delete m_next;
    delete m_child;
  }

  void eval(Int& result)
  {
    Int value;
    if (m_child)
      m_child->eval(value);
    else
      value = m_value;
    if (m_op == "+")
      result += value;
    else if (m_op == "*")
      result *= value;
    else
      result = value;
    if (m_next)
      m_next->eval(result);
  }

  void collapse()
  {
    Expression *current = this;
    while (current) {
      Expression* child = current->m_child;
      if (child) {
        child->collapse();
        current->m_value = child->m_value;
        current->m_child = nullptr;
        delete child;
      }
      current = current->m_next;
    }
    current = this;
    while (current) {
      Expression* next = current->m_next;
      if (next && next->m_op == "+") {
        current->m_value += next->m_value;
        current->m_next = next->m_next;
        next->m_next = nullptr;
        delete next;
      } else
        current = current->m_next;
    }
    current = this;
    while (current) {
      Expression* next = current->m_next;
      if (next) {
        current->m_value *= next->m_value;
        current->m_next = next->m_next;
        next->m_next = nullptr;
        delete next;
      } else
        current = current->m_next;
    }
  }

  QString m_op{};
  Expression* m_next{nullptr};
  Expression* m_child{nullptr};
  Int m_value{0};
};

} // namespace puzzle_2020_18

void Solver_2020_18_1::solve(const QString& input)
{
  using namespace puzzle_2020_18;
  QStringList lines = common::splitLines(input);
  Int sum = 0;
  for (const QString& line : lines) {
    Expression exp(line);
    Int value;
    exp.eval(value);
    sum += value;
  }
  emit finished(QString::number(sum));
}

void Solver_2020_18_2::solve(const QString& input)
{
  using namespace puzzle_2020_18;
  QStringList lines = common::splitLines(input);
  Int sum = 0;
  for (const QString& line : lines) {
    Expression exp(line);
    exp.collapse();
    sum += exp.m_value;
  }
  emit finished(QString::number(sum));
}

