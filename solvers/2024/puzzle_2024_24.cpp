#include <QFile>
#include <solvers/common.h>
#include <ranges>
#include <solvers/2024/puzzle_2024_24.h>

#include <iostream>

namespace puzzle_2024_24 {

inline QString makeLabel(const QChar &c, uint i) {
  return QString("%1%2").arg(c).arg(i, 2, 10, QChar('0'));
}

inline GateType stringToType(const QString &str) {
  if (str == "AND") {
    return GateType::AND;
  }
  if (str == "OR") {
    return GateType::OR;
  }
  if (str == "XOR") {
    return GateType::XOR;
  }
  common::throwInvalidArgumentError(
      QString("puzzle_2024_24::stringToType: invalid gate type \"%1\"")
          .arg(str));
  return GateType::AND;
}

struct Variable {
  Variable(const QChar &name, const uint index) : name{name}, index{index} {}
  QChar name;
  uint index;
};

inline std::optional<Variable> getVariable(const QString &wire_label) {
  if (wire_label.size() > 1) {
    if (std::any_of(std::next(std::begin(wire_label)), std::end(wire_label),
                    [](const QChar &c) { return not c.isDigit(); })) {
      return std::nullopt;
    }
    auto ok = true;
    const auto index = wire_label.midRef(1).toUInt(&ok);
    if (ok) {
      return Variable(wire_label.front(), index);
    }
  }
  return std::nullopt;
}

Gate::Gate(const QString &input) {
  auto rx = QRegExp("^(.+) (AND|OR|XOR) (.+) -> (.+)$");
  if (not rx.exactMatch(input)) {
    common::throwInvalidArgumentError(
        QString("puzzle_2024_24::Gate: cannot parse input string \"%1\"")
            .arg(input));
  }
  m_type = stringToType(rx.cap(2));
  m_first_input = rx.cap(1);
  m_second_input = rx.cap(3);
  m_output = rx.cap(4);
}

GateType Gate::type() const { return m_type; }

const QString &Gate::firstInput() const { return m_first_input; }

const QString &Gate::secondInput() const { return m_second_input; }

const QString &Gate::output() const { return m_output; }

void Gate::setOutput(const QString &output) { m_output = output; }

Circuit::Circuit(const QString &input, bool part_one) {
  auto nb_x_wires = 0u;
  auto nb_y_wires = 0u;
  auto nb_z_wires = 0u;
  const auto add_wire = [this, &nb_x_wires, &nb_y_wires,
                         &nb_z_wires](const QString &label,
                                      const Bit &value = std::nullopt) {
    if (not m_wires.contains(label)) {
      const auto variable = getVariable(label);
      if (variable.has_value()) {
        if (variable->name == 'x') {
          ++nb_x_wires;
        } else if (variable->name == 'y') {
          ++nb_y_wires;
        } else if (variable->name == 'z') {
          ++nb_z_wires;
        } else {
          common::throwInvalidArgumentError(
              QString("puzzle_2024_24::Circuit: invalid wire label \"%1\"")
                  .arg(label));
        }
      }
      m_wires[label].value = value;
    }
  };

  const auto lines = common::splitLines(input, true);
  m_gates.reserve(lines.size());
  for (const auto &line : lines) {
    auto rx = QRegExp("^(.+): (0|1)$");
    if (rx.exactMatch(line)) {
      add_wire(rx.cap(1), rx.cap(2) == "1");
    } else {
      const auto gate_index = std::size(m_gates);
      m_gates.emplace_back(line);
      add_wire(m_gates.back().firstInput());
      add_wire(m_gates.back().secondInput());
      add_wire(m_gates.back().output());
      m_wires[m_gates.back().firstInput()].next_gates.insert(gate_index);
      m_wires[m_gates.back().secondInput()].next_gates.insert(gate_index);
      m_wires[m_gates.back().output()].previous_gate = gate_index;
    }
  }

  if (nb_x_wires != nb_y_wires) {
    common::throwInvalidArgumentError(
        QString("puzzle_2024_24::Circuit: number of input bits for x (%1) is "
                "different than the number of input bits for y (%2)")
            .arg(nb_x_wires)
            .arg(nb_y_wires));
  }
  m_size = nb_x_wires;

  if (m_size < 1) {
    common::throwInvalidArgumentError(
        "puzzle_2024_24::Circuit: did not find any adder");
  }

  if (nb_z_wires != m_size + 1) {
    common::throwInvalidArgumentError(
        QString("puzzle_2024_24::Circuit: number of output bits (%1) is "
                "not coherent with the number of input bits (%2)")
            .arg(nb_z_wires)
            .arg(m_size));
  }

  if (part_one) {
    run();
  } else {
    fix();
  }
}

const QString &Circuit::result() const { return m_result; }

const Wire &Circuit::getWire(const QString &label) const {
  auto it = m_wires.constFind(label);
  if (it == m_wires.cend()) {
    common::throwInvalidArgumentError(
        QString("puzzle_2024_24::cannot find wire with label \"%1\"")
            .arg(label));
  }
  return it.value();
}

bool Circuit::isReady(const std::size_t gate_index) const {
  const auto &gate = m_gates[gate_index];
  const auto &first_input_wire = getWire(gate.firstInput());
  const auto &second_input_wire = getWire(gate.secondInput());
  return first_input_wire.value.has_value() and
         second_input_wire.value.has_value();
}

void Circuit::run() {
  auto ready = QList<std::size_t>();
  for (auto i = 0u; i < std::size(m_gates); ++i) {
    if (isReady(i)) {
      ready << i;
    }
  }
  while (not ready.isEmpty()) {
    const auto &gate = m_gates[ready.front()];
    const auto &next_gates = m_wires[gate.output()].next_gates;
    ready.pop_front();
    if (gate.type() == GateType::AND) {
      m_wires[gate.output()].value = *m_wires[gate.firstInput()].value and
                                     *m_wires[gate.secondInput()].value;
    } else if (gate.type() == GateType::OR) {
      m_wires[gate.output()].value = *m_wires[gate.firstInput()].value or
                                     *m_wires[gate.secondInput()].value;
    } else {
      m_wires[gate.output()].value = *m_wires[gate.firstInput()].value xor
                                     *m_wires[gate.secondInput()].value;
    }
    for (const auto next : next_gates) {
      if (isReady(next)) {
        ready << next;
      }
    }
  }
  auto result = Int(0);
  auto power = Int(1);
  for (auto i = 0u; i <= m_size; ++i) {
    const auto label = makeLabel('z', i);
    const auto &wire = getWire(label);
    if (not wire.value.has_value()) {
      common::throwRunTimeError(
          QString(
              "puzzle_2024_24::Circuit::run: output wire \"%1\" has no value")
              .arg(label));
    }
    if (*wire.value) {
      result += power;
    }
    power *= 2;
  }
  m_result = QString("%1").arg(result);
}

enum class GateRole { inputs_xor, inputs_and, output_xor, carry_and, carry_or };

inline QString roleToString(GateRole role) {
  switch (role) {
  case GateRole::inputs_xor:
    return "inputs_xor";
  case GateRole::inputs_and:
    return "inputs_and";
  case GateRole::output_xor:
    return "output_xor";
  case GateRole::carry_and:
    return "carry_and";
  case GateRole::carry_or:
    return "carry_or";
  }
  common::throwInvalidArgumentError(
      "puzzle_2024_24::roleToString: unknown role");
  return "";
}

inline uint qHash(GateRole role) { return qHash(roleToString(role)); }

class Adder {
public:
  bool has(GateRole role) const { return m_gates.contains(role); }

  std::size_t get(GateRole role) const {
    const auto it = m_gates.constFind(role);
    if (it == m_gates.end()) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_24::Adder::get: role %1 has not been set")
              .arg(roleToString(role)));
    }
    return it.value();
  }

  void set(GateRole role, std::size_t value) {
    if (m_gates.contains(role)) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_24::Adder::set: role %1 has already been set")
              .arg(roleToString(role)));
    }
    m_gates.insert(role, value);
  }

private:
  QHash<GateRole, std::size_t> m_gates;
};

void Circuit::fix() {
  const auto check_index = [this](const std::optional<Variable> &variable) {
    if (variable->index >= m_size) {
      common::throwInvalidArgumentError(
          QString(
              "puzzle_2024_24::Circuit::fix: out-of-range %1 index %2 (size "
              "is %3)")
              .arg(variable->name)
              .arg(variable->index)
              .arg(m_size));
    }
  };

  auto bad_outputs = QSet<QString>();
  const auto set_output = [&bad_outputs](Gate &gate, const QString &output) {
    bad_outputs.insert(output);
    gate.setOutput(output);
  };

  auto output_xor_gates = QSet<std::size_t>();
  auto carry_and_gates = QSet<std::size_t>();
  auto carry_or_gates = QSet<std::size_t>();
  auto adders = std::vector<Adder>(m_size);

  // Set adder's inputs gates (inputs_xor and inputs_and)
  for (auto gate_index = 0u; gate_index < m_gates.size(); ++gate_index) {
    const auto &gate = m_gates[gate_index];

    // Check if (x, y) inputs
    const auto first_input_variable = getVariable(gate.firstInput());
    const auto second_input_variable = getVariable(gate.secondInput());
    if (first_input_variable.has_value() and
        second_input_variable.has_value()) {
      check_index(first_input_variable);
      check_index(second_input_variable);
      if (first_input_variable->index != second_input_variable->index) {
        common::throwInvalidArgumentError(
            QString(
                "puzzle_2024_24::Circuit::fix: input index %1:%2 is different "
                "from input index %3:%4 in gate \"%5\"")
                .arg(first_input_variable->name)
                .arg(first_input_variable->index)
                .arg(second_input_variable->name)
                .arg(second_input_variable->index)
                .arg(gate.output()));
      }
      if ((first_input_variable->name != 'x' or
           second_input_variable->name != 'y') and
          (first_input_variable->name != 'y' or
           second_input_variable->name != 'x')) {
        common::throwInvalidArgumentError(
            QString(
                "puzzle_2024_24::Circuit::fix gate \"%1\" has unexpected input "
                "wires \"%2\" and \"%3\"")
                .arg(gate.output())
                .arg(gate.firstInput())
                .arg(gate.secondInput()));
      }
      const auto adder_index = first_input_variable->index;
      auto &adder = adders[adder_index];
      if (gate.type() == GateType::XOR) {
        adder.set(GateRole::inputs_xor, gate_index);
      } else if (gate.type() == GateType::AND) {
        adder.set(GateRole::inputs_and, gate_index);
      } else {
        common::throwInvalidArgumentError(
            QString("puzzle_2024_24::Circuit::fix: gate \"%1\" in adder %2 has "
                    "unexpected type "
                    "\"OR\"")
                .arg(gate.output())
                .arg(adder_index));
      }
    } else if (gate.type() == GateType::AND) {
      carry_and_gates.insert(gate_index);
    } else if (gate.type() == GateType::OR) {
      carry_or_gates.insert(gate_index);
    } else {
      output_xor_gates.insert(gate_index);
    }
  }

  // First output xor gate
  const auto adder_0_inputs_xor = adders[0].get(GateRole::inputs_xor);
  auto &first_output_gate = m_gates[adder_0_inputs_xor];
  if (first_output_gate.output() != "z00") {
    bad_outputs.insert(first_output_gate.output());
    set_output(first_output_gate, "z00");
  }
  adders[0].set(GateRole::output_xor, adder_0_inputs_xor);

  // First carry or gate
  adders[0].set(GateRole::carry_or, adders[0].get(GateRole::inputs_and));

  // Set all remaining gates
  for (auto i = 1u; i < adders.size(); ++i) {
    auto &inputs_xor_gate = m_gates[adders[i].get(GateRole::inputs_xor)];
    auto &inputs_and_gate = m_gates[adders[i].get(GateRole::inputs_and)];
    auto &previous_carry_or_gate =
        m_gates[adders[i - 1].get(GateRole::carry_or)];

    // Find output_xor
    auto output_xor_it = std::find_if(
        std::begin(output_xor_gates), std::end(output_xor_gates),
        [this, &inputs_xor_gate,
         &previous_carry_or_gate](const std::size_t gate_index) {
          const auto &output_xor_gate = m_gates[gate_index];
          return output_xor_gate.firstInput() == inputs_xor_gate.output() or
                 output_xor_gate.secondInput() == inputs_xor_gate.output() or
                 output_xor_gate.firstInput() ==
                     previous_carry_or_gate.output() or
                 output_xor_gate.secondInput() ==
                     previous_carry_or_gate.output();
        });
    if (output_xor_it == std::end(output_xor_gates)) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_24::Circuit::fix: cannot find the output_xor "
                  "gate for adder %1")
              .arg(i));
    }
    adders[i].set(GateRole::output_xor, *output_xor_it);
    auto &output_xor_gate = m_gates[*output_xor_it];
    output_xor_gates.erase(output_xor_it);

    // Repair bad inputs
    if (inputs_xor_gate.output() == output_xor_gate.firstInput()) {
      if (previous_carry_or_gate.output() != output_xor_gate.secondInput()) {
        bad_outputs.insert(previous_carry_or_gate.output());
        set_output(previous_carry_or_gate, output_xor_gate.secondInput());
      }
    } else if (inputs_xor_gate.output() == output_xor_gate.secondInput()) {
      if (previous_carry_or_gate.output() != output_xor_gate.firstInput()) {
        set_output(previous_carry_or_gate, output_xor_gate.firstInput());
      }
    } else {
      bad_outputs.insert(inputs_xor_gate.output());
      if (previous_carry_or_gate.output() == output_xor_gate.firstInput()) {
        set_output(inputs_xor_gate, output_xor_gate.secondInput());
      } else {
        set_output(inputs_xor_gate, output_xor_gate.firstInput());
      }
    }

    // Repair bad output
    const auto expected_output = makeLabel('z', i);
    if (output_xor_gate.output() != expected_output) {
      bad_outputs.insert(output_xor_gate.output());
      set_output(output_xor_gate, expected_output);
    }

    // Find carry_and
    auto carry_and_it = std::find_if(
        std::begin(carry_and_gates), std::end(carry_and_gates),
        [this, &inputs_xor_gate,
         &previous_carry_or_gate](const std::size_t gate_index) {
          const auto &carry_and_gate = m_gates[gate_index];
          return (carry_and_gate.firstInput() == inputs_xor_gate.output() and
                  carry_and_gate.secondInput() ==
                      previous_carry_or_gate.output()) or
                 (carry_and_gate.firstInput() ==
                      previous_carry_or_gate.output() or
                  carry_and_gate.secondInput() == inputs_xor_gate.output());
        });
    if (carry_and_it == std::end(carry_and_gates)) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_24::Circuit::fix: cannot find the carry_and "
                  "gate for adder %1")
              .arg(i));
    }
    adders[i].set(GateRole::carry_and, *carry_and_it);
    auto &carry_and_gate = m_gates[*carry_and_it];
    carry_and_gates.erase(carry_and_it);

    // Find carry_or
    auto carry_or_it = std::find_if(
        std::begin(carry_or_gates), std::end(carry_or_gates),
        [this, &inputs_and_gate,
         &carry_and_gate](const std::size_t gate_index) {
          const auto &carry_or_gate = m_gates[gate_index];
          return carry_or_gate.firstInput() == inputs_and_gate.output() or
                 carry_or_gate.secondInput() == inputs_and_gate.output() or
                 carry_or_gate.firstInput() == carry_and_gate.output() or
                 carry_or_gate.secondInput() == carry_and_gate.output();
        });
    if (carry_or_it == std::end(carry_or_gates)) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_24::Circuit::fix: cannot find the carry_or "
                  "gate for adder %1")
              .arg(i));
    }
    adders[i].set(GateRole::carry_or, *carry_or_it);
    auto &carry_or_gate = m_gates[*carry_or_it];
    carry_or_gates.erase(carry_or_it);

    // Repair bad inputs
    if (inputs_and_gate.output() == carry_or_gate.firstInput()) {
      if (carry_and_gate.output() != carry_or_gate.secondInput()) {
        bad_outputs.insert(carry_and_gate.output());
        set_output(carry_and_gate, carry_or_gate.secondInput());
      }
    } else if (inputs_and_gate.output() == carry_or_gate.secondInput()) {
      if (carry_and_gate.output() != carry_or_gate.firstInput()) {
        bad_outputs.insert(carry_or_gate.secondInput());
        set_output(carry_and_gate, carry_or_gate.firstInput());
      }
    } else {
      bad_outputs.insert(inputs_and_gate.output());
      if (carry_and_gate.output() == carry_or_gate.firstInput()) {
        set_output(inputs_and_gate, carry_or_gate.secondInput());
      } else {
        set_output(inputs_and_gate, carry_or_gate.firstInput());
      }
    }
  }

  if (not carry_and_gates.empty()) {
    common::throwRunTimeError(
        "puzzle_2024_24::Circuit::fix: failed to assign all carry_and gates");
  }
  if (not carry_or_gates.empty()) {
    common::throwRunTimeError(
        "puzzle_2024_24::Circuit::fix: failed to assign all carry_or gates");
  }
  if (not output_xor_gates.empty()) {
    common::throwRunTimeError(
        "puzzle_2024_24::Circuit::fix: failed to assign all ouput_xor gates");
  }

  auto bad_list =
      QList<QString>(std::begin(bad_outputs), std::end(bad_outputs));
  std::sort(std::begin(bad_list), std::end(bad_list));
  for (auto it = std::begin(bad_list); it != std::end(bad_list); ++it) {
    m_result += *it;
    if (std::next(it) != std::end(bad_list)) {
      m_result += ",";
    }
  }
}

} // namespace puzzle_2024_24

void Solver_2024_24_1::solve(const QString &input) {
  const auto circuit = puzzle_2024_24::Circuit(input, true);
  emit finished(circuit.result());
}

void Solver_2024_24_2::solve(const QString &input) {
  const auto circuit = puzzle_2024_24::Circuit(input, false);
  emit finished(circuit.result());
}
