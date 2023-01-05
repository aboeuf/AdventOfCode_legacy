#include <2022/puzzle_2022_16.h>
#include <common.h>
#include <set>
#include <memory>

#include <iostream>

namespace puzzle_2022_16 {

struct Valve
{
    Valve() = default;
    Valve(const QString& input, std::size_t index) : index{index} {
        auto rx = QRegExp("^Valve (.+) has flow rate=(\\d+); tunnels{0,1} leads{0,1} to valves{0,1} (.+)$");
        if (not rx.exactMatch(input))
            throw std::invalid_argument(QString("invalid valve description \"%1\"").arg(input).toStdString());
        label = rx.cap(1);
        bool ok;
        flow_rate = rx.cap(2).toUInt(&ok);
        if (not ok)
            throw std::invalid_argument(QString("cannot convert \"%1\" to unsigned int")
                                        .arg(rx.cap(2)).toStdString());
        neighbors = common::splitValues(rx.cap(3));
        for (auto& neighbor : neighbors)
            neighbor.remove(' ');
    }

    QString toString() const
    {
        auto res = QString("[%1](%2):").arg(label).arg(flow_rate);
        for (const auto& neighbor : neighbors)
            res += QString(" [%1]").arg(neighbor);
        return res;
    }

    std::size_t index{0};
    QString label{""};
    uint flow_rate{0};
    QStringList neighbors{};
};

using OtherAgent = std::optional<std::size_t>;

struct State
{
    State(const Valve& valve, uint remaining_time) :
        valve{valve.index},
        remaining_time{remaining_time} {}

    State(const std::vector<Valve>& valves,
          std::size_t valve,
          uint transition_duration,
          std::shared_ptr<State> previous) :
        valve{valve},
        remaining_time{previous->remaining_time},
        other_agent{previous->other_agent.has_value() ? OtherAgent{previous->valve} : OtherAgent{std::nullopt}},
        previous{previous},
        flow_rate{previous->flow_rate + valves[valve].flow_rate},
        released_presure{previous->released_presure + transition_duration * previous->flow_rate},
        closed_valves{previous->closed_valves}
    {
        remaining_time -= transition_duration;
        closed_valves.erase(valve);
        auto heuristic_flow_rate = flow_rate;
        for (auto index : closed_valves)
            heuristic_flow_rate += valves[index].flow_rate;
        cost = released_presure + remaining_time * heuristic_flow_rate;
    }

    std::size_t valve{std::numeric_limits<std::size_t>::max()};
    uint remaining_time{0};
    OtherAgent other_agent{std::nullopt};
    uint transition_duration{0};
    std::shared_ptr<State> previous{nullptr};
    uint flow_rate{0};
    uint released_presure{0};
    std::set<std::size_t> closed_valves{};
    uint cost{0};
};

bool operator < (const std::shared_ptr<State>& lhs, const std::shared_ptr<State>& rhs)
{
    return lhs->cost > rhs->cost;
}

using Plan = std::list<std::shared_ptr<State>>;

struct OpenSet : public std::multiset<std::shared_ptr<State>>
{
    std::shared_ptr<State> pop() {
        const auto it = this->begin();
        if (it == this->end())
            return nullptr;
        const auto state = *it;
        this->erase(it);
        return state;
    }

    void push(std::shared_ptr<State> state)
    {
        this->insert(state);
    }
};

class Volcano
{
public:
    Volcano(const QString& input) {
        const auto lines = common::splitLines(input);
        m_valves.reserve(lines.size());
        for (const auto& line : lines) {
            m_valves.emplace_back(line, m_valves.size());
            m_label_to_index[m_valves.back().label] = m_valves.back().index;
        }
        m_transitive_closure.resize(m_valves.size());
        for (auto& row : m_transitive_closure)
            row.resize(m_valves.size(), std::numeric_limits<uint>::max());
        for (auto i = 0u; i < m_valves.size(); ++i) {
            m_transitive_closure[i][i] = 0u;
            auto open_set = std::set<std::size_t>{};
            const auto pop = [i, &open_set, this]() {
                const auto it = std::min_element(open_set.begin(), open_set.end(),
                                                 [i, this](const auto lhs, const auto rhs) {
                    return m_transitive_closure[i][lhs] < m_transitive_closure[i][rhs];
                });
                if (it == open_set.end())
                    return std::numeric_limits<std::size_t>::max();
                const auto index = *it;
                open_set.erase(it);
                return index;
            };
            for (auto j = 0u; j < m_valves.size(); ++j)
                open_set.insert(j);
            while (not open_set.empty()) {
                const auto current = pop();
                for (const auto& label : m_valves[current].neighbors) {
                    const auto neighbor = m_label_to_index[label];
                    const auto alternative_distance = m_transitive_closure[i][current] + 1u;
                    if (alternative_distance < m_transitive_closure[i][neighbor])
                        m_transitive_closure[i][neighbor] = alternative_distance;
                }
            }
            for (auto j = i + 1u; j < m_valves.size(); ++j)
                m_transitive_closure[j][i] = m_transitive_closure[i][j];
        }
    }

    const std::vector<Valve>& valves() const { return m_valves; }

    Plan computeOptimalPlan(const QString& source, uint nb_minutes, bool with_help = false) const
    {
        auto current = std::make_shared<State>(m_valves[m_label_to_index[source]], nb_minutes);
        for (const auto& valve : m_valves)
            if (valve.flow_rate > 0u)
                current->closed_valves.insert(valve.index);
        if (with_help)
            current->other_agent = current->valve;
        auto open_set = OpenSet{};
        open_set.push(current);
        while (not open_set.empty()) {
            current = open_set.pop();
            if (current->remaining_time == 0 or current->closed_valves.empty()) {
                auto plan = Plan{};
                do {
                    plan.push_front(current);
                    current = current->previous;
                } while (current != nullptr);
                return plan;
            }
            const auto previous_index = with_help and current->previous != nullptr ? current->previous->valve : current->valve;
            auto remaining_time = current->remaining_time;
            if (with_help)
                remaining_time += current->transition_duration;
            for (auto index : current->closed_valves) {
                

                const auto transition_duration = m_transitive_closure[current->valve][index];
                if (transition_duration <= current->remaining_time) {

                    open_set.push(std::make_shared<State>(m_valves,
                                                          index,
                                                          transition_duration + 1u,
                                                          current));
                }
            }
        }
        return Plan{};
    }

    QString toString() const
    {
        auto res = QString();
        for (const auto& valve : m_valves)
            res += valve.toString() + "\n";
        auto line = QString("");
        for (auto i = 0u; i <= m_valves.size(); ++i)
            line += "|----";
        line += "|\n";
        res += "\n" + line + "|    ";
        for (auto i = 0u; i < m_valves.size(); ++i)
            res += QString("| %1 ").arg(m_valves[i].label);
        res += "|\n" + line;
        for (auto i = 0u; i < m_valves.size(); ++i) {
            res += QString("| %1 ").arg(m_valves[i].label);
            for (auto j = 0u; j < m_valves.size(); ++j) {
                const auto dist = m_transitive_closure[i][j];
                if (dist < m_valves.size())
                    res += QString("| %1%2 ").arg(dist < 10 ? " " : "").arg(dist);
                else
                    res += "|  X ";
            }
            res += "|\n" + line;
        }
        return res;
    }

    QString stateToString(const std::shared_ptr<State>& state) const
    {
        auto res = QString("valve: %1, remaining time: %2, flow rate: %3, released presure: %4, cost: %5, closed valves: ")
                .arg(m_valves[state->valve].label)
                .arg(state->remaining_time)
                .arg(state->flow_rate)
                .arg(state->released_presure)
                .arg(state->cost);
        for (auto index : state->closed_valves)
            res += QString(" %1").arg(m_valves[index].label);
        return res;
    }

    QString planToString(const Plan& plan) const
    {
        auto res = QString("Plan: nb states = %1\n").arg(std::size(plan));
        for (const auto& state : plan) {
            res += stateToString(state);
            res.push_back('\n');
        }
        return res;
    }

    QString openSetToString(const OpenSet& open_set) const
    {
        auto res = QString("Open set:\n");
        for (const auto& state : open_set)
            res += "  " + stateToString(state) + "\n";
        return res;
    }

private:
    std::vector<Valve> m_valves{};
    QMap<QString, std::size_t> m_label_to_index{};
    std::size_t first_valve{std::numeric_limits<std::size_t>::max()};
    std::vector<std::vector<uint>> m_transitive_closure{};
};

}

void Solver_2022_16_1::solve(const QString& input)
{
    const auto volcano = puzzle_2022_16::Volcano(input);
    const auto plan = volcano.computeOptimalPlan("AA", 30);
    //    emit output(volcano.planToString(plan));
    //    emit output(volcano.toString());
    emit finished(QString("%1").arg(plan.back()->cost));
}

void Solver_2022_16_2::solve(const QString& input)
{
    emit output(input);
    emit finished("Default");
}
