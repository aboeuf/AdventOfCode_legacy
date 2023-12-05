#include <2023/puzzle_2023_05.h>
#include <common.h>
#include <limits>

namespace puzzle_2023_05 {

using Int = unsigned long long int;

class Range
{
public:
    Range() = default;
    Range(Int start, Int length) : m_start{start}, m_stop{start + length} {}

    Int start() const { return m_start; }
    Int stop() const { return m_stop; }

    bool contains(Int value) const {
        return m_start <= value and value <= m_stop;
    }

private:
    Int m_start{0};
    Int m_stop{0};
};

class RangeMap
{
public:
    RangeMap() = default;

    RangeMap(const QString& input) {
        const auto splitted = common::splitValues(input, ' ');
        const auto destination_range_start = splitted[0].toULongLong();
        const auto source_range_start = splitted[1].toULongLong();
        const auto range_length = splitted[2].toULongLong();
        m_source_range = Range{source_range_start, range_length};
        m_destination_range = Range{destination_range_start, range_length};
    }

    const Range& sourceRange() const { return m_source_range; }
    const Range& destinationRange() const { return m_destination_range; }

    Int isInSourceRange(Int value) const {
        return m_source_range.contains(value);
    }

    Int isInDestinationRange(Int value) const {
        return m_destination_range.contains(value);
    }

    Int getDestinationIndex(Int source_index) const {
        if (source_index < m_source_range.start())
            throw std::runtime_error("out-of-range");
        const auto shift_from_start = source_index - m_source_range.start();
        return m_destination_range.start() + shift_from_start;
    }

private:
    Range m_source_range{};
    Range m_destination_range{};
};


class Map
{
public:
    Map() = default;

    Map(QStringList::const_iterator& line_it, const QStringList::const_iterator& end) {
        const auto source_to_dest = common::splitValues(common::splitValues(*line_it, ' ')[0], '-');
        m_source =  source_to_dest[0];
        m_destination =  source_to_dest[2];
        ++line_it;
        for (;line_it != end and not line_it->isEmpty(); ++line_it)
            m_range_maps.emplace_back(*line_it);
    }

    const QString& source() const { return m_source; }
    const QString& destination() const { return m_destination; }
    const std::vector<RangeMap>& rangeMaps() const { return m_range_maps; }

    Int getDestinationIndex(Int source_index) const {
        for (const auto& range : m_range_maps)
            if (range.isInSourceRange(source_index))
                return range.getDestinationIndex(source_index);
        return source_index;
    }

private:
    QString m_source;
    QString m_destination;
    std::vector<RangeMap> m_range_maps;
};

class Almanac
{
public:
    Almanac(const QString& input) {
        const auto lines = common::splitLines(input);
        auto line_it = std::begin(lines);
        m_seeds = common::toULongLongIntValues(line_it->mid(7), ' ');
        ++line_it;
        const auto end = std::end(lines);
        while (line_it != end) {
            for (;line_it != end and line_it->isEmpty(); ++line_it) {}
            const auto map = Map{line_it, end};
            m_maps[map.source()] = map;
        }
    }

    Int minimumLocationV1() const
    {
        auto minimum = std::numeric_limits<Int>::max();
        for (const auto seed_index : m_seeds) {
            const auto location_index = getSeedLocation(seed_index);
            minimum = std::min(minimum, location_index);
        }
        return minimum;
    }

    Int minimumLocationV2() const
    {
        const auto nb_ranges = 2u * static_cast<std::size_t>(m_seeds.size());
        auto seeds_ranges = std::vector<Range>{};
        seeds_ranges.reserve(nb_ranges);
        auto min_location = std::numeric_limits<Int>::max();
        for (auto i = 0; i + 1 < m_seeds.size(); i += 2) {
            seeds_ranges.emplace_back(m_seeds[i], m_seeds[i + 1]);
            min_location = std::min(min_location, getSeedLocation(seeds_ranges.back().start()));
            min_location = std::min(min_location, getSeedLocation(seeds_ranges.back().stop()));
        }
        const auto update_min = [&min_location, &seeds_ranges, this](Int value) {
            for (const auto& range : seeds_ranges) {
                if (range.contains(value)) {
                    min_location = std::min(min_location, getSeedLocation(value));
                    return;
                }
            }
        };
        for (const auto& range_map : m_maps["seed"].rangeMaps()) {
            update_min(range_map.sourceRange().start());
            update_min(range_map.sourceRange().stop());
        }
        return min_location;
    }

private:
    Int getNextDestinationIndex(const QString& source, Int source_index, QString& destination) const {
        if (not m_maps.contains(source))
            throw std::runtime_error("bad map call");
        destination = m_maps[source].destination();
        const auto dest_index = m_maps[source].getDestinationIndex(source_index);
        return dest_index;
    }

    Int getSeedLocation(Int index) const {
        const QString& destination = QString("location");
        auto current = QString("seed");
        while (current != destination and m_maps.contains(current)) {
            auto next = QString{};
            index = getNextDestinationIndex(current, index, next);
            std::swap(current, next);
        }
        if (current != destination)
            throw std::runtime_error("cannot reach destination");
        return index;
    }

    QVector<Int> m_seeds;
    QMap<QString, Map> m_maps;
};

}

void Solver_2023_05_1::solve(const QString& input)
{
    const auto almanac = puzzle_2023_05::Almanac{input};
    const auto minimum_location = almanac.minimumLocationV1();
    emit finished(QString("%1").arg(minimum_location));
}

void Solver_2023_05_2::solve(const QString& input)
{
    const auto almanac = puzzle_2023_05::Almanac{input};
    const auto minimum_location = almanac.minimumLocationV2();
    emit finished(QString("%1").arg(minimum_location));
}
