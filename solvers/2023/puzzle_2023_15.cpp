#include <solvers/common.h>
#include <solvers/2023/puzzle_2023_15.h>

namespace puzzle_2023_15 {

constexpr auto nb_boxes = 256u;

inline uint hash(const QString &input) {
  auto current = 0u;
  for (const auto &c : input) {
    current += static_cast<uint>(c.unicode());
    current *= 17u;
    current = (current % nb_boxes);
  }
  return current;
}

QString solveOne(const QString &input) {
  auto sum = 0u;
  const auto instructions = common::splitValues(input);
  for (const auto &instruction : instructions) {
    const auto code = hash(instruction);
    sum += code;
  }
  return QString("%1").arg(sum);
}

struct Lens {
  Lens() = default;

  Lens(const QString &input) {
    const auto splitted = common::splitValues(input, '=');
    if (splitted.size() != 2)
      common::throwInvalidArgumentError(
          QString("Lens::constructor: cannot parse \"%1\"").arg(input));
    label = splitted[0];
    auto ok = true;
    focal_length = splitted[1].toUInt(&ok);
    if (not ok)
      common::throwInvalidArgumentError(
          QString("Lens::constructor: cannot convert \"%1\" to unsigned int")
              .arg(splitted[1]));
  }

  uint focusingPower(const uint box_multipler,
                     const uint slot_multiplier) const {
    return box_multipler * slot_multiplier * focal_length;
  }

  QString label{""};
  uint focal_length{0};
};

class Box {
public:
  Box() = default;

  QList<Lens>::iterator find(const QString &label) {
    for (auto it = std::begin(m_lenses); it != std::end(m_lenses); ++it)
      if (it->label == label)
        return it;
    return std::end(m_lenses);
  }

  void remove(const QString &label) {
    auto it = find(label);
    if (it != std::end(m_lenses))
      m_lenses.erase(it);
  }

  void add(const Lens &lens) {
    auto it = find(lens.label);
    if (it == std::end(m_lenses))
      m_lenses << lens;
    else
      it->focal_length = lens.focal_length;
  }

  uint focusingPower(const uint box_multipler) const {
    auto power = 0u;
    auto slot_index = 1u;
    for (auto it = std::begin(m_lenses); it != std::end(m_lenses); ++it) {
      const auto lens_power = it->focusingPower(box_multipler, slot_index++);
      power += lens_power;
    }
    return power;
  }

private:
  QList<Lens> m_lenses{};
};

class LavaProductionFacility {
public:
  LavaProductionFacility(const QString &input) {
    const auto instructions = common::splitValues(input);
    for (const auto &instruction : instructions) {
      if (instruction.isEmpty())
        common::throwInvalidArgumentError(
            "LavaProductionFacility::constructor: empty instruction");
      if (instruction.back() == '-') {
        const auto label = instruction.mid(0, instruction.size() - 1);
        m_boxes[hash(label)].remove(label);
      } else {
        const auto lens = Lens{instruction};
        m_boxes[hash(lens.label)].add(lens);
      }
    }
  }

  uint focusingPower() const {
    auto power = 0u;
    for (auto index = 0u; index < nb_boxes; ++index) {
      const auto box_power = m_boxes[index].focusingPower(index + 1u);
      power += box_power;
    }
    return power;
  }

private:
  std::array<Box, nb_boxes> m_boxes{};
};

} // namespace puzzle_2023_15

void Solver_2023_15_1::solve(const QString &input) {
  emit finished(puzzle_2023_15::solveOne(input));
}

void Solver_2023_15_2::solve(const QString &input) {
  const auto facility = puzzle_2023_15::LavaProductionFacility{input};
  emit finished(QString("%1").arg(facility.focusingPower()));
}
