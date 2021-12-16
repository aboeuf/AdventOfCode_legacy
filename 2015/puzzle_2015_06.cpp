#include <2015/puzzle_2015_06.h>
#include <common.h>
#include <optional>
#include <QSet>

using Int = unsigned long long int;

struct Instruction
{
  Instruction(const QString& input) {
    auto rx = QRegExp{"^(turn on|turn off|toggle) (\\d+),(\\d+) through (\\d+),(\\d+)$"};
    valid = rx.exactMatch(input);
    if (valid) {
      if (rx.cap(1) == "toggle")
        on_off = std::nullopt;
      else if (rx.cap(1) == "turn on")
        on_off = true;
      else
        on_off = false;
      xmin = rx.cap(2).toUInt();
      ymin = rx.cap(3).toUInt();
      xmax = rx.cap(4).toUInt();
      ymax = rx.cap(5).toUInt();
    }
  }

  bool valid;
  std::optional<bool> on_off;
  uint xmin;
  uint ymin;
  uint xmax;
  uint ymax;
};

inline QString makeLight(uint x, uint y)
{
  return QString("%1%2").arg(x, 3, 10, QChar('0')).arg(y, 3, 10, QChar('0'));
}

class ChristmasLights_V1
{
public:
  ChristmasLights_V1(const QString& instructions) {
    const auto lines = common::splitLines(instructions);
    for (const auto& line : lines) {
      const auto instruction = Instruction{line};
      if (instruction.valid) {
        for (auto x = instruction.xmin; x <= instruction.xmax; ++x) {
          for (auto y = instruction.ymin; y <= instruction.ymax; ++y) {
            const auto light = makeLight(x, y);
            if (instruction.on_off.has_value()) {
              if (instruction.on_off.value())
                m_grid.insert(light);
              else
                m_grid.remove(light);
            } else {
              if (m_grid.contains(light))
                m_grid.remove(light);
              else
                m_grid.insert(light);
            }
          }
        }
      }
    }
  }

  QString nbLit() const
  {
    return QString("%1").arg(m_grid.size());
  }

private:
  QSet<QString> m_grid;
};

class ChristmasLights_V2
{
public:
  ChristmasLights_V2(const QString& instructions) {
    const auto lines = common::splitLines(instructions);
    for (const auto& line : lines) {
      const auto instruction = Instruction{line};
      if (instruction.valid) {
        const auto decrease = instruction.on_off.has_value() and not instruction.on_off.value();
        const auto increase = instruction.on_off.has_value() ? Int{1} : Int{2};
        for (auto x = instruction.xmin; x <= instruction.xmax; ++x) {
          for (auto y = instruction.ymin; y <= instruction.ymax; ++y) {
            const auto light = makeLight(x, y);
            auto it = m_grid.find(light);
            if (decrease) {
              if (it != std::end(m_grid)) {
                if (it.value() == Int{1})
                  m_grid.erase(it);
                else
                  --(it.value());
              }
            } else {
              if (it == std::end(m_grid))
                m_grid[light] = increase;
              else
                it.value() += increase;
            }
          }
        }
      }
    }
  }

  QString totalBrightness() const
  {
    auto total_brightness = Int{0};
    for (const auto& brightness : m_grid)
      total_brightness += brightness;
    return QString("%1").arg(total_brightness);
  }

private:
  QMap<QString, Int> m_grid;
};

void Solver_2015_06_1::solve(const QString& input)
{
  emit finished(ChristmasLights_V1(input).nbLit());
}

void Solver_2015_06_2::solve(const QString& input)
{
   emit finished(ChristmasLights_V2(input).totalBrightness());
}
