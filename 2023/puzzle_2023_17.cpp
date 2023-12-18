#include <2023/puzzle_2023_17.h>
#include <common.h>
#include <limits>
#include <functional>

#include <iostream>
#include <fstream>

namespace puzzle_2023_17 {

enum class Direction : unsigned int
{
    none,
    north,
    west,
    south,
    east
};

constexpr auto nb_directions = 5u;

QChar dirToChar(const Direction& dir) {
    if (dir == Direction::north)
        return '^';
    if (dir == Direction::west)
        return '<';
    if (dir == Direction::south)
        return 'v';
    if (dir == Direction::east)
        return '>';
    return '?';
}

inline Direction oppositeDirection(const Direction dir) {
    if (dir == Direction::north)
        return Direction::south;
    if (dir == Direction::west)
        return Direction::east;
    if (dir == Direction::south)
        return Direction::north;
    if (dir == Direction::east)
        return Direction::west;
    common::throwInvalidArgumentError("oppositeDirection: unknown direction");
    return dir;
}

inline QVector<Direction> orthogonalDirections(const Direction dir) {
    if (dir == Direction::north or dir == Direction::south)
        return QVector<Direction>{Direction::west, Direction::east};
    if (dir == Direction::west or dir == Direction::east)
        return QVector<Direction>{Direction::north, Direction::south};
    return QVector<Direction>{};
}

struct Coordinates
{
    Coordinates() = default;
    Coordinates(int x, int y) : x{x}, y{y} {}
    Coordinates(const Coordinates& previous, const Direction next) : Coordinates{previous}
    {
        if (next == Direction::north)
            --x;
        else if (next == Direction::west)
            --y;
        else if (next == Direction::south)
            ++x;
        else if (next == Direction::east)
            ++y;
        else
            common::throwInvalidArgumentError("Coordinates::constructor: unknown direction");
    }

    int x{0};
    int y{0};
};

bool operator == (const Coordinates& lhs, const Coordinates& rhs) {
    return lhs.x == rhs.x and lhs.y == rhs.y;
}

struct Node {
    Node() = default;
    Node(const Coordinates& coordinates) : coordinates{coordinates} {}
    Node(const Node& parent, Direction direction) :
        coordinates{parent.coordinates, direction},
        direction_to_parent{oppositeDirection(direction)} {
        if (direction_to_parent == parent.direction_to_parent)
            nb_steps = parent.nb_steps + 1u;
        else
            nb_steps = 0u;
    }

    QString toString() const {
        return QString("%1,%2,%3,%4")
                .arg(coordinates.x)
                .arg(coordinates.y)
                .arg(dirToChar(direction_to_parent))
                .arg(nb_steps);
    }

    QVector<Direction> complementDirections(const uint min_nb_steps) const {
        auto directions = QVector<Direction>{Direction::north, Direction::west, Direction::south, Direction::east};
        directions.removeOne(direction_to_parent);
        if (nb_steps + 1 < min_nb_steps) {
            for (auto dir : orthogonalDirections(direction_to_parent))
                directions.removeOne(dir);
        }
        return directions;
    }

    Coordinates coordinates{};
    Direction direction_to_parent{Direction::none};
    uint nb_steps{0};
};

bool operator == (const Node& lhs, const Node& rhs) {
    return lhs.coordinates == rhs.coordinates and lhs.direction_to_parent == rhs.direction_to_parent and lhs.nb_steps == rhs.nb_steps;
}

uint qHash(const Node& node) {
    return qHash(node.toString());
}

class CostMap
{
public:
    CostMap(int length, int width) : m_length{length}, m_width{width} {
        m_costs.resize(m_length);
        for (auto x = 0; x < m_length; ++x)
            m_costs[x].resize(m_width);
    }

    const QHash<Node, uint>& operator[](const Coordinates& c) const {
        return m_costs[c.x][c.y];
    }

    QHash<Node, uint>& operator[](const Coordinates& c) {
        return m_costs[c.x][c.y];
    }

    bool areValidCoordinates(const Coordinates& c) const {
        return c.x > -1 and c.x < m_length and c.y > -1 and c.y < m_width;
    }

    uint minCost(const Coordinates& c, const uint nb_steps_min) const {
        auto min_cost = std::numeric_limits<uint>::max();
        const auto& cell = (*this)[c];
        for (auto it = std::begin(cell); it != std::end(cell); ++it) {
            if (it.key().nb_steps + 1u >= nb_steps_min)
                min_cost = std::min(min_cost, it.value());
        }
        return min_cost;
    }

    uint getCost(const Node& node) const {
        const auto& cell = (*this)[node.coordinates];
        if (cell.contains(node))
            return cell[node];
        return std::numeric_limits<uint>::max();
    }

    void setCost(const Node& node, uint cost) {
        auto& cell = (*this)[node.coordinates];
        cell[node] = cost;
    }

private:
    int m_length;
    int m_width;
    std::vector<std::vector<QHash<Node, uint>>> m_costs{};
};


class Map
{
public:
    Map(const QString& input) {
        const auto lines = common::splitLines(input);
        m_length = lines.size();
        if (m_length == 0)
            common::throwInvalidArgumentError("empty map");
        m_blocks.reserve(m_length);
        m_width = lines.front().size();
        auto ok = true;
        for (const auto& line : lines) {
            if (line.size() != m_width)
                common::throwInvalidArgumentError("incoherent width");
            m_blocks.emplace_back();
            m_blocks.back().reserve(m_width);
            for (const auto& c : line) {
                m_blocks.back().emplace_back(static_cast<uint>(QString(c).toUInt(&ok)));
                if (not ok)
                    common::throwInvalidArgumentError("cast error");
            }
        }
    }

    CostMap computeCostMap(const Coordinates& start, const Coordinates& end, const uint nb_steps_min, const uint nb_steps_max) const
    {
        const auto start_node = Node{start};
        auto open_set = common::OpenSet<Node, uint>{};
        open_set.push(start_node, 0u);

        auto cost_map = CostMap{m_length, m_width};
        cost_map.setCost(start_node, 0u);

        const auto add_child = [&open_set, &cost_map, nb_steps_max, this] (const Node& father, const Direction direction) {
            const auto child = Node{father, direction};
            if (not cost_map.areValidCoordinates(child.coordinates) or child.nb_steps >= nb_steps_max)
                return;
            const auto new_cost = cost_map.getCost(father) + m_blocks[child.coordinates.x][child.coordinates.y];
            if (new_cost < cost_map.getCost(child)) {
                cost_map.setCost(child, new_cost);
                open_set.push(child, new_cost);
            }
        };

        while (not open_set.empty()) {
            const auto current = open_set.pop();
            if (not current.has_value())
                break;
            const auto& current_node = current->first;
            if (current_node.coordinates == end and current_node.nb_steps + 1u >= nb_steps_min)
                return cost_map;
            for (const auto direction : current_node.complementDirections(nb_steps_min))
                add_child(current_node, direction);
        }
        return cost_map;
    }

    QString solve(const uint nb_steps_min, const uint nb_steps_max) const {
        const auto start = Coordinates{0u, 0u};
        const auto end = Coordinates{m_length - 1, m_width - 1};
        const auto cost_map = computeCostMap(start, end, nb_steps_min, nb_steps_max);
        return QString("%1").arg(cost_map.minCost(end, nb_steps_min));
    }

private:
    std::vector<std::vector<uint>> m_blocks;
    int m_length;
    int m_width;
};

}

void Solver_2023_17_1::solve(const QString& input)
{
    const auto map = puzzle_2023_17::Map{input};
    emit finished(map.solve(1u, 3u));
}

void Solver_2023_17_2::solve(const QString& input)
{
    const auto map = puzzle_2023_17::Map{input};
    emit finished(map.solve(4u, 10u));
}
