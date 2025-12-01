#include <QSet>
#include <solvers/common.h>
#include <optional>
#include <solvers/2021/puzzle_2021_23.h>

namespace puzzle_2021_23 {

constexpr auto hallway_index_to_x = std::array<int, 7>{0, 1, 3, 5, 7, 9, 10};
constexpr auto hallway_x_to_index =
    std::array<int, 11>{0, 1, -1, 2, -1, 3, -1, 4, -1, 5, 6};
const auto amphipods = QString("ABCD");
const auto energies =
    QMap<QChar, uint>{{'A', 1u}, {'B', 10u}, {'C', 100u}, {'D', 1000u}};

struct State : public QString {
  State() : QString{"......."} {}

  QHash<State, uint> neighbors(uint energy) const {
    const auto rooms_size = (size() - 7) / 4;

    const auto valid_rooms = [rooms_size, this]() {
      auto rooms = QSet<int>{};
      for (auto room_index = 0; room_index < 4; ++room_index) {
        auto matching_char = amphipods[room_index];
        auto valid = true;
        for (auto i = rooms_size - 1; valid and i >= 0; --i) {
          const auto index = room_index * rooms_size + 7 + i;
          if ((*this)[index] != matching_char)
            matching_char = '.';
          if ((*this)[index] != matching_char)
            valid = false;
        }
        if (valid)
          rooms.insert(room_index);
      }
      return rooms;
    }();

    const auto point = [rooms_size](auto index) {
      if (index > 6) {
        const auto shifted = index - 7;
        const auto room_index = shifted / rooms_size;
        const auto slot_in_room = shifted % rooms_size;
        return QPoint(2 * (room_index + 1), slot_in_room + 1);
      } else
        return QPoint(hallway_index_to_x[index], 0);
    };

    const auto is_top = [rooms_size, this](auto room_index, auto index,
                                           bool in) {
      const auto pivot = in ? index + 1 : index;
      for (auto i = 7 + room_index * rooms_size; i < pivot; ++i)
        if ((*this)[i] != '.')
          return false;
      for (auto i = pivot; i < 7 + (room_index + 1) * rooms_size; ++i)
        if ((*this)[i] == '.')
          return false;
      return true;
    };

    auto res = QHash<State, uint>{};
    for (auto from = 0; from < size(); ++from) {
      if ((*this)[from] == '.')
        continue;
      const auto from_pt = point(from);
      if (from_pt.y() != 0) {
        const auto room_index = from_pt.x() / 2 - 1;
        if (valid_rooms.contains(room_index))
          continue;
        if (not is_top(room_index, from, false))
          continue;
      }
      for (auto to = 0; to < size(); ++to) {

        const auto to_pt = point(to);
        if ((from_pt.y() == 0) ^ (to_pt.y() != 0))
          continue;

        const auto x_min = std::min(from_pt.x(), to_pt.x());
        const auto x_max = std::max(from_pt.x(), to_pt.x());
        auto valid_hallway_move = true;
        for (auto x = x_min; valid_hallway_move and x <= x_max; ++x)
          if (x != from_pt.x() and hallway_x_to_index[x] >= 0 and
              (*this)[hallway_x_to_index[x]] != '.')
            valid_hallway_move = false;
        if (not valid_hallway_move)
          continue;

        if (to_pt.y() != 0) {
          const auto room_index = to_pt.x() / 2 - 1;
          if ((*this)[from] != amphipods[room_index])
            continue;
          if (not valid_rooms.contains(room_index))
            continue;
          if (not is_top(room_index, from_pt.y() == 0 ? to : from,
                         from_pt.y() == 0))
            continue;
        }

        auto neighbor = State{*this};
        neighbor[to] = neighbor[from];
        neighbor[from] = '.';
        res[neighbor] = energy + uint((from_pt - to_pt).manhattanLength()) *
                                     energies[(*this)[from]];
      }
    }
    return res;
  }
};

class Burrow {
public:
  Burrow(const QString &input, bool unfolded = false) {
    auto rx_top = QRegExp("^###(A|B|C|D)#(A|B|C|D)#(A|B|C|D)#(A|B|C|D)###$");
    auto rx_mid = QRegExp("^  #(A|B|C|D)#(A|B|C|D)#(A|B|C|D)#(A|B|C|D)#$");
    auto lines = common::splitLines(input);
    auto rooms = std::array<QString, 4>{};
    while (not lines.empty()) {
      const auto line = lines.front();
      lines.pop_front();
      if (rx_top.exactMatch(line)) {
        for (auto i = 0; i < 4; ++i)
          rooms[i].push_front(rx_top.cap(i + 1));
        if (unfolded) {
          lines.push_front("  #D#B#A#C#");
          lines.push_front("  #D#C#B#A#");
        }
      } else if (rx_mid.exactMatch(line)) {
        for (auto i = 0; i < 4; ++i)
          rooms[i].push_back(rx_mid.cap(i + 1));
      }
    }
    const auto max_size =
        std::max_element(std::cbegin(rooms), std::cend(rooms),
                         [](const auto &lhs, const auto &rhs) {
                           return lhs.size() < rhs.size();
                         })
            ->size();
    for (auto i = 0; i < 4; ++i)
      while (rooms[i].size() < max_size)
        rooms[i].push_back('.');
    for (auto i = 0; i < 4; ++i)
      m_root.push_back(rooms[i]);
    for (const auto &amph : amphipods)
      for (auto i = 0; i < max_size; ++i)
        m_goal.push_back(amph);
  }

  QString lowestEnergy() const {
    auto open_set = common::OpenSet<State, uint>{};
    auto current = std::optional<std::pair<State, uint>>{};
    current = std::make_pair(m_root, 0u);
    while (current) {
      if (current->first == m_goal)
        return QString("%1").arg(current->second);
      const auto neighbors = current->first.neighbors(current->second);
      for (auto it = std::cbegin(neighbors); it != std::cend(neighbors); ++it)
        open_set.push(it.key(), it.value());
      current = open_set.pop();
    }
    return "Failure";
  }

private:
  State m_root;
  State m_goal;
};

} // namespace puzzle_2021_23

void Solver_2021_23_1::solve(const QString &input) {
  emit finished(puzzle_2021_23::Burrow(input).lowestEnergy());
}

void Solver_2021_23_2::solve(const QString &input) {
  emit finished(puzzle_2021_23::Burrow(input, true).lowestEnergy());
}
