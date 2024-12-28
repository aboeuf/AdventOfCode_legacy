#pragma once
#include <QSet>
#include <optional>
#include <solvers.h>

namespace puzzle_2024_24 {

using Int = unsigned long long;

enum class GateType { AND, OR, XOR };

class Gate {
public:
  Gate(const QString &input);

  GateType type() const;
  const QString &firstInput() const;
  const QString &secondInput() const;
  const QString &output() const;

  void setOutput(const QString &output);

private:
  GateType m_type;
  QString m_first_input;
  QString m_second_input;
  QString m_output;
};

using Bit = std::optional<bool>;

struct Wire {
  Bit value{std::nullopt};
  std::optional<std::size_t> previous_gate{std::nullopt};
  QSet<std::size_t> next_gates{};
};

class Circuit {
public:
  Circuit(const QString &input, bool part_one);

  const QString &result() const;

private:
  const Wire &getWire(const QString &label) const;
  bool isReady(std::size_t gate_index) const;

  void run();
  void fix();

  uint m_size;
  std::vector<Gate> m_gates;
  QHash<QString, Wire> m_wires;

  QString m_result;
};

} // namespace puzzle_2024_24

class Solver_2024_24_1 : public Solver {
public:
  void solve(const QString &input) override;
};

class Solver_2024_24_2 : public Solver {
public:
  void solve(const QString &input) override;
};
