#include <numeric>
#include <solvers/2023/puzzle_2023_20.h>
#include <solvers/common.h>

namespace puzzle_2023_20 {

using Int = unsigned long long int;

enum class Pulse { low, hight };

struct Message {
  Message() = default;
  Message(const QString &from, const QString &to, const Pulse pulse)
      : from{from}, to{to}, pulse{pulse} {}

  QString from{"button"};
  QString to{"broadcaster"};
  Pulse pulse{Pulse::low};
};

struct Compter {
  static void reset() {
    low = Int{0};
    hight = Int{0};
    nb_cycles = Int{0};
  }
  static Int low;
  static Int hight;
  static Int nb_cycles;
};

Int Compter::low = Int{0};
Int Compter::hight = Int{0};
Int Compter::nb_cycles = Int{0};

class Module {
public:
  Module() = default;
  Module(const QString &label, const QStringList &outputs)
      : m_label{label}, m_outputs{outputs} {}

  const QString &label() const { return m_label; }
  const QStringList &outputs() { return m_outputs; }

  void broadcast(const Pulse pulse, QList<Message> &out) {
    for (const auto &output : m_outputs) {
      out << Message{label(), output, pulse};
      if (pulse == Pulse::low)
        ++Compter::low;
      else
        ++Compter::hight;
    }
  }

  virtual void process(const Message &in, QList<Message> &out) = 0;

private:
  QString m_label{""};
  QStringList m_outputs{};
};

class Broadcaster : public Module {
public:
  Broadcaster(const QString &label, const QStringList &outputs)
      : Module{label, outputs} {}

  void process(const Message &in, QList<Message> &out) final {
    broadcast(in.pulse, out);
  }
};

class FlipFlop : public Module {
public:
  FlipFlop(const QString &label, const QStringList &outputs)
      : Module{label, outputs}, m_is_on{false} {}

  void process(const Message &in, QList<Message> &out) final {
    if (in.pulse == Pulse::hight)
      return;
    m_is_on = not m_is_on;
    broadcast(m_is_on ? Pulse::hight : Pulse::low, out);
  }

private:
  bool m_is_on;
};

class Conjunction : public Module {
public:
  Conjunction(const QString &label, const QStringList &outputs)
      : Module{label, outputs} {}

  void process(const Message &in, QList<Message> &out) final {
    m_last_received[in.from] = in.pulse;
    auto all_on = [this]() {
      for (auto it = std::begin(m_last_received);
           it != std::end(m_last_received); ++it)
        if (it.value() == Pulse::low)
          return false;
      return true;
    }();
    broadcast(all_on ? Pulse::low : Pulse::hight, out);
  }

  const std::optional<Int> firstOnAt() const { return m_first_on_at; }

  bool isInput(const QString &label) const {
    return m_last_received.contains(label);
  }

  void addInput(const QString &label) { m_last_received[label] = Pulse::low; }

  void setFirstOnAt(const QString &label, const Int nb_cycles) {
    m_first_on_at_map[label] = nb_cycles;
  }

  bool computeFirstOnAt() {
    if (m_first_on_at.has_value())
      return false;
    auto all_set = true;
    for (auto it = std::cbegin(m_last_received);
         it != std::cend(m_last_received); ++it) {
      if (not m_first_on_at_map.contains(it.key())) {
        if (it.value() == Pulse::hight)
          m_first_on_at_map[it.key()] = Compter::nb_cycles;
        else
          all_set = false;
      }
    }
    if (all_set) {
      qDebug() << "Compute LCM for module" << label();
      auto it = std::cbegin(m_first_on_at_map);
      auto lcm = it.value();
      qDebug() << "   " << it.value() << "->" << lcm;
      ++it;
      for (; it != std::cend(m_first_on_at_map); ++it) {
        lcm = (lcm * it.value()) / std::gcd(lcm, it.value());
        qDebug() << "   " << it.value() << "->" << lcm;
      }
      qDebug() << "result:" << lcm;
      qDebug() << "";
      m_first_on_at = lcm;
    }
    return all_set;
  }

private:
  QHash<QString, Pulse> m_last_received;
  QHash<QString, Int> m_first_on_at_map{};
  std::optional<Int> m_first_on_at{std::nullopt};
};

class Network {
public:
  Network(const QString &input) {
    // Parse input
    auto lines = common::splitLines(input);
    for (auto &line : lines) {
      line.remove(' ').remove('-');
      const auto splitted = common::splitValues(line, '>');
      if (splitted.size() != 2)
        common::throwInvalidArgumentError(
            QString("Network::constructor: cannot parse line \"%1\"")
                .arg(line));
      const auto &label = splitted[0];
      if (label.isEmpty())
        common::throwInvalidArgumentError(
            "Network::constructor: empty module label");
      const auto outputs = common::splitValues(splitted[1], ',');
      if (label == "broadcaster")
        m_modules[label] = std::make_shared<Broadcaster>(label, outputs);
      else if (label.front() == '%')
        m_modules[label.mid(1)] =
            std::make_shared<FlipFlop>(label.mid(1), outputs);
      else if (label.front() == '&')
        m_modules[label.mid(1)] =
            std::make_shared<Conjunction>(label.mid(1), outputs);
      else
        common::throwInvalidArgumentError(
            QString(
                "Network::constructor: cannot determine type of module \"%1\"")
                .arg(label));
    }

    // Connect conjuctions inputs
    for (auto it = std::begin(m_modules); it != std::end(m_modules); ++it) {
      const auto &label = it.key();
      const auto &outputs = it.value()->outputs();
      for (const auto &output : outputs) {
        if (m_modules.contains(output)) {
          auto casted =
              std::dynamic_pointer_cast<Conjunction>(m_modules[output]);
          if (casted)
            casted->addInput(label);
        }
      }
    }
  }

  void pressButton() {
    ++Compter::nb_cycles;
    ++Compter::low;
    auto current = QList<Message>{Message{"", "broadcaster", Pulse::low}};
    while (not current.empty()) {
      auto out = QList<Message>{};
      for (const auto &message : current) {
        if (m_modules.contains(message.to))
          m_modules[message.to]->process(message, out);
      }
      std::swap(current, out);
    }
  }

  QString solveOne() {
    Compter::reset();
    for (auto i = 0u; i < 1000u; ++i)
      pressButton();
    return QString("%1").arg(Compter::low * Compter::hight);
  }

  QString solveTwo() {
    auto conjonctions = QHash<QString, std::shared_ptr<Conjunction>>{};
    for (auto it = std::begin(m_modules); it != std::end(m_modules); ++it) {
      auto conjonction = std::dynamic_pointer_cast<Conjunction>(it.value());
      if (conjonction)
        conjonctions[conjonction->label()] = conjonction;
    }

    Compter::reset();

    while (std::any_of(std::cbegin(conjonctions), std::cend(conjonctions),
                       [](const auto &module) {
                         return not module->firstOnAt().has_value();
                       })) {
      pressButton();
      auto queue = QSet<QString>{};
      for (auto it = std::cbegin(conjonctions); it != std::end(conjonctions);
           ++it)
        queue << it.key();
      while (not queue.empty()) {
        auto module = conjonctions[*std::begin(queue)];
        queue.erase(std::begin(queue));
        if (module->computeFirstOnAt()) {
          for (auto it = std::begin(conjonctions); it != std::end(conjonctions);
               ++it) {
            if (it.value()->isInput(module->label())) {
              queue << it.key();
              it.value()->setFirstOnAt(module->label(),
                                       module->firstOnAt().value());
            }
          }
        }
      }
    }

    auto max_nb_cycles = Int{0};
    for (auto it = std::begin(conjonctions); it != std::end(conjonctions); ++it)
      max_nb_cycles = std::max(max_nb_cycles, it.value()->firstOnAt().value());

    return QString("%1").arg(max_nb_cycles);
  }

private:
  QHash<QString, std::shared_ptr<Module>> m_modules;
};

} // namespace puzzle_2023_20

void Solver_2023_20_1::solve(const QString &input) {
  auto network = puzzle_2023_20::Network{input};
  emit finished(network.solveOne());
}

void Solver_2023_20_2::solve(const QString &input) {
  auto network = puzzle_2023_20::Network{input};
  emit finished(network.solveTwo());
}
