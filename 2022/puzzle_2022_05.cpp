#include <2022/puzzle_2022_05.h>
#include <common.h>
#include <QStack>

namespace puzzle_2022_05 {

using Int = std::size_t;

class RearrangementStep
{
public:
    RearrangementStep(const QString& input) {
        auto rx = QRegExp{"^move (\\d+) from (\\d+) to (\\d+)$"};
        m_valid = rx.exactMatch(input);
        if (m_valid) {
            m_nb_crates = static_cast<Int>(rx.cap(1).toUInt());
            m_from = static_cast<Int>(rx.cap(2).toUInt()) - 1;
            m_to = static_cast<Int>(rx.cap(3).toUInt()) - 1;
        }
    }

    bool valid() const { return m_valid; }
    Int nbCrates() const { return m_nb_crates; }
    Int from() const { return m_from; }
    Int to() const { return m_to; }

private:
    bool m_valid{false};
    Int m_nb_crates{0};
    Int m_from{0};
    Int m_to{0};
};

class Stacks
{
public:
    Stacks(const QString& input, bool version_9001)
    {
        const auto lines = common::splitLines(input);
        auto procedure = std::vector<RearrangementStep>{};
        procedure.reserve(lines.size());
        auto nb_stacks = 0;
        auto crates = QStringList{};
        for (const auto& line : lines) {
            procedure.emplace_back(line);
            if (not procedure.back().valid()) {
                procedure.pop_back();
                auto crate_line = QString{};
                for (auto i = 0; 4 * i + 1 < line.size(); ++i)
                    crate_line.push_back(line[4 * i + 1]);
                if (not crate_line.isEmpty()) {
                    crates << crate_line;
                    nb_stacks = std::max(crate_line.size(), nb_stacks);
                }
            }
        }
        m_stacks.resize(nb_stacks);
        while (not crates.empty()) {
            auto& line = crates.back();
            for (auto i = 0; i < line.size(); ++i)
                if (line[i].isLetter())
                    m_stacks[i].push(line[i]);
            crates.pop_back();
        }
        for (const auto& step : procedure)
            applyStep(step, version_9001);
    }

    QString top() const
    {
        auto res = QString{};
        for (const auto& stack : m_stacks)
            if (not stack.empty())
                res.push_back(stack.top());
        return res;
    }

private:
    void applyStep(const RearrangementStep& step, bool version_9001)
    {
        if (version_9001) {
            auto stack = QStack<QChar>{};
            for (auto i = 0u; i < step.nbCrates(); ++i) {
                stack.push(m_stacks[step.from()].top());
                m_stacks[step.from()].pop();
            }
            while (not stack.empty()) {
                m_stacks[step.to()].push(stack.top());
                stack.pop();
            }
        } else {
            for (auto i = 0u; i < step.nbCrates(); ++i) {
                m_stacks[step.to()].push(m_stacks[step.from()].top());
                m_stacks[step.from()].pop();
            }
        }
    }

    std::vector<QStack<QChar>> m_stacks;
};

}

void Solver_2022_05_1::solve(const QString& input)
{
    const auto stacks = puzzle_2022_05::Stacks{input, false};
    emit finished(stacks.top());
}

void Solver_2022_05_2::solve(const QString& input)
{
    const auto stacks = puzzle_2022_05::Stacks{input, true};
    emit finished(stacks.top());
}
