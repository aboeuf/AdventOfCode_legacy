#include <2020/puzzle_2020_08.h>
#include <common.h>
#include <QDebug>

namespace puzzle_2020_08
{
struct Instruction
{
    Instruction() = default;
    Instruction(const QString& operation,
                int argument) :
        m_operation{operation},
        m_argument{argument} {}

    QString m_operation{};
    int m_argument{0};
    bool m_called{false};
};

enum ReturnStatus
{
    LOOP,
    TERMINATED,
    ERROR
};

struct Program
{
    Program(const QString& input) {
        const QStringList& lines = common::splitLines(input);
        m_instructions.reserve(lines.size());
        for (const QString& line : lines) {
            const QStringList& values = common::splitValues(line, ' ');
            if (values.size() < 2)
                continue;
            bool ok;
            int argument = values[1].toInt(&ok);
            if (ok)
                m_instructions.emplace_back(values[0], argument);
        }
        nb_instructions = static_cast<int>(m_instructions.size());
    }

    ReturnStatus run()
    {
        while (m_instruction_pointer >= 0 &&
               m_instruction_pointer < nb_instructions) {
            Instruction& I = m_instructions[m_instruction_pointer];
            if (I.m_called)
                return LOOP;
            I.m_called = true;
            if (I.m_operation == "acc") {
                m_accumulator += I.m_argument;
                ++m_instruction_pointer;
            } else if (I.m_operation == "jmp") {
                m_instruction_pointer += I.m_argument;
            } else if (I.m_operation == "nop") {
                ++m_instruction_pointer;
            } else {
                m_error = QString("Unknown operation \"%1\"").arg(I.m_operation);
                return ERROR;
            }
        }
        if (m_instruction_pointer != nb_instructions) {
            m_error = QString("Bad instruction pointer %1 (%2 instructions)")
                    .arg(m_instruction_pointer)
                    .arg(nb_instructions);
            return ERROR;
        }
        return TERMINATED;
    }

    int m_instruction_pointer{0};
    int m_accumulator{0};
    std::vector<Instruction> m_instructions{0};
    int nb_instructions{0};
    QString m_error{};
};

}

void Solver_2020_08_1::solve(const QString& input)
{
    using namespace puzzle_2020_08;
    Program prog(input);
    if (prog.run() != ERROR)
        emit finished(QString::number(prog.m_accumulator));
    else
        emit finished(prog.m_error);
}

void Solver_2020_08_2::solve(const QString& input)
{
    using namespace puzzle_2020_08;
    const Program prog(input);
    for (int i = 0; i < prog.nb_instructions; ++i) {
        if (prog.m_instructions[i].m_operation == "acc")
            continue;
        Program copy = prog;
        if (copy.m_instructions[i].m_operation == "jmp")
            copy.m_instructions[i].m_operation = "nop";
        else
            copy.m_instructions[i].m_operation = "jmp";
        if (copy.run() == TERMINATED) {
            emit finished(QString::number(copy.m_accumulator));
            return;
        }
    }
    emit finished("FAILURE");
}

