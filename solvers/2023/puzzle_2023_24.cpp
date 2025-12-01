#include <solvers/common.h>
#include <eigen3/Eigen/LU>
#include <solvers/2023/puzzle_2023_24.h>

#include <QFile>
#include <QTextStream>

namespace puzzle_2023_24 {

constexpr auto epsilon = 1.e-3;

inline void set3DCoordinates(const QString &input,
                             Eigen::Vector3d &coordinates) {
  const auto splitted = common::splitValues(input, ',');
  if (splitted.size() != 3)
    common::throwInvalidArgumentError(
        QString("set3DCoordinates: wrong dimension (%1)").arg(splitted.size()));
  for (auto i = 0; i < 3; ++i) {
    auto ok = true;
    coordinates(i) = splitted[i].toDouble(&ok);
    if (not ok)
      common::throwInvalidArgumentError(
          QString("set3DCoordinates: cannot cast \"%1\" to double")
              .arg(splitted[i]));
  }
}

class HailStone {
public:
  HailStone(QString input) {
    input.remove(' ');
    const auto splitted = common::splitValues(input, '@');
    if (splitted.size() != 2)
      common::throwInvalidArgumentError(
          QString("HailStone::constructor: bad split size (%1)")
              .arg(splitted.size()));
    set3DCoordinates(splitted[0], m_position);
    set3DCoordinates(splitted[1], m_velocity);
  }

  const Eigen::Vector3d &position() const { return m_position; }
  const Eigen::Vector3d &velocity() const { return m_velocity; }

  Eigen::Vector3d getPositionAtTime(double t) const {
    return m_position + t * m_velocity;
  }

  QString toString() const {
    return QString("%1, %2, %3 @ %4, %5, %6")
        .arg(static_cast<int>(m_position(0)))
        .arg(static_cast<int>(m_position(1)))
        .arg(static_cast<int>(m_position(2)))
        .arg(static_cast<int>(m_velocity(0)))
        .arg(static_cast<int>(m_velocity(1)))
        .arg(static_cast<int>(m_velocity(2)));
  }

private:
  Eigen::Vector3d m_position;
  Eigen::Vector3d m_velocity;
};

inline std::optional<Eigen::Vector2d>
get2DCollisionPointCoefficients(const HailStone &A, const HailStone &B) {
  Eigen::Matrix2d M;
  M << A.velocity()(0), -B.velocity()(0), A.velocity()(1), -B.velocity()(1);
  if (std::abs(M.determinant()) < epsilon)
    return std::nullopt;
  Eigen::Vector2d AB(B.position()(0) - A.position()(0),
                     B.position()(1) - A.position()(1));
  return M.inverse() * AB;
}

class HailStones {
public:
  HailStones(const QString &input) {
    const auto lines = common::splitLines(input);
    m_stones.reserve(lines.size());
    for (const auto &line : lines)
      m_stones.emplace_back(line);
  }

  QString solveOne(const double test_area_min,
                   const double test_area_max) const {
    auto nb_collisions = 0u;
    for (auto i = 0u; i < m_stones.size(); ++i) {
      for (auto j = i + 1u; j < m_stones.size(); ++j) {
        const auto coeffs_opt =
            get2DCollisionPointCoefficients(m_stones[i], m_stones[j]);
        if (coeffs_opt.has_value()) {
          const Eigen::Vector2d &coeffs = *coeffs_opt;
          if (coeffs(0) >= 0. and coeffs(1) >= 0.) {
            Eigen::Vector3d P =
                m_stones[i].position() + coeffs(0) * m_stones[i].velocity();
            if (P(0) >= test_area_min and P(0) <= test_area_max and
                P(1) >= test_area_min and P(1) <= test_area_max)
              ++nb_collisions;
          }
        }
      }
    }
    return QString("%1").arg(nb_collisions);
  }

  QString solveTwo(const uint N) const {
    QFile file("/home/aboeuf/shared/test_aoc/day_24.obj");
    if (not file.open(QIODevice::WriteOnly | QIODevice::Text))
      return "Cannot open file";
    QTextStream out(&file);
    for (auto i = 0u; i < m_stones.size(); ++i) {
      for (auto t = 0u; t < N; ++t) {
        const Eigen::Vector3d P =
            m_stones[i].getPositionAtTime(static_cast<double>(t));
        out << QString("v %1 %2 %3\n").arg(P(0)).arg(P(1)).arg(P(2));
      }
    }
    auto index = 1u;
    for (auto i = 0u; i < m_stones.size(); ++i) {
      for (auto t = 1u; t < N; ++t) {
        out << QString("l %1 %2\n").arg(index).arg(index + 1u);
        ++index;
      }
      ++index;
    }
    return "Done";
  }

private:
  std::vector<HailStone> m_stones;
};

} // namespace puzzle_2023_24

void Solver_2023_24_1::solve(const QString &input) {
  const auto stones = puzzle_2023_24::HailStones{input};
  emit finished(stones.solveOne(200000000000000., 400000000000000.));
}

void Solver_2023_24_2::solve(const QString &input) {
  const auto stones = puzzle_2023_24::HailStones{input};
  emit finished(stones.solveTwo(8u));
}
