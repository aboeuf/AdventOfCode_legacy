#include <limits>
#include <solvers/2023/puzzle_2023_05.h>
#include <solvers/common.h>

namespace puzzle_2023_05 {

using Int = unsigned long long int;

class Range {
public:
  Range() = default;
  Range(const Int start, const Int stop) : m_start{start}, m_stop{stop} {
    if (stop < start)
      throw std::invalid_argument("bad range");
  }

  Int start() const { return m_start; }
  Int stop() const { return m_stop; }

  QString toString() const {
    return QString("[%1, %2]").arg(m_start).arg(m_stop);
  }

  bool contains(const Int value) const {
    return m_start <= value and value <= m_stop;
  }

  void setStart(const Int value) { m_start = value; }

  void setStop(const Int value) { m_stop = value; }

private:
  Int m_start{0};
  Int m_stop{std::numeric_limits<Int>::max()};
};

class RangeMap {
public:
  RangeMap() = default;

  RangeMap(const QString &input) {
    const auto splitted = common::splitValues(input, ' ');
    const auto destination_range_start = splitted[0].toULongLong();
    const auto source_range_start = splitted[1].toULongLong();
    const auto range_length = splitted[2].toULongLong();
    m_source_range =
        Range{source_range_start, source_range_start + range_length - 1u};
    m_destination_range = Range{destination_range_start,
                                destination_range_start + range_length - 1u};
  }

  const Range &sourceRange() const { return m_source_range; }
  const Range &destinationRange() const { return m_destination_range; }

  Int isInSourceRange(const Int value) const {
    return m_source_range.contains(value);
  }

  Int isInDestinationRange(const Int value) const {
    return m_destination_range.contains(value);
  }

  Int getDestinationIndex(const Int source_index) const {
    if (source_index < m_source_range.start())
      throw std::runtime_error("out-of-range");
    const auto shift_from_start = source_index - m_source_range.start();
    return m_destination_range.start() + shift_from_start;
  }

private:
  Range m_source_range{};
  Range m_destination_range{};
};

class Map {
public:
  Map() = default;

  Map(QStringList::const_iterator &line_it,
      const QStringList::const_iterator &end) {
    const auto source_to_dest =
        common::splitValues(common::splitValues(*line_it, ' ')[0], '-');
    m_source = source_to_dest[0];
    m_destination = source_to_dest[2];
    ++line_it;
    for (; line_it != end and not line_it->isEmpty(); ++line_it)
      m_range_maps.push_back(RangeMap{*line_it});
  }

  const QString &source() const { return m_source; }
  const QString &destination() const { return m_destination; }
  const QVector<RangeMap> &rangeMaps() const { return m_range_maps; }

  Int getDestinationIndex(const Int source_index) const {
    for (const auto &range : m_range_maps)
      if (range.isInSourceRange(source_index))
        return range.getDestinationIndex(source_index);
    return source_index;
  }

private:
  QString m_source;
  QString m_destination;
  QVector<RangeMap> m_range_maps;
};

class SortedRanges {
public:
  SortedRanges() = default;

  SortedRanges(const QVector<Int> &seeds) {
    for (auto i = 0; i + 1 < seeds.size(); i += 2)
      mergeInsert(Range{seeds[i], seeds[i] + seeds[i + 1] - 1u});
  }

  const QList<Range> &ranges() const { return m_ranges; }

  QString toString() const {
    auto str = QString{};
    for (const auto &range : m_ranges)
      str += range.toString() + " ";
    return str;
  }

  void mergeInsert(const Range &range) {
    if (m_ranges.empty() or range.stop() < m_ranges.front().start()) {
      m_ranges.push_front(range);
      return;
    }
    if (range.start() > m_ranges.back().stop()) {
      m_ranges.push_back(range);
      return;
    }

    auto start_it = find(range.start());
    if (start_it == m_ranges.end()) {
      for (auto it = m_ranges.begin(); it != m_ranges.end(); ++it) {
        if (it->stop() < range.start())
          start_it = std::next(it);
      }
      if (start_it == m_ranges.end())
        throw std::runtime_error("mergeInsert: cannot find start iterator");
      start_it->setStart(range.start());
    }

    auto stop_it = find(range.stop());
    if (stop_it == m_ranges.end()) {
      for (auto it = m_ranges.begin();
           stop_it != m_ranges.end() and it != m_ranges.end(); ++it) {
        if (stop_it == m_ranges.begin() and range.stop() < it->start())
          stop_it = std::prev(it);
      }
      if (stop_it == m_ranges.end())
        throw std::runtime_error("mergeInsert: cannot find stop iterator");
      stop_it->setStop(range.stop());
    }

    start_it->setStop(stop_it->stop());
    ++start_it;
    ++stop_it;
    m_ranges.erase(start_it, stop_it);
  }

  void splitInsert(const Range &range) {
    if (m_ranges.empty())
      throw std::runtime_error("splitInsert empty");

    const auto split_insert = [this, &range](Int value, bool start) {
      auto it = find(value);
      if (it != m_ranges.end()) {
        auto new_range = Range{};
        new_range.setStart(it->start());
        if (start) {
          if (value == 0u or value == it->start())
            return;
          new_range.setStop(value - 1u);
          it->setStart(value);
        } else {
          if (value == it->stop())
            return;
          new_range.setStop(value);
          it->setStart(value + 1u);
        }
        if (new_range.start() <= new_range.stop()) {
          it = m_ranges.insert(it, new_range);
          ++it;
        }
        if (it->start() > it->stop())
          m_ranges.erase(it);
      }
    };

    split_insert(range.start(), true);
    split_insert(range.stop(), false);
  }

private:
  QList<Range>::iterator find(const Int value) {
    for (auto it = m_ranges.begin(); it != m_ranges.end(); ++it)
      if (it->contains(value))
        return it;
    return m_ranges.end();
  }

  QList<Range> m_ranges;
};

class Almanac {
public:
  Almanac(const QString &input) {
    const auto lines = common::splitLines(input);
    auto line_it = std::begin(lines);
    m_seeds = common::toVecULongLong(line_it->mid(7), ' ');
    ++line_it;
    const auto end = std::end(lines);
    while (line_it != end) {
      for (; line_it != end and line_it->isEmpty(); ++line_it) {
      }
      const auto map = Map{line_it, end};
      m_maps[map.source()] = map;
    }
  }

  Int minimumLocationV1() const {
    auto minimum = std::numeric_limits<Int>::max();
    for (const auto seed_index : m_seeds) {
      const auto location_index = getSeedLocation(seed_index);
      minimum = std::min(minimum, location_index);
    }
    return minimum;
  }

  Int minimumLocationV2() const {
    const auto destination_map = QString("location");
    auto current_map = QString("seed");
    auto current_ranges = SortedRanges{m_seeds};
    qDebug() << "merged \"seed\"" << current_ranges.toString();
    while (current_map != destination_map and m_maps.contains(current_map))
      getNextRanges(current_map, current_ranges);
    return 0u;
  }

private:
  Int getNextDestinationIndex(const QString &source, const Int source_index,
                              QString &destination) const {
    if (not m_maps.contains(source))
      throw std::runtime_error("bad map call");
    destination = m_maps[source].destination();
    const auto dest_index = m_maps[source].getDestinationIndex(source_index);
    return dest_index;
  }

  Int getSeedLocation(const Int index) const {
    const auto destination_map = QString("location");
    auto current_map = QString("seed");
    auto next_map = QString{};
    auto current_index = index;
    while (current_map != destination_map and m_maps.contains(current_map)) {
      current_index =
          getNextDestinationIndex(current_map, current_index, next_map);
      std::swap(current_map, next_map);
    }
    if (current_map != destination_map)
      throw std::runtime_error("cannot reach destination");
    return current_index;
  }

  void getNextRanges(QString &current_map, SortedRanges &current_ranges) const {
    if (not m_maps.contains(current_map))
      throw std::runtime_error("bad map call");
    for (const auto &range_map : m_maps[current_map].rangeMaps())
      current_ranges.splitInsert(range_map.sourceRange());
    qDebug() << "splitted" << current_map << current_ranges.toString();
    auto next_map = m_maps[current_map].destination();
    auto next_ranges = SortedRanges{};
    for (const auto &range : current_ranges.ranges()) {
      const auto mapped_start =
          m_maps[current_map].getDestinationIndex(range.start());
      const auto mapped_stop =
          m_maps[current_map].getDestinationIndex(range.stop());
      const auto mapped_range = Range{mapped_start, mapped_stop};
      next_ranges.mergeInsert(mapped_range);
    }
    qDebug() << "merged" << next_map << next_ranges.toString();
    std::swap(current_map, next_map);
    std::swap(current_ranges, next_ranges);
  }

  QVector<Int> m_seeds;
  QMap<QString, Map> m_maps;
};

} // namespace puzzle_2023_05

void Solver_2023_05_1::solve(const QString &input) {
  const auto almanac = puzzle_2023_05::Almanac{input};
  const auto minimum_location = almanac.minimumLocationV1();
  emit finished(QString("%1").arg(minimum_location));
}

void Solver_2023_05_2::solve(const QString &input) {
  const auto almanac = puzzle_2023_05::Almanac{input};
  const auto minimum_location = almanac.minimumLocationV2();
  emit finished(QString("%1").arg(minimum_location));
}
