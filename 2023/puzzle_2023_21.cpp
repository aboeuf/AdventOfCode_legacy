#include <2023/puzzle_2023_21.h>
#include <common.h>
#include <limits>

#include <QFile>
#include <QTextStream>


namespace puzzle_2023_21 {

using Int = unsigned long long int;

enum class CellType {
    plot,
    rock
};

enum class Direction
{
    north,
    west,
    south,
    east
};

const auto directions = std::array<Direction, 4>{
    Direction::north, Direction::west, Direction::south, Direction::east
};

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

Int qHash(const Coordinates& c) {
    return qHash(QString("%1,%2").arg(c.x).arg(c.y));
}

class Grid
{
public:
    Grid(const QString& input) {
        const auto lines = common::splitLines(input);
        m_dimension = lines.size();
        if (m_dimension == 0)
            common::throwInvalidArgumentError("Grid::constructor: empty grid");
        m_cells.reserve(m_dimension);
        auto start_set = false;
        auto x = 0;
        for (const auto& line : lines) {
            if (line.size() != m_dimension)
                common::throwInvalidArgumentError("Grid::constructor: incoherent width");
            m_cells.emplace_back();
            m_cells.back().reserve(m_dimension);
            auto y = 0;
            for (const auto& c : line) {
                if (c == '.') {
                    m_cells.back().emplace_back(CellType::plot);
                } else if (c == '#') {
                    m_cells.back().emplace_back(CellType::rock);
                } else if (c == 'S') {
                    if (start_set)
                        common::throwInvalidArgumentError("Grid::constructor: more than one starting point is defined");
                    m_cells.back().emplace_back(CellType::plot);
                    m_start = Coordinates{x, y};
                    start_set = true;
                } else {
                    common::throwInvalidArgumentError(QString("Grid::constructor: unknown cell type '%1'").arg(c));
                }
                ++y;
            }
            ++x;
        }
        if (not start_set)
            common::throwInvalidArgumentError("Grid::constructor: no starting point has been defined");
    }

    Coordinates toLocal(const Coordinates& c) const {
        const auto mod = [](const int value, const int modulo) {
            if (value < 0) {
                const auto remainder = abs(value) % modulo;
                return remainder == 0 ? 0 : modulo - remainder;
            }
            return value % modulo;
        };
        return Coordinates(mod(c.x, m_dimension), mod(c.y, m_dimension));
    }

    CellType getInfinite(const Coordinates& c) const {
        const auto local = toLocal(c);
        return m_cells[local.x][local.y];
    }

    bool isPlotCell(const Coordinates& c, bool v1) const {
        if (v1)
            return c.x > -1 and c.x < m_dimension and c.y > -1 and c.y < m_dimension and m_cells[c.x][c.y] == CellType::plot;
        return getInfinite(c) == CellType::plot;
    }

    QHash<Coordinates, Int> computeCostMap(const Int N) const {
        const auto dim = static_cast<std::size_t>(m_dimension);
        const auto nb_step_max = N  * dim;
        auto cost_map = QHash<Coordinates, Int>{};
        cost_map[m_start] = Int{0};
        auto open_set = common::OpenSet<Coordinates, Int>{};
        open_set.push(m_start, 0);
        auto current = open_set.pop();
        while (current.has_value()) {
            const auto& father = current->first;
            if (not cost_map.contains(father))
                common::throwRunTimeError("cannot find father cost");
            const auto father_cost = cost_map[father];
            if (father_cost < nb_step_max) {
                const auto child_cost = father_cost + Int{1};
                for (const auto direction : directions) {
                    const auto child = Coordinates(father, direction);
                    if (isPlotCell(child, false)) {
                        if (not cost_map.contains(child) or child_cost < cost_map[child]) {
                            cost_map[child] = child_cost;
                            open_set.push(child, child_cost);
                        }
                    }
                }
            }
            current = open_set.pop();
        }
        return cost_map;
    }

    std::vector<Int> getFirstOrder(const Int N) const
    {
        const auto dim = static_cast<std::size_t>(m_dimension);
        auto nb_cells = std::vector<Int>{};
        nb_cells.resize(N * dim);
        const auto cost_map = computeCostMap(N);
        for (auto it = std::begin(cost_map); it != std::end(cost_map); ++it)
            if (it.value() < N * dim)
                ++nb_cells[it.value()];
        return nb_cells;
    }

    std::vector<Int> getSecondOrder(const Int N) const {
        const auto dim = static_cast<std::size_t>(m_dimension);
        const auto first_order = getFirstOrder(N);
        auto second_order = std::vector<Int>{};
        second_order.reserve(first_order.size() - dim);
        for (auto i = 4u * dim + 1u; i < first_order.size(); ++i) {
            second_order.emplace_back(first_order[i] - first_order[i - dim]);
        }
        return second_order;
    }

    std::vector<Int> getThirdOrder(const Int N) const {
        const auto dim = static_cast<std::size_t>(m_dimension);
        const auto second_order = getSecondOrder(N);
        auto third_order = std::vector<Int>{};
        third_order.reserve(second_order.size() - dim);
        for (auto i = dim; i < second_order.size(); ++i) {
            third_order.emplace_back(second_order[i] - second_order[i - dim]);
        }
        return third_order;
    }

    std::vector<Int> getThirdOrderTest(const Int N) const {
        const auto dim = static_cast<std::size_t>(m_dimension);
        const auto first_order = getFirstOrder(N);
        auto third_order = std::vector<Int>{};
        third_order.reserve((N - 3u) * dim + 1u);
        for (auto i = 3u * dim; i < N * dim; ++i)
            third_order.emplace_back(first_order[i] + first_order[ i - 2u * dim] - 2u * first_order[ i - dim]);
        return third_order;
    }

    Int getNbCellsTemp(const Int nb_step_max) const {
        auto cost_map = QHash<Coordinates, Int>{};
        cost_map[m_start] = Int{0};
        auto open_set = common::OpenSet<Coordinates, Int>{};
        open_set.push(m_start, 0);
        auto current = open_set.pop();
        while (current.has_value()) {
            const auto& father = current->first;
            if (not cost_map.contains(father))
                common::throwRunTimeError("cannot find father cost");
            const auto father_cost = cost_map[father];
            if (father_cost < nb_step_max) {
                const auto child_cost = father_cost + Int{1};
                for (const auto direction : directions) {
                    const auto child = Coordinates(father, direction);
                    if (isPlotCell(child, false)) {
                        if (not cost_map.contains(child) or child_cost < cost_map[child]) {
                            cost_map[child] = child_cost;
                            open_set.push(child, child_cost);
                        }
                    }
                }
            }
            current = open_set.pop();
        }
        const auto target_parity = (nb_step_max % 2u == 0u);
        auto nb_cells = Int{0};
        for (auto it = std::begin(cost_map); it != std::end(cost_map); ++it) {
            const auto parity = (it.value() % 2u == 0u);
            if (parity == target_parity)
                ++nb_cells;
        }
        return nb_cells;
    }

    void test1(const int n, const int m) const {
        const auto L = (2 * n + 1) * m_dimension;
        const auto W = (2 * m + 1) * m_dimension;
        const auto small_to_big = [n, m, this](const Coordinates& c) {
            return Coordinates{c.x + n * m_dimension, c.y + m * m_dimension};
        };
        const auto big_to_small = [n, m, this](const Coordinates& c) {
            return Coordinates{c.x - n * m_dimension, c.y - m * m_dimension};
        };
        const auto valid = [L, W, &small_to_big](const Coordinates& c) {
            const auto converted = small_to_big(c);
            return converted.x > -1 and converted.x < L and converted.y > -1 and converted.y < W;
        };
        auto cost_max = Int{0};
        auto cost_map = std::vector<std::vector<Int>>{};
        cost_map.resize(L);
        for(auto i = 0; i < L; ++i)
            cost_map[i].resize(W, std::numeric_limits<Int>::max());
        const auto cost = [&cost_map, &small_to_big, this](const Coordinates& c) -> Int& {
            const auto converted = small_to_big(c);
            return cost_map[converted.x][converted.y];
        };
        cost(m_start) = Int{0};
        auto open_set = common::OpenSet<Coordinates, Int>{};
        open_set.push(m_start, 0);
        auto current = open_set.pop();
        while (current.has_value()) {
            const auto& father = current->first;
            const auto child_cost = cost(father) + Int{1};
            for (const auto direction : directions) {
                const auto child = Coordinates(father, direction);
                if (valid(child) and isPlotCell(child, false)) {
                    if (child_cost < cost(child)) {
                        cost(child) = child_cost;
                        open_set.push(child, child_cost);
                        cost_max = std::max(cost_max, child_cost);
                    }
                }
            }
            current = open_set.pop();
        }

        QFile file("/home/aboeuf/color_map.csv");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            for (auto i = 0; i < L; ++i) {
                for (auto j = 0; j < W; ++j) {
                    const auto c = Coordinates(i, j);
                    const auto converted = big_to_small(c);
                    if (isPlotCell(converted, false))
                        out << i << "," << j << "," << cost_map[i][j] << "\n";
                }
            }
            file.close();
        }
    }

private:
    std::vector<std::vector<CellType>> m_cells;
    int m_dimension;
    Coordinates m_start;
};

}

void Solver_2023_21_1::solve(const QString& input)
{
    const auto grid = puzzle_2023_21::Grid{input};
    grid.test1(5, 5);
    emit finished("done test1");
}

inline void exportVector(const QString& filepath, const std::vector<puzzle_2023_21::Int>& data) {
    QFile file(filepath);
    if (not file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    out << "nb_steps,data\n";
    for (auto i = 0u; i < data.size(); ++i)
        out << i << "," << data[i] << "\n";
    file.close();
}

void Solver_2023_21_2::solve(const QString& input)
{
    const auto grid = puzzle_2023_21::Grid{input};
    const auto N = 20u;
    exportVector("/home/aboeuf/shared/test_aoc/test_aoc_first_order.csv", grid.getFirstOrder(N));
    exportVector("/home/aboeuf/shared/test_aoc/test_aoc_second_order.csv", grid.getSecondOrder(N));
    exportVector("/home/aboeuf/shared/test_aoc/test_aoc_third_order.csv", grid.getThirdOrder(N));
    exportVector("/home/aboeuf/shared/test_aoc/test_aoc_third_order_test.csv", grid.getThirdOrderTest(N));
    emit finished("done test2");
}
