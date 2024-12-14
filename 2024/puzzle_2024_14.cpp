#include <2024/puzzle_2024_14.h>
#include <common.h>

#include <QDir>
#include <QImage>
#include <QStandardPaths>

namespace puzzle_2024_14 {

using Int = long long;

const auto black = QColor(0, 0, 0);
const auto white = QColor(255, 255, 255);

const auto pictures_dir = QDir(
    QDir(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation))
        .absoluteFilePath("aoc_2024_14"));

inline Int modulo(Int value, Int mod) {
  auto result = value % mod;
  if (result < Int{0}) {
    result += mod;
  }
  return result;
}

struct Point {
  Point() = default;
  Point(Int x, Int y) : x{x}, y{y} {}

  Int x{0};
  Int y{0};
};

Point operator+(const Point &lhs, const Point &rhs) {
  return Point(lhs.x + rhs.x, lhs.y + rhs.y);
}

Point operator*(Int scalar, const Point &point) {
  return Point(scalar * point.x, scalar * point.y);
}

class Robot {
public:
  Robot(const QString &input) {
    auto rx = QRegExp("^p=(.+),(.+) v=(.+),(.+)$");
    const auto valid = rx.exactMatch(input);
    if (not valid) {
      common::throwInvalidArgumentError(
          QString("puzzle_2024_14::Robot: cannot parse string \"%1\"")
              .arg(input));
    }
    const auto parse = [](const QString &in, Int &out) {
      auto ok = true;
      out = in.toLongLong(&ok);
      if (not ok) {
        common::throwInvalidArgumentError(
            QString("puzzle_2024_14::Robot: cannot convert string \"%1\" to "
                    "long long int")
                .arg(in));
      }
    };
    parse(rx.cap(1), m_position.x);
    parse(rx.cap(2), m_position.y);
    parse(rx.cap(3), m_velocity.x);
    parse(rx.cap(4), m_velocity.y);
  }

  Point simulate(Int room_width, Int room_height, Int duration) const {
    auto position = m_position + duration * m_velocity;
    position.x = modulo(position.x, room_width);
    position.y = modulo(position.y, room_height);
    return position;
  }

private:
  Point m_position;
  Point m_velocity;
};

using Pixels = std::vector<std::vector<bool>>;

class Pattern {
public:
  Pattern(const QImage &image) {
    static const auto black = QColor(0, 0, 0);
    m_width = static_cast<Int>(image.width());
    m_height = static_cast<Int>(image.height());
    m_pixels = Pixels(m_width, std::vector<bool>(m_height, false));
    for (auto x = 0; x < m_width; ++x) {
      for (auto y = 0; y < m_height; ++y) {
        if (image.pixelColor(x, y) == black) {
          m_pixels[x][y] = true;
        }
      }
    }
  }

  Pattern(Int width, Int height, Pixels &&pixels)
      : m_width{width}, m_height{height}, m_pixels{pixels} {}

  Int width() const { return m_width; }
  Int height() const { return m_height; }
  const Pixels &pixels() const { return m_pixels; }

  bool contains(const Pattern &pattern) const {
    const auto matches = [this, &pattern](Int x_start, Int y_start) {
      for (auto x = x_start; x < x_start + pattern.width(); ++x) {
        for (auto y = y_start; y < y_start + pattern.width(); ++y) {
          if (m_pixels[x][y] != pattern.pixels()[x - x_start][y - y_start]) {
            return false;
          }
        }
      }
      return true;
    };
    for (auto x_start = 0; x_start < m_width - pattern.width(); ++x_start) {
      for (auto y_start = 0; y_start < m_width - pattern.width(); ++y_start) {
        if (matches(x_start, y_start)) {
          return true;
        }
      }
    }
    return false;
  }

private:
  Int m_width;
  Int m_height;
  Pixels m_pixels;
};

class Room {
public:
  Room(const QString &input, Int width, Int height)
      : m_width{width}, m_height{height} {
    const auto lines = common::splitLines(input);
    m_robots.reserve(lines.size());
    for (const auto &line : lines) {
      m_robots.emplace_back(line);
    }
  }

  void
  recordImageAt(Int timestamp,
                const std::optional<QString> &filepath = std::nullopt) const {
    if (not filepath.has_value() and not pictures_dir.exists()) {
      if (not QDir(QStandardPaths::writableLocation(
                       QStandardPaths::PicturesLocation))
                  .mkdir("aoc_2024_14")) {
        common::throwRunTimeError(
            "puzzle_2024_14::Room::recordImageAt: cannot create directory \"" +
            pictures_dir.absolutePath() + "\"");
      }
    }
    auto image = QImage(m_width, m_height, QImage::Format_RGB32);
    image.fill(white);
    const auto positions = simulate(timestamp);
    for (const auto &position : positions) {
      image.setPixelColor(position.x, position.y, black);
    }
    const auto path = filepath.value_or(pictures_dir.absoluteFilePath(
        QString("aoc_2024_14_%1.png").arg(timestamp, 5, 10, QChar('0'))));
    if (not image.save(path, "PNG")) {
      common::throwRunTimeError(
          "puzzle_2024_14::Room::recordImageAt: cannot save image \"" + path +
          "\"");
    }
  }

  void recordImages(Int start, Int duration) const {
    for (auto timestamp = start; timestamp < start + duration; ++timestamp) {
      recordImageAt(timestamp);
    }
  }

  std::vector<Point> simulate(Int duration) const {
    auto positions = std::vector<Point>{};
    positions.reserve(std::size(m_robots));
    for (const auto &robot : m_robots) {
      positions.emplace_back(robot.simulate(m_width, m_height, duration));
    }
    return positions;
  }

  QString solveOne(Int duration) const {
    const auto positions = simulate(duration);
    auto nb_per_quadrant = std::array<std::size_t, 4u>{0u, 0u, 0u, 0u};
    for (const auto &position : positions) {
      const auto index = quadrantIndex(position);
      if (index.has_value()) {
        ++nb_per_quadrant[*index];
      }
    }
    auto product = 1u;
    for (auto nb_robots : nb_per_quadrant) {
      product *= nb_robots;
    }
    return QString("%1").arg(product);
  }

  QString solveTwo(const QImage &image, Int max_duration) const {
    const auto easter_egg_pattern = Pattern(image);
    for (auto duration = 0; duration < max_duration; ++duration) {
      auto pixels = Pixels(m_width, std::vector<bool>(m_height, false));
      for (const auto &robot : m_robots) {
        const auto position = robot.simulate(m_width, m_height, duration);
        pixels[position.x][position.y] = true;
      }
      const auto room_pattern = Pattern(m_width, m_height, std::move(pixels));
      if (room_pattern.contains(easter_egg_pattern)) {
        recordImageAt(duration);
        return QString("%1").arg(duration);
      }
    }
    return "Failure";
  }

private:
  std::optional<ushort> quadrantIndex(const Point &position) const {
    static const auto half_width = m_width / 2;
    static const auto half_height = m_height / 2;
    if (position.x < half_width) {
      if (position.y < half_height) {
        return 0u;
      } else if (position.y > half_height) {
        return 1u;
      }
    } else if (position.x > half_width) {
      if (position.y < half_height) {
        return 2u;
      } else if (position.y > half_height) {
        return 3u;
      }
    }
    return std::nullopt;
  }

  Int m_width;
  Int m_height;
  std::vector<Robot> m_robots{};
};

} // namespace puzzle_2024_14

void Solver_2024_14_1::solve(const QString &input) {
  const auto room = puzzle_2024_14::Room(input, 101, 103);
  emit finished(room.solveOne(100));
}

void Solver_2024_14_2::solve(const QString &input) {
  const auto room = puzzle_2024_14::Room(input, 101, 103);
  const auto easter_egg_pattern =
      QImage(":/2024/puzzle_2024_14_easter_egg_pattern.png", "PNG");
  emit finished(room.solveTwo(easter_egg_pattern, 10000));
}
