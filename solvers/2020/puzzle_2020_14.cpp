#include <QMap>
#include <QVector>
#include <solvers/common.h>
#include <solvers/2020/puzzle_2020_14.h>

namespace puzzle_2020_14 {

using Int = unsigned long long int;

Int toInt(const QString &str, bool &ok) { return str.toULongLong(&ok, 2); }

QString fromInt(Int n) { return QString("%1").arg(n, 36, 2, QChar('0')); }

Int applyMaskV1(const QString &mask, Int n) {
  bool ok;
  QString val = fromInt(n);
  for (int i = 0; i < std::min(mask.size(), val.size()); ++i) {
    if (mask[i] == '0')
      val[i] = '0';
    else if (mask[i] == '1')
      val[i] = '1';
  }
  return toInt(val, ok);
}

QString applyMaskV2(const QString &mask, Int n) {
  QString address = fromInt(n);
  for (int i = 0; i < std::min(mask.size(), address.size()); ++i) {
    if (mask[i] == '1')
      address[i] = '1';
    else if (mask[i] == 'X')
      address[i] = 'X';
  }
  return address;
}

struct Emulator {
  Int memorySum() const {
    Int sum = 0;
    for (Int val : m_memory.values())
      sum += val;
    return sum;
  }

  void reset() {
    m_mask.clear();
    m_memory.clear();
  }

  QMap<Int, Int> m_memory{};
  QString m_mask{};
};

struct Instruction {
  Instruction() = default;
  virtual ~Instruction() = default;
  virtual void runOn(Emulator &) const = 0;
};

struct MaskChange : public Instruction {
  MaskChange(const QString &mask) : Instruction(), m_mask{mask} {}
  void runOn(Emulator &emulator) const override { emulator.m_mask = m_mask; }
  QString m_mask{};
};

struct WriteV1 : public Instruction {
  WriteV1(Int address, Int value)
      : Instruction(), m_address{address}, m_value{value} {}
  void runOn(Emulator &emulator) const override {
    Int masked_value = applyMaskV1(emulator.m_mask, m_value);
    emulator.m_memory[m_address] = masked_value;
  }
  Int m_address{0};
  Int m_value{0};
};

struct WriteV2 : public Instruction {
  WriteV2(Int address, Int value)
      : Instruction(), m_address{address}, m_value{value} {}
  void runOn(Emulator &emulator) const override {
    QString address = applyMaskV2(emulator.m_mask, m_address);
    if (address.isEmpty())
      return;
    QList<Int> addresses;
    if (address.back() == '0')
      addresses << 0;
    else if (address.back() == '1')
      addresses << 1;
    else
      addresses << 0 << 1;
    Int pow = 2;
    for (auto it_char = std::next(address.rbegin()); it_char != address.rend();
         ++it_char) {
      if (*it_char == 'X') {
        int s = addresses.size();
        int i = 0;
        for (auto it = addresses.begin(); i < s; ++it) {
          addresses << *it + pow;
          ++i;
        }
      } else if (*it_char == '1') {
        for (Int &add : addresses)
          add += pow;
      }
      pow *= 2;
    }
    for (Int &add : addresses)
      emulator.m_memory[add] = m_value;
  }
  Int m_address{0};
  Int m_value{0};
};

struct Program {
  Program(const QString &input, bool version_2) {
    bool ok;
    QRegExp mask_rx("mask = ([X01]*)");
    QRegExp mem_rx("mem\\[(\\d+)\\] = (\\d+)");
    const QStringList lines = common::splitLines(input);
    m_instructions.reserve(lines.size());
    for (const QString line : lines) {
      if (mask_rx.indexIn(line) >= 0) {
        const QStringList captured = mask_rx.capturedTexts();
        if (captured.size() == 2)
          m_instructions << new MaskChange(captured[1]);
      } else if (mem_rx.indexIn(line) >= 0) {
        const QStringList captured = mem_rx.capturedTexts();
        if (captured.size() == 3) {
          Int address = captured[1].toULongLong(&ok, 10);
          if (ok) {
            Int value = captured[2].toULongLong(&ok, 10);
            if (ok) {
              if (version_2)
                m_instructions << new WriteV2(address, value);
              else
                m_instructions << new WriteV1(address, value);
            }
          }
        }
      }
    }
    m_instructions.shrink_to_fit();
  }

  ~Program() {
    for (Instruction *instruction : m_instructions)
      delete instruction;
  }

  void runOn(Emulator &emulator) const {
    emulator.reset();
    for (Instruction *instruction : m_instructions)
      instruction->runOn(emulator);
  }

  QVector<Instruction *> m_instructions;
};

} // namespace puzzle_2020_14

void Solver_2020_14_1::solve(const QString &input) {
  using namespace puzzle_2020_14;
  Emulator emulator;
  Program(input, false).runOn(emulator);
  emit finished(QString::number(emulator.memorySum()));
}

void Solver_2020_14_2::solve(const QString &input) {
  using namespace puzzle_2020_14;
  Emulator emulator;
  Program(input, true).runOn(emulator);
  emit finished(QString::number(emulator.memorySum()));
}
