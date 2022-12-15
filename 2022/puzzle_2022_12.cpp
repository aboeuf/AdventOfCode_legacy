#include <2022/puzzle_2022_12.h>
#include <common.h>
#include <QMap>

namespace puzzle_2022_12 {

const auto alphabet = QString{"abcdefghijklmnopqrstuvwxyz"};

const auto char_to_elevation = []() {
    auto res = QMap<QChar, uint>{};
    for (auto i = 0; i < 26; ++i)
        res.insert(alphabet[i], i);
    res.insert('S', 0);
    res.insert('E', 26);
    return res;
}();

struct Position
{
    Position() = default;
    Position(uint i, uint j) : i{i}, j{j} {}

    uint i{0};
    uint j{0};

    bool operator == (const Position& other) const
    {
        return i == other.i and j == other.j;
    }

    uint distance(const Position& other) const
    {
        return (i < other.i ? other.i - i : i - other.i) +
                (j < other.j ? other.j - j : j - other.j);
    }
};

using Path = std::list<Position>;

struct Cell
{
    Cell() = default;
    Cell(QChar elevation, uint i, uint j) :
        elevation{char_to_elevation[elevation]}, pos{i, j} {}

    uint fCost() const { return gCost + hCost; }

    void reset()
    {
        gCost = std::numeric_limits<uint>::max();
        previous = nullptr;
        is_in_set = false;
    }

    uint elevation{0u};
    Position pos{};
    uint hCost{0u};
    uint gCost{std::numeric_limits<uint>::max()};
    Cell* previous{nullptr};
    bool is_in_set{false};
};

class OpenSet
{
public:
    bool empty() const { return m_set.empty(); }

    Cell* pop()
    {
        auto it = std::min_element(m_set.begin(), m_set.end(), [](auto lhs, auto rhs) {
            return lhs->fCost() < rhs->fCost();
        });
        if (it == std::end(m_set))
            return nullptr;
        auto* cell = *it;
        cell->is_in_set = false;
        m_set.erase(it);
        return cell;
    }

    void push(Cell* cell)
    {
        if (not cell->is_in_set) {
            cell->is_in_set = true;
            m_set.push_back(cell);
        }
    }

private:
    std::list<Cell*> m_set{};
};

class ElevationMap
{
public:
    ElevationMap(const QString& input)
    {
        const auto lines = common::splitLines(input);
        m_N = static_cast<std::size_t>(lines.size());
        m_M = static_cast<std::size_t>(lines.front().size());
        m_map.resize(m_N);
        for (auto i = 0u; i < m_N; ++i) {
            m_map[i].reserve(m_M);
            for (auto j = 0u; j < m_M; ++j) {
                m_map[i].push_back(new Cell(lines[i][j], i, j));
                if (lines[i][j] == 'S')
                    m_start = m_map[i].back();
                else if (lines[i][j] == 'E')
                    m_goal = m_map[i].back();
            }
        }
        for (auto& row : m_map)
            for (auto* cell : row)
                cell->hCost = cell->pos.distance(m_goal->pos);
    }

    ~ElevationMap() {
        for (auto& row : m_map)
            for (auto* cell : row)
                delete cell;
    }

    Position start() const { return m_start->pos; }
    Position goal() const { return m_goal->pos; }

    std::vector<Position> zeroElevationPositions() const
    {
        auto res = std::vector<Position>{};
        res.reserve(m_N * m_M);
        for (auto& row : m_map)
            for (auto* cell : row)
                if (cell->elevation == 0u)
                    res.push_back(cell->pos);
        return res;
    }

    Path shortestPath(const Position& start, const Position& goal) const
    {
        for (auto& row : m_map)
            for (auto* cell : row)
                cell->reset();
        auto* goal_cell = m_map[goal.i][goal.j];
        auto* cell = m_map[start.i][start.j];
        cell->gCost = 0u;
        auto set = OpenSet{};
        set.push(cell);
        while (not set.empty()) {
            cell = set.pop();
            if (cell == goal_cell) {
                auto path = Path{};
                do {
                    path.push_front(cell->pos);
                    cell = cell->previous;
                } while (cell != nullptr);
                return path;
            }
            auto neighbors = getNeighbors(cell);
            for (auto* neighbor : neighbors) {
                const auto new_cost = cell->gCost + 1u;
                if (new_cost < neighbor->gCost) {
                    neighbor->previous = cell;
                    neighbor->gCost = new_cost;
                    set.push(neighbor);
                }
            }
        }
        return Path{};
    }

    QString drawPath(const Path& path) const
    {
        auto str_list = QStringList{};
        for (auto i = 0u; i < m_N; ++i) {
            str_list << "";
            str_list.back().fill('.', m_M);
        }
        if (not path.empty()) {
            for (auto it = std::begin(path); std::next(it) != std::end(path); ++it) {
                const auto& curr = *it;
                const auto& next = *(std::next(it));
                if (curr.i == next.i) {
                  if (curr.j + 1 == next.j)
                      str_list[curr.i][curr.j] = '>';
                  else if (curr.j == next.j + 1)
                      str_list[curr.i][curr.j] = '<';
                  else
                      str_list[curr.i][curr.j] = '@';
                } else if(curr.j == next.j) {
                    if (curr.i + 1 == next.i)
                        str_list[curr.i][curr.j] = 'v';
                    else if (curr.i == next.i + 1)
                        str_list[curr.i][curr.j] = '^';
                    else
                        str_list[curr.i][curr.j] = '@';
                } else
                    str_list[curr.i][curr.j] = '@';
            }
            str_list[path.back().i][path.back().j] = 'E';
        }
        auto res = QString{};
        for (const auto& row : str_list)
            res += row + "\n";
        return res;
    }

private:
    std::vector<Cell*> getNeighbors(Cell* cell) const
    {
        auto neighbors = std::vector<Cell*>{};
        neighbors.reserve(4);
        const auto add_neighbor = [&neighbors, cell] (Cell* neighbor)
        {
            if (cell->elevation >= neighbor->elevation or cell->elevation + 1u == neighbor->elevation)
                neighbors.push_back(neighbor);
        };
        if (cell->pos.i != 0u)
            add_neighbor(m_map[cell->pos.i - 1u][cell->pos.j]);
        if (cell->pos.j != 0u)
            add_neighbor(m_map[cell->pos.i][cell->pos.j - 1u]);
        if (cell->pos.i + 1u < m_N)
            add_neighbor(m_map[cell->pos.i + 1u][cell->pos.j]);
        if (cell->pos.j + 1u < m_M)
            add_neighbor(m_map[cell->pos.i][cell->pos.j + 1u]);
        return neighbors;
    }

    std::vector<std::vector<Cell*>> m_map{};
    std::size_t m_N{0};
    std::size_t m_M{0};
    Cell* m_start{nullptr};
    Cell* m_goal{nullptr};
};

}

void Solver_2022_12_1::solve(const QString& input)
{
    const auto map = puzzle_2022_12::ElevationMap{input};
    const auto path = map.shortestPath(map.start(), map.goal());
    emit output(map.drawPath(path));
    if (path.empty())
        emit finished("NO SOLUTION");
    else
        emit finished(QString("%1").arg(path.size() - 1));
}

void Solver_2022_12_2::solve(const QString& input)
{
    const auto map = puzzle_2022_12::ElevationMap{input};
    auto min_steps = std::numeric_limits<std::size_t>::max();
    auto min_path = puzzle_2022_12::Path{};
    for (const auto& start : map.zeroElevationPositions()) {
        auto path = map.shortestPath(start, map.goal());
        if (not path.empty() and path.size() - 1 < min_steps) {
            min_steps = path.size() - 1;
            std::swap(min_path, path);
        }
    }
    emit output(map.drawPath(min_path));
    if (min_path.empty())
        emit finished("NO SOLUTION");
    else
        emit finished(QString("%1").arg(min_steps));
}
