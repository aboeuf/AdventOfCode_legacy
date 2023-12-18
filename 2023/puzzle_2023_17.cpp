#include <2023/puzzle_2023_17.h>
#include <common.h>
#include <limits>
#include <functional>

#include <iostream>
#include <fstream>

namespace puzzle_2023_17 {

enum class Direction
{
    none,
    north,
    west,
    south,
    east
};

const auto directions = std::array<Direction, 4>{Direction::north, Direction::west, Direction::south, Direction::east};

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

    int x{-1};
    int y{-1};
};

bool operator == (const Coordinates& lhs, const Coordinates& rhs) {
    return lhs.x == rhs.x and lhs.y == rhs.y;
}

uint qHash(const Coordinates& c) {
    return qHash(QString("%1,%2").arg(c.x).arg(c.y));
}

struct CostAndPrevious
{
    uint cost{std::numeric_limits<uint>::max()};
    Direction previous{Direction::none};
};

class CostAndPreviousMap
{
public:
    CostAndPreviousMap(int length, int width) : m_length{length}, m_width{width} {
        m_map.reserve(length);
        for (auto i = 0; i < length; ++i) {
            m_map.emplace_back();
            m_map.back().reserve(width);
            for (auto j = 0; j < width; ++j)
                m_map.back().emplace_back();
        }
    }

    QString toString() const {
        const auto separator = "\n" + QString{5 * m_width + 4, QChar{'-'}} + "\n";
        auto str = separator + "|  |";
        for (auto i = 0; i < m_width; ++i) {
            str += QString("%1|").arg(i, 4, 10, QChar('0'));
        }
        str += separator;
        for (auto i = 0; i < m_length; ++i) {
            str += QString("|%1|").arg(i, 2, 10, QChar('0'));
            for (auto j = 0; j < m_width; ++j) {
                const auto& data = m_map[i][j];
                str += QString("%1").arg(std::min(data.cost, 999u), 3, 10, QChar('0'));
                str.push_back(dirToChar(data.previous));
                str.push_back('|');
            }
            str += separator;
        }
        return str;
    }

    bool areValidCoordinates(const Coordinates& c) const {
        return c.x > -1 and c.x < m_length and c.y > -1 and c.y < m_width;
    }

    bool isConstraintViolated(const Coordinates& c, const std::optional<uint>& constraint) const {
        if (not areValidCoordinates(c))
            return true;
        if (not constraint.has_value())
            return false;
        const auto direction = m_map[c.x][c.y].previous;
        if (direction == Direction::none)
            return false;
        auto current = Coordinates{c, direction};
        for (auto i = 0u; i + 1u < *constraint; ++i) {
            if (not areValidCoordinates(current))
                return false;
            if (direction != m_map[current.x][current.y].previous)
                return false;
            current = Coordinates{current, direction};
        }
        return true;
    }

    const CostAndPrevious& operator[] (const Coordinates& c) const {
        if (not areValidCoordinates(c))
            common::throwInvalidArgumentError(QString("CostAndPreviousMap::operator[]: out of range (%1, %2)").arg(c.x).arg(c.y));
        return m_map[c.x][c.y];
    }

    CostAndPrevious& operator[] (const Coordinates& c) {
        if (not areValidCoordinates(c))
            common::throwInvalidArgumentError(QString("CostAndPreviousMap::operator[]: out of range (%1, %2)").arg(c.x).arg(c.y));
        return m_map[c.x][c.y];
    }


private:
    std::vector<std::vector<CostAndPrevious>> m_map;
    int m_length;
    int m_width;
};

struct Node
{
    Node() : index{std::numeric_limits<uint>::max()} {}
    Node(const Coordinates& coordinates) : index{next_index++}, coordinates{coordinates} {}

    uint index{0};
    Coordinates coordinates{};
    std::optional<uint> previous{std::nullopt};
    uint cost{0};

    static uint next_index;
};

uint Node::next_index = uint{0};

bool operator == (const Node& lhs, const Node& rhs) {
    return lhs.index == rhs.index;
}

uint qHash(const Node& node) {
    return node.index;
}

struct Tree
{
    std::optional<Node> getPreviousNode(const Node& node) {
        if (not node.previous.has_value())
            return std::nullopt;
        if (nodes.contains(*node.previous))
            return nodes[*node.previous];
        return std::nullopt;
    }

    QHash<uint, Node> nodes{};
    std::optional<Node> optimal_leaf{std::nullopt};
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

    CostAndPreviousMap getUnconstrainedMap(const Coordinates& start) const
    {
        auto map = CostAndPreviousMap{m_length, m_width};
        map[start].cost = 0;

        auto open_set = common::OpenSet<Coordinates, uint>{};
        open_set.push(start, 0);

        const auto add_child = [&map, &open_set, this] (const Coordinates& current_coordinates, const Direction next_direction) {
            auto& current = map[current_coordinates];
            if (current.previous == next_direction)
                return;
            const auto next_coordinates = Coordinates{current_coordinates, next_direction};
            if (map.areValidCoordinates(next_coordinates)) {
                auto& next = map[next_coordinates];
                const auto new_cost = current.cost + m_blocks[next_coordinates.x][next_coordinates.y];
                if (new_cost < next.cost) {
                    next.cost = new_cost;
                    next.previous = oppositeDirection(next_direction);
                    open_set.push(next_coordinates, new_cost);
                }
            }
        };

        while (not open_set.empty()) {
            const auto current = open_set.pop();
            if (not current.has_value())
                break;
            for (const auto direction : directions)
                add_child(current->first, direction);
        }
        return map;
    }

    CostAndPreviousMap getConstrainedMap(const Coordinates& start, const Coordinates& end, const std::optional<uint>& constraint = std::nullopt) const
    {
        const auto unconstrained_map = getUnconstrainedMap(end);

        auto map = CostAndPreviousMap{m_length, m_width};
        map[start].cost = 0;

        auto open_set = common::OpenSet<Coordinates, uint>{};
        open_set.push(start, 0);

        const auto add_child = [constraint, &unconstrained_map, &map, &open_set, this] (const Coordinates& current_coordinates, const Direction next_direction) {
            auto& current = map[current_coordinates];
            if (current.previous == next_direction)
                return;
            const auto next_coordinates = Coordinates{current_coordinates, next_direction};
            if (unconstrained_map.isConstraintViolated(next_coordinates, constraint))
                return;
            auto& next = map[next_coordinates];
            const auto new_cost = current.cost + m_blocks[next_coordinates.x][next_coordinates.y];
            if (new_cost < next.cost) {
                next.cost = new_cost;
                next.previous = oppositeDirection(next_direction);
                open_set.push(next_coordinates, new_cost);
            }
        };

        while (not open_set.empty()) {
            const auto current = open_set.pop();
            if (not current.has_value())
                break;
            for (const auto direction : directions)
                add_child(current->first, direction);
        }
        return map;
    }

    Tree getOptimalPath(const Coordinates& start, const Coordinates& end, const std::optional<uint>& constraint = std::nullopt) const
    {
        const auto map = getUnconstrainedMap(start);

        auto tree = Tree{};
        const auto root_node = Node{start};
        tree.nodes[0] = root_node;

        auto open_set = common::OpenSet<Node, uint>{};
        open_set.push(root_node, 0);

        const auto add_child = [&constraint, &map, &tree, &open_set, this](const Node& current, Direction direction) {
            const auto next_coordinates = Coordinates{current.coordinates, direction};
            if (map.isConstraintViolated(next_coordinates, constraint))
                return;
            if (map[next_coordinates].previous != oppositeDirection(direction))
                return;
            auto node = Node{next_coordinates};
            node.previous = current.index;
            node.cost = current.cost + m_blocks[next_coordinates.x][next_coordinates.y];
            tree.nodes[node.index] = node;
            open_set.push(node, node.cost);
        };

        while (not open_set.empty()) {
            const auto current = open_set.pop();
            if (not current.has_value())
                return tree;
            const auto& current_node = current->first;
            if (current_node.coordinates == end) {
                tree.optimal_leaf = current_node;
                return tree;
            }
            for (const auto direction : directions)
                add_child(current_node, direction);
        }
        return tree;
    }

    QString solveOne() const {
        const auto start = Coordinates{0, 0};
        const auto end = Coordinates{m_length - 1, m_width - 1};
        const auto tree = getOptimalPath(start, end, 3u);
        if (tree.optimal_leaf.has_value())
            return QString("%1").arg(tree.optimal_leaf->cost);
        return "Failure";
    }

    QString solveTwo() const {
        const auto start = Coordinates{0, 0};
        const auto end = Coordinates{m_length - 1, m_width - 1};
        const auto map = getConstrainedMap(start, end, 3);
        return map.toString();
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
    emit finished(map.solveOne());
}

void Solver_2023_17_2::solve(const QString& input)
{
    const auto map = puzzle_2023_17::Map{input};
    emit output(map.solveTwo());
    emit finished("Done");
}
