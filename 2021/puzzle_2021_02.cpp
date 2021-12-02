#include <2021/puzzle_2021_02.h>
#include <common.h>

class Submarine
{
public:
  Submarine() = default;

  Submarine(const QString& commands) : Submarine{}
  {
    for (const auto& line : common::splitLines(commands)) {
      const auto command = line.split(' ');
      if (command.size() < 2)
        continue;
      bool conversion_ok;
      auto value = command[1].toInt(&conversion_ok);
      if (not conversion_ok)
        continue;
      if (command[0] == "forward")
        m_horizontal_position += value;
      else if (command[0] == "down")
        m_depth += value;
      else if (command[0] == "up")
        m_depth -= value;
    }
  }

  QString pose() const
  {
    return QString{"%1"}.arg(m_horizontal_position * m_depth);
  }

protected:
  int m_horizontal_position{0};
  int m_depth{0};
};

class Submarine_V2 : public Submarine
{
public:
  Submarine_V2(const QString& commands) : Submarine{}
  {
    for (const auto& line : common::splitLines(commands)) {
      const auto command = line.split(' ');
      if (command.size() < 2)
        continue;
      bool conversion_ok;
      auto value = command[1].toInt(&conversion_ok);
      if (not conversion_ok)
        continue;
      if (command[0] == "forward") {
        m_horizontal_position += value;
        m_depth += m_aim * value;
      }
      else if (command[0] == "down")
        m_aim += value;
      else if (command[0] == "up")
        m_aim -= value;
    }
  }

private:
  int m_aim{0};
};

void Solver_2021_02_1::solve(const QString& input)
{
  const auto sub = Submarine{input};
  emit finished(sub.pose());
}

void Solver_2021_02_2::solve(const QString& input)
{
  const auto sub = Submarine_V2{input};
  emit finished(sub.pose());
}

