#include <solvers/common.h>
#include <set>
#include <solvers/2015/puzzle_2015_03.h>

using Int = long long int;

struct DeliveryMan {
  void move(const QChar &chr) {
    if (chr == '<')
      --m_x;
    else if (chr == '>')
      ++m_x;
    else if (chr == 'v')
      --m_y;
    else if (chr == '^')
      ++m_y;
  }
  Int m_x{0};
  Int m_y{0};
};

struct House {
  House(const DeliveryMan &dm) : m_x{dm.m_x}, m_y{dm.m_y} {}
  bool operator<(const House &other) const {
    if (m_x == other.m_x)
      return m_y < other.m_y;
    return m_x < other.m_x;
  }
  Int m_x{0};
  Int m_y{0};
};

struct Houses {
  Houses(const QString &input, uint nb_delivery_man) {
    if (nb_delivery_man == 0u)
      return;
    auto delivery_men = std::vector<DeliveryMan>{};
    delivery_men.resize(nb_delivery_man);
    for (const auto &dm : delivery_men)
      m_houses.insert(House{dm});
    auto dm = std::begin(delivery_men);
    for (const auto &chr : input) {
      dm->move(chr);
      m_houses.insert(House{*dm});
      ++dm;
      if (dm == std::end(delivery_men))
        dm = std::begin(delivery_men);
    }
  }

  QString nbHouses() const { return QString("%1").arg(m_houses.size()); }

  std::set<House> m_houses;
};

void Solver_2015_03_1::solve(const QString &input) {
  emit finished(Houses(input, 1).nbHouses());
}

void Solver_2015_03_2::solve(const QString &input) {
  emit finished(Houses(input, 2).nbHouses());
}
