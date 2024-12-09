#include <2016/puzzle_2016_03.h>
#include <common.h>

namespace puzzle_2016_03 {

class Triangle {
public:
  Triangle(const QString &input) {
    const auto values = common::toUInt(input, ' ');
    for (auto i = 0; i < 3; ++i) {
      m_side_lengths[i] = values[i];
    }
  }

  Triangle(uint A, uint B, uint C) {
    m_side_lengths[0] = A;
    m_side_lengths[1] = B;
    m_side_lengths[2] = C;
  }

  bool isValid() const {
    for (auto i = 0; i < 3; ++i) {
      for (auto j = 0; j < 2; ++j) {
        if (m_side_lengths[i] >=
            m_side_lengths[(i + j) % 3u] + m_side_lengths[(i + j + 1u) % 3u]) {
          return false;
        }
      }
    }
    return true;
  }

private:
  std::array<uint, 3u> m_side_lengths;
};

class TrianglesV1 {
public:
  TrianglesV1() = default;

  TrianglesV1(const QString &input) {
    const auto lines = common::splitLines(input);
    m_triangles.reserve(lines.size());
    for (const auto &line : lines)
      m_triangles.emplace_back(line);
  }

  virtual ~TrianglesV1() = default;

  QString solve() const {
    auto nb_valid = 0u;
    for (const auto &triangle : m_triangles) {
      if (triangle.isValid()) {
        ++nb_valid;
      }
    }
    return QString("%1").arg(nb_valid);
  }

protected:
  std::vector<Triangle> m_triangles;
};

class TrianglesV2 : public TrianglesV1 {
public:
  TrianglesV2(const QString &input) : TrianglesV1{} {
    const auto lines = common::splitLines(input);
    m_triangles.reserve(lines.size());
    for (auto i = 0; i + 2 < lines.size(); i += 3) {
      const auto A = common::toUInt(lines[i], ' ');
      const auto B = common::toUInt(lines[i + 1], ' ');
      const auto C = common::toUInt(lines[i + 2], ' ');
      for (auto j = 0; j < 3; ++j) {
        m_triangles.emplace_back(A[j], B[j], C[j]);
      }
    }
  }
};

} // namespace puzzle_2016_03

void Solver_2016_03_1::solve(const QString &input) {
  const auto triangles = puzzle_2016_03::TrianglesV1{input};
  emit finished(triangles.solve());
}

void Solver_2016_03_2::solve(const QString &input) {
  const auto triangles = puzzle_2016_03::TrianglesV2{input};
  emit finished(triangles.solve());
}
