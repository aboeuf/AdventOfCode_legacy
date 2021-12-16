#include <2015/puzzle_2015_07.h>
#include <common.h>
#include <optional>
#include <deque>
#include <set>

using Int = unsigned short;

struct IO
{
  IO() = default;

  IO(const QString& str)
  {
    if (std::all_of(std::begin(str),
                    std::end(str),
                    [](const auto& c) { return c.isDigit(); }))
      m_signal = str.toUShort();
    else
      m_identifier = str;
  }

  QString m_identifier{""};
  std::optional<Int> m_signal{std::nullopt};
};

enum Type
{
  FORWARD,
  NOT,
  AND,
  OR,
  RSHIFT,
  LSHIFT
};

const QMap<QString, Type> size_5_types = {
  {"AND", AND},
  {"OR", OR},
  {"RSHIFT", RSHIFT},
  {"LSHIFT", LSHIFT},
};

inline bool isValidGate(const QStringList& words)
{
  if (words.size() < 3)
    return false;
  if (words.size() > 4)
    return size_5_types.contains(words[1]);
  return true;
}

struct Gate
{
  Gate() = default;

  Gate(const QStringList& words) {
    if (words.size() == 3) {
      m_type = FORWARD;
      m_inputs << IO(words[0]);
      m_output = IO(words[2]);
      return;
    }
    if (words.size() == 4) {
      m_type = NOT;
      m_inputs << IO(words[1]);
      m_output = words[3];
      return;
    }
    m_type = size_5_types[words[1]];
    m_inputs << IO(words[0]);
    m_inputs << IO(words[2]);
    m_output = words[4];
  }

  bool ready() const
  {
    return std::all_of(std::begin(m_inputs),
                       std::end(m_inputs),
                       [](const auto& io) { return io.m_signal.has_value(); });
  }

  bool evaluated() const
  {
    return m_output.m_signal.has_value();
  }

  void setSignal(const IO& io)
  {
    if (io.m_signal.has_value()) {
      for (auto& in : m_inputs)
        if (not in.m_signal.has_value() and
            in.m_identifier == io.m_identifier)
          in.m_signal = io.m_signal.value();
    }
  }

  void evaluate()
  {
    if (not m_inputs.empty() and ready()) {
      switch (m_type) {
      case FORWARD:
        m_output.m_signal = m_inputs.front().m_signal.value();
        break;
      case NOT:
        m_output.m_signal = ~(m_inputs.front().m_signal.value());
        break;
      case AND:
        m_output.m_signal = m_inputs.front().m_signal.value() & m_inputs.back().m_signal.value();
        break;
      case OR:
        m_output.m_signal = m_inputs.front().m_signal.value() | m_inputs.back().m_signal.value();
        break;
      case RSHIFT:
        m_output.m_signal = m_inputs.front().m_signal.value() >> m_inputs.back().m_signal.value();
        break;
      case LSHIFT:
        m_output.m_signal = m_inputs.front().m_signal.value() << m_inputs.back().m_signal.value();
        break;
      }
    }
  }

  Type m_type{FORWARD};
  QList<IO> m_inputs{};
  IO m_output{};
};

class Circuit
{
public:
  Circuit(const QString& input)
  {
    const auto lines = common::splitLines(input);
    m_gates.reserve(lines.size());
    for (const auto& line : lines) {
      const auto words = common::splitValues(line, QChar(' '));
      if (isValidGate(words))
        m_gates.emplace_back(words);
    }
    for (const auto& gate : m_gates) {
      for (const auto& io : gate.m_inputs) {
        if (not io.m_identifier.isEmpty())
          m_wires[io.m_identifier] = std::nullopt;
        m_wires[gate.m_output.m_identifier] = std::nullopt;
      }
    }
    for (const auto& id : m_wires.keys())
      m_listeners[id] = std::set<std::size_t>{};
    for (auto i = 0; i < m_gates.size(); ++i) {
      for (const auto& io : m_gates[i].m_inputs)
        if (not io.m_identifier.isEmpty())
          m_listeners[io.m_identifier].insert(i);
    }
  }

  QString solvePuzzleOne() {
    reset();
    run();
    const auto sig_a = getSignal("a");
    return sig_a.has_value() ? QString("%1").arg(sig_a.value()) : "none";
  }

  QString solvePuzzleTwo() {
    reset();
    run();
    auto sig_a = getSignal("a");
    reset();
    if (sig_a.has_value()) {
      auto sig_b = IO("b");
      sig_b.m_signal = sig_a.value();
      setSignal(sig_b);
    }
    run();
    sig_a = getSignal("a");
    return sig_a.has_value() ? QString("%1").arg(sig_a.value()) : "none";
  }

private:
  void reset()
  {
    for (auto& gate : m_gates) {
      for (auto& io : gate.m_inputs)
        if (not io.m_identifier.isEmpty())
          io.m_signal = std::nullopt;
      gate.m_output.m_signal = std::nullopt;
    }
    for (auto& id : m_wires.keys())
      m_wires[id] = std::nullopt;
  }

  void run()
  {
    auto ready = std::deque<std::size_t>{};
    for (auto i = 0; i < m_gates.size(); ++i)
      if (m_gates[i].ready())
        ready.push_back(i);

    for(auto it = std::begin(ready); it != std::end(ready); ++it) {
      auto& gate = m_gates[*it];
      gate.evaluate();
      if (gate.evaluated()) {
        setSignal(gate.m_output);
        for (auto i : m_listeners[gate.m_output.m_identifier])
          if (not m_gates[i].evaluated() and m_gates[i].ready())
            ready.push_back(i);
      }
    }
  }

  void setSignal(const IO& io)
  {
    m_wires[io.m_identifier] = io.m_signal;
    for (auto i : m_listeners[io.m_identifier])
      m_gates[i].setSignal(io);
  }

  std::optional<Int> getSignal(const QString& wire) const
  {
    auto it = m_wires.find(wire);
    if (it == m_wires.end())
      return std::nullopt;
    return it.value();
  }

  std::vector<Gate> m_gates{};
  QMap<QString, std::set<std::size_t>> m_listeners{};
  QMap<QString, std::optional<Int>> m_wires;
};

void Solver_2015_07_1::solve(const QString& input)
{
  emit finished(Circuit(input).solvePuzzleOne());
}

void Solver_2015_07_2::solve(const QString& input)
{
  emit finished(Circuit(input).solvePuzzleTwo());
}
