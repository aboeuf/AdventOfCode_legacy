#include <2023/puzzle_2023_20.h>
#include <common.h>
#include <numeric>

namespace puzzle_2023_20 {

using Int = unsigned long long int;

enum class Pulse {
    low,
    hight
};

struct Message
{
    Message() = default;
    Message(const QString& from, const QString& to, const Pulse pulse) :
        from{from}, to{to}, pulse{pulse} {}

    QString from{"button"};
    QString to{"broadcaster"};
    Pulse pulse{Pulse::low};
};

struct Compter {
    static void reset() {
        low = Int{0};
        hight = Int{0};
    }
    static Int low;
    static Int hight;
};

Int Compter::low = Int{0};
Int Compter::hight = Int{0};

class Module
{
public:
    Module() = default;
    Module(const QString& label, const QStringList& outputs) : m_label{label}, m_outputs{outputs} {}

    const QString& label() const { return m_label; }
    const QStringList& outputs() { return m_outputs; }

    void broadcast(const Pulse pulse, QList<Message>& out) {
        for (const auto& output : m_outputs) {
            out << Message{label(), output, pulse};
            if (pulse == Pulse::low)
                ++Compter::low;
            else
                ++Compter::hight;
        }
    }

    virtual void process(const Message& in, QList<Message>& out) = 0;

    bool isSet() const { return m_is_set; }

    Int getCycleSize() const {
        if (not m_is_set)
            return Int{0};
        return std::max(Int{1}, static_cast<Int>(m_history.size() / 2));
    }

private:
    QString m_label{""};
    QStringList m_outputs{};

protected:
//    void settle() {
//        if (m_is_set or m_history.empty() or not m_history.size() % 2 == 0)
//            return;
//       auto mid =
//    }

    bool m_is_set{false};
    QList<bool> m_history{};
};

class Broadcaster : public Module
{
public:
    Broadcaster(const QString& label, const QStringList& outputs) : Module{label, outputs} {}

    void process(const Message& in, QList<Message>& out) final {
        broadcast(in.pulse, out);
    }
};

class FlipFlop : public Module
{
public:
    FlipFlop(const QString& label, const QStringList& outputs) : Module{label, outputs}, m_is_on{false} {}

    void process(const Message& in, QList<Message>& out) final {
        if (in.pulse == Pulse::hight)
            return;
        m_is_on = not m_is_on;
        broadcast(m_is_on ? Pulse::hight : Pulse::low, out);
    }

private:
    bool m_is_on;
};

class Conjunction : public Module
{
public:
    Conjunction(const QString& label, const QStringList& outputs) : Module{label, outputs} {}

    const QHash<QString, uint>& firstOnAtCycle() const {
        return m_first_on_at_cycle;
    }

    void process(const Message& in, QList<Message>& out) final {
        m_last_received[in.from] = in.pulse;
//        if (in.pulse == Pulse::hight and not m_first_on_at_cycle.contains(in.from))
//            m_first_on_at_cycle[in.from] = Compter::nb_cycles;
        auto all_on = [this]() {
            for (auto it = std::begin(m_last_received); it != std::end(m_last_received); ++it)
                if (it.value() == Pulse::low)
                    return false;
            return true;
        }();
        broadcast(all_on ? Pulse::low : Pulse::hight, out);
    }

    void addInput(const QString& label) {
        m_last_received[label] = Pulse::low;
    }

private:
    bool m_is_on;
    QHash<QString, Pulse> m_last_received;
    QHash<QString, uint> m_first_on_at_cycle;
};

class Network
{
public:
    Network(const QString& input) {
        // Parse input
        auto lines = common::splitLines(input);
        for (auto& line : lines) {
            line.remove(' ').remove('-');
            const auto splitted = common::splitValues(line, '>');
            if (splitted.size() != 2)
                common::throwInvalidArgumentError(QString("Network::constructor: cannot parse line \"%1\"").arg(line));
            const auto& label = splitted[0];
            if (label.isEmpty())
                common::throwInvalidArgumentError("Network::constructor: empty module label");
            const auto outputs = common::splitValues(splitted[1], ',');
            if (label == "broadcaster")
                m_modules[label] = std::make_shared<Broadcaster>(label, outputs);
            else if (label.front() == '%')
                m_modules[label.mid(1)] = std::make_shared<FlipFlop>(label.mid(1), outputs);
            else if (label.front() == '&')
                m_modules[label.mid(1)] = std::make_shared<Conjunction>(label.mid(1), outputs);
            else
                common::throwInvalidArgumentError(QString("Network::constructor: cannot determine type of module \"%1\"").arg(label));
        }

        // Connect conjuctions inputs
        for (auto it = std::begin(m_modules); it != std::end(m_modules); ++it) {
            const auto& label = it.key();
            const auto& outputs = it.value()->outputs();
            for (const auto& output : outputs) {
                if (m_modules.contains(output)) {
                    auto casted = std::dynamic_pointer_cast<Conjunction>(m_modules[output]);
                    if (casted)
                        casted->addInput(label);
                }
            }
        }
    }

    void pressButton() {
        ++Compter::low;
        auto current = QList<Message>{Message{"", "broadcaster", Pulse::low}};
        while (not current.empty()) {
            auto out = QList<Message>{};
            for (const auto& message : current) {
                if (m_modules.contains(message.to))
                    m_modules[message.to]->process(message, out);
            }
            std::swap(current, out);
        }
//        ++Compter::nb_cycles;
    }

    QString solveOne() {
        Compter::reset();
        for (auto i = 0u; i < 1000u; ++i)
            pressButton();
        return QString("%1").arg(Compter::low * Compter::hight);
    }

    QString solveTwo() {
        Compter::reset();
//        ++Compter::nb_cycles;

        // Find the conjontion module "nearest" to to "rx"
        auto current = QString("rx");
        auto inputs = QSet<QString>{};
        for (;;) {
            qDebug() << current;
            inputs.clear();
            for (auto it = std::begin(m_modules); it != std::end(m_modules); ++it)
                if (it.value()->outputs().contains(current))
                    inputs << it.key();
            if (inputs.empty())
                common::throwRunTimeError("Network::solveTwo: empty inputs");
            if (inputs.size() > 1)
                break;
            current = *inputs.begin();
        }
        auto debug_string = QString("|");
        for (const auto& input : inputs)
            debug_string += input + "|";
        qDebug() << debug_string;

        const auto conjuction_module = std::dynamic_pointer_cast<Conjunction>(m_modules[current]);
        if (not conjuction_module)
            common::throwRunTimeError("Network::solveTwo: failed to find conjuction module");

        // Run the cycles until all of those module have turned on at least once
        const auto& first_on = conjuction_module->firstOnAtCycle();
        const auto all_inputs_are_set = [inputs, &first_on]() {
            for (const auto& input : inputs)
                if (not first_on.contains(input))
                    return false;
            return true;
        };
        while (not all_inputs_are_set()) { pressButton(); }

        // Compute the lcm of all indexes
        auto lcm = std::begin(first_on).value() + 1u;
        for (auto it = std::next(std::begin(first_on)); it != std::end(first_on); ++it) {
            const auto cycle_index = it.value() + 1u;
            lcm = (lcm * cycle_index) / std::gcd(lcm, cycle_index);
        }
        return QString("%1").arg(lcm);
    }

private:
    QHash<QString, std::shared_ptr<Module>> m_modules;
};

}

void Solver_2023_20_1::solve(const QString& input)
{
    auto network = puzzle_2023_20::Network{input};
    emit finished(network.solveOne());
}

void Solver_2023_20_2::solve(const QString& input)
{
    auto network = puzzle_2023_20::Network{input};
    emit finished(network.solveTwo());
}
