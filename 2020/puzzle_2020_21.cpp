#include <2020/puzzle_2020_21.h>
#include <common.h>
#include <QDebug>

namespace puzzle_2020_21
{

struct Ingredient;
struct Food;
struct Allergen
{
  Allergen() = default;
  Allergen(const QString& label) : m_label{label} {}

  void debug() const
  {
    QString s = m_label + " (";
    for (const QString& ingredient : m_contained_by_ingredients.keys())
      s += ingredient + ", ";
    if (s.endsWith(", "))
      s.chop(2);
    s += ")";
    qDebug() << s.toStdString().c_str();
  }

  QString m_label{};
  QMap<QString, Ingredient*> m_contained_by_ingredients{};
  QSet<Food*> m_contained_by_foods{};
};

struct Ingredient
{
  Ingredient() = default;
  Ingredient(const QString& label) : m_label{label} {}

  void debug() const
  {
    QString s = m_label + " (";
    for (const QString& allergen : m_contained_allergens.keys())
      s += allergen + ", ";
    if (s.endsWith(", "))
      s.chop(2);
    s += ")";
    qDebug() << s.toStdString().c_str();
  }

  QString m_label{};
  QMap<QString, Allergen*> m_contained_allergens{};
  QSet<Food*> m_contained_by_foods{};
};

using Arrangement = QMap<QString, QString>;

struct Food
{
  Food() = default;

  void debug() const
  {
    QString s;
    for (const QString& ingredient : m_ingredients.keys())
      s += ingredient + " ";
    s += "(contains ";
    for (const QString& allergen : m_allergens.keys())
      s += allergen + ", ";
    if (s.endsWith(", "))
      s.chop(2);
    s += ") [";
    for (const QString& ingredient : m_non_empty_ingredients.keys())
      s += ingredient + ", ";
    if (s.endsWith(", "))
      s.chop(2);
    s += "]";
    qDebug() << s.toStdString().c_str();
  }

  void setCandidates()
  {
    for (Ingredient* ingredient : m_ingredients.values())
      for (Allergen* allergen : m_allergens.values())
        allergen->m_contained_by_ingredients[ingredient->m_label] = ingredient;
    for (Allergen* allergen : m_allergens.values())
      for (Ingredient* ingredient : m_ingredients.values())
        ingredient->m_contained_allergens[allergen->m_label] = allergen;
  }

  bool isValidArrangement(const Arrangement& arr) const
  {
    QSet<QString> allergens;
    for (const QString& ingredient : arr.keys())
      if (m_non_empty_ingredients.contains(ingredient) &&
          m_non_empty_ingredients[ingredient]->m_contained_allergens.contains(arr[ingredient]) &&
          m_allergens.contains(arr[ingredient]))
        allergens.insert(arr[ingredient]);
    return allergens.size() == m_allergens.size();
  }

  QMap<QString, Ingredient*> m_ingredients{};
  QMap<QString, Allergen*> m_allergens{};
  QMap<QString, Ingredient*> m_non_empty_ingredients{};
};

struct FoodsList
{
  FoodsList(const QString& input)
  {
    QStringList lines = common::splitLines(input);
    for (QString& line : lines) {
      QStringList labels = common::splitValues(line, ' ');
      m_foods << Food();
      QStringList::iterator label = labels.begin();
      for (; *label != "(contains" && label != labels.end(); ++label) {
        if (!m_ingredients.contains(*label))
          m_ingredients[*label] = Ingredient(*label);
        Ingredient* ingredient = &m_ingredients[*label];
        m_foods.back().m_ingredients[*label] = ingredient;
        ingredient->m_contained_by_foods.insert(&m_foods.back());
      }
      ++label;
      for (; label != labels.end(); ++label) {
        label->chop(1);
        if (!m_allergens.contains(*label))
          m_allergens[*label] = Allergen(*label);
        Allergen* allergen = &m_allergens[*label];
        m_foods.back().m_allergens[*label] = allergen;
        allergen->m_contained_by_foods.insert(&m_foods.back());
      }
      m_foods.back().m_non_empty_ingredients = m_foods.back().m_ingredients;
    }
    for (Food& food : m_foods)
      food.setCandidates();
    for (const QString& al_label : m_allergens.keys()) {
      Allergen* allergen = &m_allergens[al_label];
      for (const QString& in_label : m_ingredients.keys()) {
        Ingredient* ingredient = &m_ingredients[in_label];
        for (Food* food : allergen->m_contained_by_foods) {
          if (!food->m_ingredients.contains(in_label)) {
            ingredient->m_contained_allergens.remove(al_label);
            allergen->m_contained_by_ingredients.remove(in_label);
          }
        }
      }
    }
    for (Food& food : m_foods)
      for (Ingredient* ingredient : food.m_ingredients)
        if (ingredient->m_contained_allergens.isEmpty()) {
          ++m_puzzle_1;
          food.m_non_empty_ingredients.remove(ingredient->m_label);
        }
  }

  void debug()
  {
    qDebug() << "";
    qDebug() << "FOODS";
    for (const Food& food : m_foods)
      food.debug();
    qDebug() << "";
    qDebug() << "INGREDIENTS";
    for (const Ingredient& ingredient : m_ingredients.values())
      ingredient.debug();
    qDebug() << "";
    qDebug() << "ALLERGENS";
    for (const Allergen& allergen : m_allergens.values())
      allergen.debug();
  }

  bool isValidArrangement(const Arrangement& arr) const
  {
    for (const Food& food : m_foods)
      if (!food.isValidArrangement(arr))
        return false;
    return true;
  }

  QList<Arrangement> possibleArrangements() const
  {
    QList<Arrangement> arr = {Arrangement{}};
    for (const Ingredient& ingredient : m_ingredients) {
      QList<Arrangement> new_arr;
      for (const QString& all : ingredient.m_contained_allergens.keys())
        for (Arrangement a : arr)
          if (!a.values().contains(all)) {
            a[ingredient.m_label] = all;
            new_arr << a;
          }
      arr.append(new_arr);
    }
    return arr;
  }

  QList<Arrangement> validArrangements() const
  {
    QList<Arrangement> arr = possibleArrangements();
    auto it = arr.begin();
    while (it != arr.end()) {
      if (isValidArrangement(*it))
        ++it;
      else
        it = arr.erase(it);
    }
    return arr;
  }

  QString cannonicals() const
  {
    QString result;
    for (const Arrangement a : validArrangements())
    {
      Arrangement arr;
      for (const QString& in : a.keys())
        arr[a[in]] = in;
      QString can;
      for (QString in_sorted : arr.values())
        can += in_sorted + ",";
      if (can.endsWith(","))
        can.chop(1);
      result += can + "\n";
    }
    if (result.endsWith("\n"))
      result.chop(1);
    return result;
  }

  QMap<QString, Ingredient> m_ingredients;
  QMap<QString, Allergen> m_allergens;
  QList<Food> m_foods;
  uint m_puzzle_1{0};
};

} // namespace puzzle_2020_21

void Solver_2020_21_1::solve(const QString& input)
{
  using namespace puzzle_2020_21;
  FoodsList list(input);
  emit finished(QString::number(list.m_puzzle_1));
}

void Solver_2020_21_2::solve(const QString& input)
{
  using namespace puzzle_2020_21;
  FoodsList list(input);
  emit finished(list.cannonicals());
}

