#include <solvers/2023/puzzle_2023_19.h>
#include <solvers/common.h>

namespace puzzle_2023_19 {

using Int = unsigned long long int;
using ushort = unsigned short int;

constexpr auto dimension = ushort{4};

inline ushort charToAxis(const QChar &c) {
  if (c == 'x')
    return ushort{0};
  if (c == 'm')
    return ushort{1};
  if (c == 'a')
    return ushort{2};
  if (c == 's')
    return ushort{3};
  common::throwInvalidArgumentError(
      QString("charToAxis: unknown axis '%1'").arg(c));
  return dimension;
}

class Interval {
public:
  Interval() = default;

  Interval(Int min, Int max) : m_min{min}, m_max{max} {
    if (m_min > m_max)
      std::swap(m_min, m_max);
  }

  Int lenght() const { return m_max - m_min + Int{1u}; }

  std::optional<Interval> clipMin(const Int value) const {
    if (value > m_max)
      return std::nullopt;
    return Interval{std::max(value, m_min), m_max};
  }

  std::optional<Interval> clipMax(const Int value) const {
    if (value < m_min)
      return std::nullopt;
    return Interval{m_min, std::min(value, m_max)};
  }

private:
  Int m_min{1};
  Int m_max{4000};
};

class Part {
public:
  Part(const QString &input) {
    auto rx = QRegExp("^\\{x=(\\d+),m=(\\d+),a=(\\d+),s=(\\d+)\\}$");
    if (not rx.exactMatch(input))
      common::throwInvalidArgumentError(
          QString("Part::constructor: cannot parse Part \"%1\"").arg(input));
    auto ok = true;
    for (auto i = 1u; i < 5u; ++i) {
      m_ratings[i - 1] = rx.cap(i).toULongLong(&ok);
      if (not ok)
        common::throwInvalidArgumentError(
            QString("Part::constructor: cannot cast \"%1\" to integer")
                .arg(rx.cap(1)));
      m_ratings_sum += m_ratings[i - 1];
    }
  }

  Int ratingsSum() const { return m_ratings_sum; }

  Int rating(const ushort axis) const {
    if (axis >= dimension)
      common::throwInvalidArgumentError(
          QString("Part::rating: out-of-range axis %1 (dimension is %2)")
              .arg(axis)
              .arg(dimension));
    return m_ratings[axis];
  }

private:
  Int m_ratings_sum{0};
  std::array<Int, dimension> m_ratings;
};

class Cuboid {
public:
  Cuboid() = default;

  Cuboid(const Cuboid &father, const ushort axis, const Interval &interval)
      : Cuboid{father} {
    if (axis >= dimension)
      common::throwInvalidArgumentError(
          "Cuboid::constructor: out-of-range axis");
    m_intervals[axis] = interval;
  }

  Int volume() const {
    auto volume = Int{1};
    for (auto i = 0u; i < 4u; ++i)
      volume *= m_intervals[i].lenght();
    return volume;
  }

  std::array<std::optional<Cuboid>, 2> split(const ushort axis,
                                             const Int value) const {
    if (axis >= dimension)
      common::throwInvalidArgumentError("Cuboid::split: out-of-range axis");
    auto cuboids =
        std::array<std::optional<Cuboid>, 2>{std::nullopt, std::nullopt};
    const auto min_clip = m_intervals[axis].clipMax(value);
    if (min_clip.has_value())
      cuboids[0] = Cuboid(*this, axis, *min_clip);
    const auto max_clip = m_intervals[axis].clipMin(value + Int{1});
    if (max_clip.has_value())
      cuboids[1] = Cuboid(*this, axis, *max_clip);
    return cuboids;
  }

private:
  std::array<Interval, dimension> m_intervals{Interval{}, Interval{},
                                              Interval{}, Interval{}};
};

class Test {
public:
  Test(const QString &input) {
    auto rx = QRegExp("^(x|m|a|s)(<|>)(\\d+):(.+)$");
    if (not rx.exactMatch(input))
      common::throwInvalidArgumentError(
          QString("Test::constructor: cannot parse Test \"%1\"").arg(input));
    m_axis = charToAxis(rx.cap(1).front());
    m_is_lesser_than_operator = (rx.cap(2) == "<");
    auto ok = true;
    m_test_value = rx.cap(3).toULongLong(&ok);
    if (not ok)
      common::throwInvalidArgumentError(
          QString("Test::constructor: cannot cast \"%1\" to integer")
              .arg(rx.cap(3)));
    m_next_workflow = rx.cap(4);
  }

  bool operator()(const Part &part) const {
    if (m_is_lesser_than_operator)
      return part.rating(m_axis) < m_test_value;
    return part.rating(m_axis) > m_test_value;
  }

  const QString &nextWorflow() const { return m_next_workflow; }

  std::array<std::optional<Cuboid>, 2> split(const Cuboid &cuboid) const {
    if (m_is_lesser_than_operator) {
      if (m_test_value == Int{0})
        return std::array<std::optional<Cuboid>, 2>{std::nullopt, cuboid};
      return cuboid.split(m_axis, m_test_value - Int{1});
    }
    auto splitted = cuboid.split(m_axis, m_test_value);
    std::swap(splitted[0], splitted[1]);
    return splitted;
  }

private:
  ushort m_axis;
  bool m_is_lesser_than_operator;
  Int m_test_value;
  QString m_next_workflow;
};

class Wokflow {
public:
  Wokflow() = default;

  Wokflow(const QString &input) {
    const auto splitted = common::splitValues(input);
    if (splitted.size() > 1) {
      m_tests.reserve(splitted.size() - 1);
      for (auto it = std::begin(splitted); std::next(it) != std::end(splitted);
           ++it)
        m_tests.emplace_back(*it);
    }
    m_default = splitted.back();
  }

  const std::vector<Test> &tests() const { return m_tests; }
  const QString &defaultWorkflow() const { return m_default; }

  QString operator()(const Part &part) const {
    for (const auto &test : m_tests)
      if (test(part))
        return test.nextWorflow();
    return m_default;
  }

private:
  std::vector<Test> m_tests{};
  QString m_default{""};
};

class WokflowsAndParts {
public:
  WokflowsAndParts(const QString &input) {
    const auto lines = common::splitLines(input);
    auto nb_workflows = 0;
    auto it = std::begin(lines);
    for (; not it->isEmpty() and it != std::end(lines); ++it) {
      auto rx = QRegExp("^(.+)\\{(.+)\\}$");
      if (not rx.exactMatch(*it))
        common::throwInvalidArgumentError(
            QString(
                "WokflowsAndParts::constructor: cannot parse Worflow \"%1\"")
                .arg(*it));
      m_workflows[rx.cap(1)] = Wokflow(rx.cap(2));
      ++nb_workflows;
    }
    ++it;
    m_parts.reserve(lines.size() - nb_workflows - 1);
    for (; it != std::end(lines); ++it)
      m_parts.emplace_back(*it);
  }

  bool partIsAccepted(const Part &part) const {
    auto workflow = QString("in");
    while (workflow != "A" and workflow != "R")
      workflow = m_workflows[workflow](part);
    return workflow == "A";
  }

  QString solveOne() const {
    auto sum = Int{0};
    for (const auto &part : m_parts)
      if (partIsAccepted(part))
        sum += part.ratingsSum();
    return QString("%1").arg(sum);
  }

  void updateVolume(const Cuboid &cuboid, const QString &workflow_label,
                    Int &volume) const {
    if (workflow_label == "R")
      return;
    if (workflow_label == "A") {
      volume += cuboid.volume();
      return;
    }
    const auto &worflow = m_workflows[workflow_label];
    auto current = cuboid;
    for (const auto &test : worflow.tests()) {
      const auto splitted = test.split(current);
      if (splitted[0].has_value())
        updateVolume(*splitted[0], test.nextWorflow(), volume);
      if (not splitted[1].has_value())
        return;
      current = *splitted[1];
    }
    updateVolume(current, worflow.defaultWorkflow(), volume);
  }

  QString solveTwo() const {
    auto volume = Int{0};
    updateVolume(Cuboid{}, "in", volume);
    return QString("%1").arg(volume);
  }

private:
  QHash<QString, Wokflow> m_workflows;
  std::vector<Part> m_parts;
};

} // namespace puzzle_2023_19

void Solver_2023_19_1::solve(const QString &input) {
  const auto workflows_end_parts = puzzle_2023_19::WokflowsAndParts(input);
  emit finished(workflows_end_parts.solveOne());
}

void Solver_2023_19_2::solve(const QString &input) {
  const auto workflows_end_parts = puzzle_2023_19::WokflowsAndParts(input);
  emit finished(workflows_end_parts.solveTwo());
}
