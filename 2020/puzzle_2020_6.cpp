#include <2020/puzzle_2020_6.h>
#include <common.h>
#include <QMap>

namespace puzzle_2020_6
{

const QString default_questions_labels = "abcdefghijklmnopqrstuvwxyz";

class DeclarationForm
{
public:
  DeclarationForm(const QString& questions_labels = default_questions_labels)
  {
    for (const QChar& label : questions_labels)
      m_q_and_a[label] = false;
  }

  void submitPositiveAnswers(const QString& positive_answers)
  {
    for (const QChar& label : positive_answers)
      if (m_q_and_a.contains(label))
        m_q_and_a[label] = true;
  }

  int nbQuestions() const { return m_q_and_a.size(); }

  int nbPositiveAnswers() const
  {
    int nb_positive = 0;
    for (bool answer : m_q_and_a.values())
      if (answer)
        ++nb_positive;
    return nb_positive;
  }

  QString positiveAnswers() const
  {
    QString positives;
    for (const QChar& label : m_q_and_a.keys())
      if (m_q_and_a[label])
        positives.push_back(label);
    return positives;
  }

  int nbNegativeAnswers() const
  {
    int nb_negatives = 0;
    for (bool answer : m_q_and_a.values())
      if (!answer)
        ++nb_negatives;
    return nb_negatives;
  }

  QString negativeAnswers() const
  {
    QString negatives;
    for (const QChar& label : m_q_and_a.keys())
      if (!m_q_and_a[label])
        negatives.push_back(label);
    return negatives;
  }

  bool operator[](const QChar& label) const
  {
    if (!m_q_and_a.contains(label))
      return false;
    return m_q_and_a[label];
  }

private:
  QMap<QChar, bool> m_q_and_a;
};

class Group
{
public:
  Group(const QString& questions_labels = default_questions_labels) :
  m_questions_labels{questions_labels} {}

  void operator << (const QString& form)
  {
    m_forms << DeclarationForm(m_questions_labels);
    m_forms.back().submitPositiveAnswers(form);
  }

  bool anyoneYes(const QChar& label) const {
    for (const DeclarationForm& form : m_forms)
      if (form[label])
        return true;
    return false;
  }

  bool anyoneNo(const QChar& label) const {
    for (const DeclarationForm& form : m_forms)
      if (!form[label])
        return true;
    return false;
  }

  bool everyoneYes(const QChar& label) const {
    for (const DeclarationForm& form : m_forms)
      if (!form[label])
        return false;
    return true;
  }

  bool everyoneNo(const QChar& label) const {
    for (const DeclarationForm& form : m_forms)
      if (form[label])
        return false;
    return true;
  }

  QString anyoneYes() const
  {
    QString result;
    for (const QChar& label : m_questions_labels)
      if (anyoneYes(label))
        result.push_back(label);
    return result;
  }

  QString anyoneNo() const
  {
    QString result;
    for (const QChar& label : m_questions_labels)
      if (anyoneNo(label))
        result.push_back(label);
    return result;
  }

  QString everyoneYes() const
  {
    QString result;
    for (const QChar& label : m_questions_labels)
      if (everyoneYes(label))
        result.push_back(label);
    return result;
  }

  QString everyoneNo() const
  {
    QString result;
    for (const QChar& label : m_questions_labels)
      if (everyoneNo(label))
        result.push_back(label);
    return result;
  }

  int nbAnyoneYes() const { return anyoneYes().size(); }

  int nbAnyoneNo() const { return anyoneNo().size(); }

  int nbEveryoneYes() const { return everyoneYes().size(); }

  int nbEveryoneNo() const { return everyoneNo().size(); }

  const QList<DeclarationForm>& forms() const { return m_forms; }

  int size() const { return m_forms.size(); }

  bool isEmpty() const { return m_forms.isEmpty(); }

private:
  QList<DeclarationForm> m_forms;
  const QString& m_questions_labels;
};

class AllGroups
{
public:
  AllGroups(const QString& input,
            const QString& questions_labels = default_questions_labels) :
    m_questions_labels{questions_labels}
  {
    const QStringList lines = common::splitLines(input);
    if (lines.empty())
      return;
    m_groups << Group(m_questions_labels);
    for (const QString& line : lines) {
      if (line.isEmpty()) {
        if (!m_groups.back().isEmpty())
         m_groups << Group(m_questions_labels);
        continue;
      }
      m_groups.back() << line;
    }
    if (m_groups.back().isEmpty())
      m_groups.pop_back();
  }

  int anyoneYesSum() const {
    int sum = 0;
    for (const Group& group : m_groups)
      sum += group.nbAnyoneYes();
    return sum;
  }

  int anyoneNoSum() const {
    int sum = 0;
    for (const Group& group : m_groups)
      sum += group.nbAnyoneNo();
    return sum;
  }

  int everyoneYesSum() const {
    int sum = 0;
    for (const Group& group : m_groups)
      sum += group.nbEveryoneYes();
    return sum;
  }

  int everyoneNoSum() const {
    int sum = 0;
    for (const Group& group : m_groups)
      sum += group.nbEveryoneNo();
    return sum;
  }

  const QList<Group>& groups() const { return m_groups; }

  int size() const { return m_groups.size(); }

  bool isEmpty() const { return m_groups.isEmpty(); }

private:
  QList<Group> m_groups;
  const QString& m_questions_labels;
};

}

void Solver_2020_6_1::solve(const QString& input) const
{
  using namespace puzzle_2020_6;
  const AllGroups all_groups(input);
  emit finished(QString::number(all_groups.anyoneYesSum()));
}

void Solver_2020_6_2::solve(const QString& input) const
{
  using namespace puzzle_2020_6;
  const AllGroups all_groups(input);
  emit finished(QString::number(all_groups.everyoneYesSum()));
}

