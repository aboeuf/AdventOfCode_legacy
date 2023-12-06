#include <2023/puzzle_2023_06.h>
#include <common.h>

namespace puzzle_2023_06 {

using Int = unsigned long long int;

struct Race
{
    Race() = default;
    Race(Int time, Int distance) : time{time}, distance{distance} {}

    Int traveledDistance(Int holding_time) const {
        return (time - holding_time) * holding_time;
    }

    bool isWinning(Int holding_time) const {
        return traveledDistance(holding_time) > distance;
    }

    Int nbWinningCommands() const
    {
        auto nb_commands = Int{0};
        for (auto command = 0u; command <= time; ++command)
            if (isWinning(command))
                ++nb_commands;
        return nb_commands;
    }

    Int time{0};
    Int distance{0};
};

class Races
{
public:
    Races(const QString& input) {
        auto lines = common::splitLines(input);
        const auto times = common::toULongLongIntValues(common::splitValues(lines.front(), ':')[1], ' ');
        const auto distances = common::toULongLongIntValues(common::splitValues(lines.back(), ':')[1], ' ');
        const auto size = std::min(times.size(), distances.size());
        m_races.reserve(size);
        for (auto i = 0; i < size; ++i)
            m_races.emplace_back(times[i], distances[i]);
    }

    QString solveOne() const {
        auto res = 1u;
        for (const auto& race : m_races)
            res *= race.nbWinningCommands();
        return QString("%1").arg(res);
    }

    QString solveTwo() const {
        auto time_str = QString{};
        auto distance_str = QString{};
        for (const auto& race : m_races) {
            time_str += QString{"%1"}.arg(race.time);
            distance_str += QString{"%1"}.arg(race.distance);
        }
        const auto time = time_str.toULongLong();
        const auto distance = distance_str.toULongLong();
        const auto race = Race{time, distance};
        const auto nb_winning_commands = race.nbWinningCommands();
        return QString("%1").arg(nb_winning_commands);
    }

private:
    std::vector<Race> m_races;
};

}

void Solver_2023_06_1::solve(const QString& input)
{
    const auto races = puzzle_2023_06::Races{input};
    emit finished(races.solveOne());
}

void Solver_2023_06_2::solve(const QString& input)
{
    const auto races = puzzle_2023_06::Races{input};
    emit finished(races.solveTwo());
}
