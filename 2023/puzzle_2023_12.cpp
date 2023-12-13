#include <2023/puzzle_2023_12.h>
#include <common.h>

namespace puzzle_2023_12 {

using Int = unsigned long long int;

struct Key {
    Key() = default;
    Key(uint start, uint group_index) : start{start}, group_index{group_index} {}
    uint start{0};
    uint group_index{0};
};

bool operator < (const Key& lhs, const Key& rhs) {
    if (lhs.start == rhs.start)
        return lhs.group_index < rhs.group_index;
    return lhs.start < rhs.start;
}

class Record
{
public:
    Record(const QString& input, bool v2) {
        const auto splitted = common::splitValues(input, ' ');
        m_record = splitted[0];
        m_groups = common::toUIntValues(splitted[1]);
        if (v2) {
            auto unfolded_record = QString{};
            auto unfolded_groups = QVector<uint>{};
            unfolded_groups.reserve(5 * m_groups.size());
            for (auto i = 0; i < 5; ++i) {
                unfolded_record += m_record;
                if (i < 4)
                    unfolded_record.push_back('?');
                for (auto group_size : m_groups)
                    unfolded_groups << group_size;
            }
            std::swap(m_record, unfolded_record);
            std::swap(m_groups, unfolded_groups);
        }
        m_record_size = static_cast<uint>(m_record.size());
        m_groups_size = static_cast<uint>(m_groups.size());
    }

    Int nbArrangements() {
        return nbArrangements(Key{0u, 0u});
    }

private:
    uint leftToMatch(const Key& key) const {
        auto left_to_match = 0u;
        for (auto i = key.group_index + 1; i < m_groups_size; ++i)
            left_to_match += m_groups[i];
        return left_to_match;
    }

    bool isFree(uint index) const {
        const auto& c = m_record.at(index);
        return  c == '.' or c == '?';
    }

    bool isPadded(uint index, uint group_size) const {
        return (index == 0u or isFree(index - 1u)) and (index + group_size >= m_record_size or isFree(index + group_size));
    }

    bool noneBroken(uint start, uint end) const {
        for (auto i = start; i < end; ++i)
            if (m_record.at(i) == '#')
                return false;
        return true;
    }

    uint nbBroken(uint start, uint end) const {
        auto nb_broken = 0u;
        for (auto i = start; i < end; ++i)
            if (m_record.at(i) == '#')
                ++nb_broken;
        return nb_broken;
    }

    bool isAMatch(uint index, uint group_size) {
        if (not isPadded(index, group_size))
            return false;
        for (auto i = 0u; i < group_size; ++i)
            if (m_record.at(index + i) == '.')
                return false;
        return true;
    }

    const QList<uint>& getMatches(const Key& key) {
        auto insertion_result = m_matches_map.insert(std::make_pair(key, QList<uint>{}));
        if (insertion_result.second) {
            const auto group_size = m_groups[key.group_index];
            const auto left_to_match = leftToMatch(key);
            for (auto i = key.start; i + group_size <= m_record_size; ++i) {
                if (isAMatch(i, group_size)) {
                    if (noneBroken(key.start, i)) {
                        const auto next_start = i + group_size + 1u;
                        const auto nb_broken_after = nbBroken(next_start, m_record_size);
                        if (nb_broken_after <= left_to_match)
                            insertion_result.first->second << next_start;
                    }
                }
            }
        }
        return insertion_result.first->second;
    }

    Int nbArrangements(const Key& key) {
        auto insertion_result = m_arrangements_map.insert(std::make_pair(key, Int{0}));
        if (insertion_result.second) {
            const auto next_index = key.group_index + 1u;
            if (next_index > m_groups_size) {
                insertion_result.first->second = Int{1};
            } else {
                const auto& matches = getMatches(key);
                for (auto start : matches)
                    insertion_result.first->second += nbArrangements(Key{start, next_index});
            }
        }
        return insertion_result.first->second;

    }

    QString m_record;
    QVector<uint> m_groups;
    uint m_record_size;
    uint m_groups_size;
    std::map<Key, QList<uint>> m_matches_map;
    std::map<Key, Int> m_arrangements_map;
};

class Records
{
public:
    Records(const QString& input, bool v2) {
        auto lines = common::splitLines(input);
        m_records.reserve(lines.size());
        for (const auto& line : lines)
             m_records.emplace_back(line, v2);
    }

    QString solve() {
        auto sum = Int{0};
        for (auto& record : m_records)
            sum += record.nbArrangements();
        return QString("%1").arg(sum);
    }

private:
    std::vector<Record> m_records;
};

}

void Solver_2023_12_1::solve(const QString& input)
{
    auto records = puzzle_2023_12::Records{input, false};
    emit finished(records.solve());
}

void Solver_2023_12_2::solve(const QString& input)
{
    auto records = puzzle_2023_12::Records{input, true};
    emit finished(records.solve());
}
