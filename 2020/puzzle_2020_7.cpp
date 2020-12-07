#include <2020/puzzle_2020_7.h>
#include <common.h>
#include <QDebug>

namespace puzzle_2020_7
{

struct Rule
{
  Rule(const QString& container) : m_container{container} {}
  QString m_container;
  QMap<QString, int> m_content;
};

struct Bag;
struct Edge
{
  Edge(Bag* head, Bag* tail, int value) :
    m_head{head}, m_tail{tail}, m_value{value} {}
  Bag* m_head;
  Bag* m_tail;
  int m_value;
};

struct Bag
{
  Bag() = default;
  Bag(const QString& color) : m_color{color} {}

  void getAncestors(QMap<QString, Bag*>& ancestors) const
  {
    for (const Edge* edge : m_in_edges) {
      if (!ancestors.contains(edge->m_head->m_color)) {
        ancestors[edge->m_head->m_color] = edge->m_head;
        edge->m_head->getAncestors(ancestors);
      }
    }
  }

  int getNbAncestors() const
  {
    QMap<QString, Bag*> ancestors;
    getAncestors(ancestors);
    return ancestors.contains(m_color) ? ancestors.size() - 1 : ancestors.size();
  }

  int getDescendantsWeight(QMap<QString, Bag*>& descendants) const
  {
    if (m_out_edges.empty())
      return 1;
    int weight = 1;
    for (const Edge* edge : m_out_edges)
      if (!descendants.contains(edge->m_tail->m_color))
        weight += edge->m_value * edge->m_tail->getDescendantsWeight(descendants);
    return weight;
  }

  QList<Edge*> m_in_edges{};
  QList<Edge*> m_out_edges{};
  QString m_color{""};
};

struct AllBags
{
  AllBags(const QString& input)
  {
    // Parse rules and create bags
    const QStringList lines = common::splitLines(input);
    QList<Rule> rules;
    for (const QString& line : lines) {
      QStringList values = common::splitValues(line, ' ');
      for (auto it = values.begin(); it != values.end();) {
        const QString& word = *it;
        if (word.contains("bag") || word == "contain" || word == "no"  || word == "other")
          it = values.erase(it);
        else
          ++it;
      }
      if (values.size() < 2)
        continue;
      rules << Rule(values[0] + "_" + values[1]);
      for (int i = 2; i + 2 < values.size(); i += 3)
        rules.back().m_content[values[i + 1] + "_" + values[i + 2]] = values[i].toInt();
      if (!m_bags.contains(rules.back().m_container))
        m_bags[rules.back().m_container] = Bag(rules.back().m_container);
      for (const QString& key : rules.back().m_content.keys())
        if (!m_bags.contains(key))
          m_bags[key] = Bag(key);
    }

    // Apply rules
    for (const Rule& rule : rules) {
      Bag* bag = &m_bags[rule.m_container];
      for (const QString& key : rule.m_content.keys()) {
        m_edges << Edge(bag, &m_bags[key], rule.m_content[key]);
        bag->m_out_edges << &m_edges.back();
        m_bags[key].m_in_edges << &m_edges.back();
      }
    }
  }

  int getNbAncestors(const QString& bag_color) const {
    if (m_bags.contains(bag_color))
      return m_bags[bag_color].getNbAncestors();
    return 0;
  }

  int getDescendantsWeight(const QString& bag_color) const {
    if (m_bags.contains(bag_color)) {
      QMap<QString, Bag*> descendants;
      return m_bags[bag_color].getDescendantsWeight(descendants) - 1;
    }
    return 0;
  }

  QMap<QString, Bag> m_bags;
  QList<Edge> m_edges;
};

}

void Solver_2020_7_1::solve(const QString& input)
{
  using namespace puzzle_2020_7;
  AllBags bags(input);
  emit finished(QString::number(bags.getNbAncestors("shiny_gold")));
}

void Solver_2020_7_2::solve(const QString& input)
{
  using namespace puzzle_2020_7;
  using namespace puzzle_2020_7;
  AllBags bags(input);
  emit finished(QString::number(bags.getDescendantsWeight("shiny_gold")));
}

