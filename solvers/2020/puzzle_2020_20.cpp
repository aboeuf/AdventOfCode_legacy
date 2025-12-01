#include <QDebug>
#include <cmath>
#include <solvers/2020/puzzle_2020_20.h>
#include <solvers/common.h>

namespace puzzle_2020_20 {

using Int = unsigned long long int;

enum Rotation { R_0, R_90, R_180, R_270 };

class Transformation {
public:
  Transformation() = default;
  Transformation(bool flip, const Rotation &r) : m_flip{flip}, m_rotation{r} {}
  Transformation(const QString &t) {
    if (t.size() > 1)
      m_flip = t[1] == '1';
    m_rotation = R_0;
    if (t.mid(3) == "90")
      m_rotation = R_90;
    else if (t.mid(3) == "180")
      m_rotation = R_180;
    else if (t.mid(3) == "270")
      m_rotation = R_270;
  }

  bool flip() const { return m_flip; }
  const Rotation &rotation() const { return m_rotation; }

  bool operator==(const Transformation &other) const {
    return m_flip == other.flip() && m_rotation == other.rotation();
  }

  QString toString() const {
    QString rot{"0"};
    if (m_rotation == R_90)
      rot = "90";
    else if (m_rotation == R_180)
      rot = "180";
    else if (m_rotation == R_270)
      rot = "270";
    return QString("F%1R%2").arg(m_flip ? 1 : 0).arg(rot);
  }

private:
  bool m_flip{false};
  Rotation m_rotation{R_0};
};

class Row {
public:
  Row() = default;
  Row(const QString &row) {
    m_data.reserve(row.size());
    for (const QChar &c : row)
      m_data.push_back(c);
  }

  bool empty() const { return m_data.empty(); }
  std::size_t size() const { return m_data.size(); }

  const QChar &operator[](std::size_t i) const {
    if (i < m_data.size())
      return m_data[i];
    return m_error_char;
  }

  QChar &operator[](std::size_t i) {
    if (i < m_data.size())
      return m_data[i];
    return m_error_char;
  }

  const QChar &front() const { return m_data.front(); }
  QChar &front() { return m_data.front(); }

  const QChar &back() const { return m_data.back(); }
  QChar &back() { return m_data.back(); }

  void resize(std::size_t asize) { m_data.resize(asize); }

  void removeBorders() {
    for (auto i = 1u; i < m_data.size(); ++i)
      m_data[i - 1] = m_data[i];
    m_data.pop_back();
    m_data.pop_back();
    m_data.shrink_to_fit();
  }

private:
  std::vector<QChar> m_data{};
  QChar m_error_char{'@'};
};

const std::vector<QString> monster = {
    "..................#.", "#....##....##....###", ".#..#..#..#..#..#..."};

class TileData {
public:
  TileData() { m_data.reserve(10); }

  TileData(const TileData &other) {
    m_data = other.data();
    m_width = other.width();
  }

  bool empty() const { return m_data.empty() || m_width == 0; }
  std::size_t length() const { return m_data.size(); }
  std::size_t width() const { return m_width; }

  void operator<<(const QString &row) {
    std::size_t row_size = static_cast<std::size_t>(row.size());
    m_width = m_data.empty() ? row_size : std::min(row_size, m_width);
    m_data.emplace_back(row);
  }

  const Row &operator[](std::size_t i) const {
    if (i < m_data.size())
      return m_data[i];
    return m_error_row;
  }

  Row &operator[](std::size_t i) {
    if (i < m_data.size())
      return m_data[i];
    return m_error_row;
  }

  void flip() {
    std::vector<Row> data{};
    data.resize(length());
    for (auto i = 0u; i < length(); ++i) {
      data[i].resize(width());
      for (auto j = 0u; j < width(); ++j)
        data[i][j] = m_data[length() - i - 1][j];
    }
    std::swap(m_data, data);
  }

  void rotate(const Rotation &r) {
    if (r == R_0)
      return;
    std::vector<Row> data{};
    data.resize(length());
    if (r == R_90) {
      for (auto i = 0u; i < length(); ++i) {
        data[i].resize(width());
        for (auto j = 0u; j < width(); ++j)
          data[i][j] = m_data[j][length() - i - 1];
      }
    } else if (r == R_180) {
      for (auto i = 0u; i < length(); ++i) {
        data[i].resize(width());
        for (auto j = 0u; j < width(); ++j)
          data[i][j] = m_data[length() - i - 1][width() - j - 1];
      }
    } else if (r == R_270) {
      for (auto i = 0u; i < length(); ++i) {
        data[i].resize(width());
        for (auto j = 0u; j < width(); ++j)
          data[i][j] = m_data[width() - j - 1][i];
      }
    } else
      return;
    std::swap(m_data, data);
  }

  TileData transformed(const QString &t_str) const {
    const Transformation t{t_str};
    TileData data{*this};
    if (t.flip())
      data.flip();
    data.rotate(t.rotation());
    return data;
  }

  void transform(const QString &t_str) {
    const Transformation t{t_str};
    if (t.flip())
      flip();
    rotate(t.rotation());
  }

  void transformBack(const QString &t_str) {
    const Transformation t{t_str};
    if (t.rotation() == R_90)
      rotate(R_270);
    else if (t.rotation() == R_180)
      rotate(R_180);
    else if (t.rotation() == R_270)
      rotate(R_90);
    if (t.flip())
      flip();
  }

  const Row &front() const { return m_data.front(); }
  Row &front() { return m_data.front(); }

  const Row &back() const { return m_data.back(); }
  Row &back() { return m_data.back(); }

  void debug() const {
    for (auto i = 0u; i < length(); ++i) {
      QString row;
      for (auto j = 0u; j < width(); ++j)
        row.push_back(m_data[i][j]);
      qDebug() << row.toStdString().c_str();
    }
  }

  void removeBorders() {
    for (auto i = 1u; i < m_data.size(); ++i) {
      m_data[i].removeBorders();
      m_data[i - 1] = m_data[i];
    }
    m_data.pop_back();
    m_data.pop_back();
    m_data.shrink_to_fit();
    m_width -= 2;
  }

  void print(std::size_t i, std::size_t j) const {
    for (auto k = 0u; k < monster.size(); ++k) {
      if (i + k < m_data.size()) {
        QString s;
        for (auto l = 0u; l < static_cast<std::size_t>(monster[k].size());
             ++l) {
          if (j + l < m_data[i + k].size()) {
            s.push_back(m_data[i + k][j + l]);
          }
        }
        qDebug() << s;
      }
    }
    qDebug() << "";
  }

  void markMonster(std::size_t i, std::size_t j, TileData &copy) const {
    QList<QPair<std::size_t, std::size_t>> indexes;
    for (auto k = 0u; k < monster.size(); ++k) {
      if (i + k >= m_data.size())
        return;
      for (auto l = 0u; l < static_cast<std::size_t>(monster[k].size()); ++l) {
        if (j + l >= m_data[i + k].size())
          return;
        if (monster[k][static_cast<int>(l)] == '#') {
          if (m_data[i + k][j + l] == '#')
            indexes << QPair<std::size_t, std::size_t>{i + k, j + l};
          else {
            // print(i, j);
            return;
          }
        }
      }
    }
    qDebug() << "All good";
    qDebug() << indexes;
    for (auto &index : indexes)
      copy[index.first][index.second] = 'O';
    copy.debug();
  }

  const std::vector<Row> &data() const { return m_data; }

protected:
  std::size_t m_width{0};
  std::vector<Row> m_data{};
  Row m_error_row{};
};

enum MatchDirection { East, North, West, South };

MatchDirection twinDirection(const MatchDirection &dir) {
  if (dir == East)
    return West;
  if (dir == North)
    return South;
  if (dir == West)
    return East;
  return North;
}

QString dirToString(const MatchDirection &dir) {
  if (dir == East)
    return "E";
  if (dir == North)
    return "N";
  if (dir == West)
    return "W";
  return "S";
}

const std::vector<Transformation> transformations = {
    Transformation{"F0R0"},   Transformation{"F0R90"}, Transformation{"F0R180"},
    Transformation{"F0R270"}, Transformation{"F1R0"},  Transformation{"F1R90"},
    Transformation{"F1R180"}, Transformation{"F1R270"}};

class Tile;
using Candidates = QMap<QString, QMap<MatchDirection, QMap<Tile *, QString>>>;

class Tile {
public:
  Tile() = default;
  Tile(Int index) : m_index{index} {}

  bool empty() const { return m_data.length() == 0 || m_data.width() == 0; }
  Int index() const { return m_index; }

  void operator<<(const QString &row) { m_data << row; }

  TileData transformed(const QString &t) const { return m_data.transformed(t); }
  void transform(const QString &t) { m_data.transform(t); }

  const Row &operator[](std::size_t i) const { return m_data[i]; }
  Row &operator[](std::size_t i) { return m_data[i]; }

  void debug() const {
    qDebug() << "Tile" << m_index << "|" << maxNbCandidates();
    for (const QString &t : m_candidates.keys()) {
      QString cand = t + ": ";
      const QMap<MatchDirection, QMap<Tile *, QString>> &candidates =
          m_candidates[t];
      for (const MatchDirection &dir : candidates.keys()) {
        cand += "[" + dirToString(dir) + ": {";
        const QMap<Tile *, QString> &by_dir = candidates[dir];
        for (Tile *tile : by_dir.keys()) {
          const QString &tt = by_dir[tile];
          cand += "(" + QString::number(tile->index()) + ", " + tt + "), ";
        }
        if (cand.endsWith(", "))
          cand.chop(2);
        cand += "}], ";
      }
      if (cand.endsWith(", "))
        cand.chop(2);
      cand += "}";
      qDebug() << cand.toStdString().c_str();
    }
    m_data.debug();
  }

  QList<MatchDirection> match(const Tile &other, const Transformation &t_this,
                              const Transformation &t_other) const {
    QList<MatchDirection> result;
    if (empty() || other.empty())
      return result;
    const TileData this_data = transformed(t_this.toString());
    const TileData other_data = other.transformed(t_other.toString());
    bool match = true;
    for (auto i = 0u; match && i < this_data.length(); ++i)
      match = this_data[i].back() == other_data[i].front();
    if (match)
      result << East;
    match = true;
    for (auto i = 0u; match && i < this_data.width(); ++i)
      match = this_data.front()[i] == other_data.back()[i];
    if (match)
      result << North;
    match = true;
    for (auto i = 0u; match && i < this_data.length(); ++i)
      match = this_data[i].front() == other_data[i].back();
    if (match)
      result << West;
    match = true;
    for (auto i = 0u; match && i < this_data.width(); ++i)
      match = this_data.back()[i] == other_data.front()[i];
    if (match)
      result << South;
    return result;
  }

  void
  addCandidate(const MatchDirection &dir, Tile *tile,
               const QPair<Transformation, Transformation> transformations) {
    if (tile)
      m_candidates[transformations.first.toString()][dir][tile] =
          transformations.second.toString();
  }

  void setCandidates(Tile &other) {
    for (const Transformation &t_this : transformations) {
      for (const Transformation &t_other : transformations) {
        const QList<MatchDirection> directions = match(other, t_this, t_other);
        for (const MatchDirection &dir : directions) {
          addCandidate(dir, &other, {t_this, t_other});
          other.addCandidate(twinDirection(dir), this, {t_other, t_this});
        }
      }
    }
  }

  int maxNbCandidates() const {
    if (m_candidates.empty())
      return 0;
    int n = 0;
    for (auto &cand : m_candidates.values())
      n = std::max(cand.size(), n);
    return n;
  }

  const Candidates &candidates() const { return m_candidates; }

  const TileData &data() const { return m_data; }

  void removeBorders() { m_data.removeBorders(); }
  const

      Tile *m_east{nullptr};
  Tile *m_north{nullptr};
  Tile *m_west{nullptr};
  Tile *m_south{nullptr};

private:
  Int m_index{0};
  TileData m_data{};
  Candidates m_candidates{};
};

class Image {
public:
  Image(const QString &input) {
    QRegExp rx("Tile (\\d+):");
    Int index{0};
    const QStringList lines = common::splitLines(input);
    for (const QString &line : lines) {
      if (rx.indexIn(line) == 0) {
        index = rx.capturedTexts()[1].toULongLong();
        m_tiles << Tile(index);
        m_by_index[index] = &m_tiles.back();
      } else if (!line.isEmpty() && !m_tiles.isEmpty())
        m_tiles.back() << line;
    }
    for (auto ti = m_tiles.begin(); ti != m_tiles.end(); ++ti)
      for (auto tj = std::next(ti); tj != m_tiles.end(); ++tj)
        ti->setCandidates(*tj);
    int asize =
        static_cast<int>(std::sqrt(static_cast<double>(m_tiles.size())));
    m_image.resize(asize);
    for (int i = 0; i < asize; ++i)
      m_image[i].resize(asize);
    reset();
    for (Tile &t : m_tiles)
      if (validUpLeftCorner(&t))
        break;
    for (int i = 0; i < m_image.size(); ++i) {
      for (int j = 0; j < m_image[i].size(); ++j) {
        Tile *t = m_image[i][j].first;
        if (t) {
          t->transform(m_image[i][j].second);
          if (i > 0)
            t->m_north = m_image[i - 1][j].first;
          if (i + 1 < m_image.size())
            t->m_south = m_image[i + 1][j].first;
          if (j > 0)
            t->m_west = m_image[i][j - 1].first;
          if (j + 1 < m_image[i].size())
            t->m_east = m_image[i][j + 1].first;
        }
      }
    }
  }

  const QVector<QVector<QPair<Tile *, QString>>> &image() const {
    return m_image;
  }

  void debug() const {
    for (const Tile &tile : m_tiles) {
      tile.debug();
      qDebug() << "";
    }
    qDebug() << "Image" << m_image.size();
    for (int i = 0; i < m_image.size(); ++i) {
      QString row;
      for (int j = 0; j < m_image[i].size(); ++j)
        row += " " + (m_image[i][j].first
                          ? QString::number(m_image[i][j].first->index())
                          : QString("null"));
      qDebug() << row.toStdString().c_str();
    }
    qDebug() << "Image" << m_image.size();
    for (int i = 0; i < m_image.size(); ++i) {
      QString row;
      for (int j = 0; j < m_image[i].size(); ++j)
        row += " " +
               (m_image[i][j].first ? m_image[i][j].second : QString("null"));
      qDebug() << row.toStdString().c_str();
    }
  }

  void debug2() const {
    int n = static_cast<int>(m_image[0][0].first->data().length());
    for (auto i = 0; i < m_image.size(); ++i) {
      for (auto tile_row = 0; tile_row < n; ++tile_row) {
        QString s;
        for (auto j = 0; j < m_image[i].size(); ++j) {
          s.push_back(' ');
          for (auto k = 0u; k < m_image[i][j].first->data()[tile_row].size();
               ++k)
            s.push_back(m_image[i][j].first->data()[tile_row][k]);
        }
        qDebug() << s.toStdString().c_str();
      }
      qDebug() << "";
    }
  }

  QString puzzle_1() const {
    if (m_image.isEmpty())
      return "Empty image";
    Int product = 1;
    for (int i = 0; i < m_image.size(); i += m_image.size() - 1) {
      if (m_image[i].isEmpty())
        return QString("Empty row %1").arg(i);
      for (int j = 0; j < m_image[i].size(); j += m_image[i].size() - 1) {
        if (!m_image[i][j].first)
          return QString("Null tile (%1, %2)").arg(i).arg(j);
        product *= m_image[i][j].first->index();
      }
    }
    return QString::number(product);
  }

  void removeBorders() {
    for (Tile &tile : m_tiles)
      tile.removeBorders();
  }

private:
  void reset(int i, int j) {
    if (i >= 0 && i < m_image.size() && j >= 0 && j < m_image.size()) {
      m_image[i][j].first = nullptr;
      m_image[i][j].second.clear();
    }
  }

  void reset() {
    for (int i = 0; i < m_image.size(); ++i)
      for (int j = 0; j < m_image[i].size(); ++j)
        reset(i, j);
  }

  bool set(int i, int j) {
    if (i == 0 && j == 0)
      return true;
    if (i < 0 || j < 0 || i >= m_image.size() || j >= m_image.size())
      return false;
    if (j != 0) {
      Tile *west = m_image[i][j - 1].first;
      if (!west)
        return false;
      const Candidates &west_C = west->candidates();
      const QString &west_t = m_image[i][j - 1].second;
      if (!west_C[west_t].contains(East))
        return false;
      if (west_C[west_t][East].size() != 1)
        return false;
      m_image[i][j].first = west_C[west_t][East].keys().front();
      m_image[i][j].second = west_C[west_t][East].values().front();
      if (i != 0) {
        Tile *north = m_image[i - 1][j].first;
        if (!north)
          return false;
        const Candidates &north_C = north->candidates();
        const QString &north_t = m_image[i - 1][j].second;
        if (!north_C[north_t].contains(South))
          return false;
        if (north_C[north_t][South].size() != 1)
          return false;
        if (north_C[north_t][South].keys().front() != m_image[i][j].first)
          return false;
        if (north_C[north_t][South].values().front() != m_image[i][j].second)
          return false;
      }
    } else if (i != 0) {
      Tile *north = m_image[i - 1][j].first;
      if (!north)
        return false;
      const Candidates &north_C = north->candidates();
      const QString &north_t = m_image[i - 1][j].second;
      if (!north_C[north_t].contains(South))
        return false;
      if (north_C[north_t][South].size() != 1)
        return false;
      m_image[i][j].first = north_C[north_t][South].keys().front();
      m_image[i][j].second = north_C[north_t][South].values().front();
    }
    return true;
  }

  bool validUpLeftCorner(Tile *tile) {
    reset();
    if (!tile || tile->maxNbCandidates() > 2)
      return false;
    for (const QString &t : tile->candidates().keys()) {
      reset();
      m_image[0][0].first = tile;
      m_image[0][0].second = t;
      bool valid = true;
      for (int i = 0; valid && i < m_image.size(); ++i)
        for (int j = 0; valid && j < m_image.size(); ++j)
          valid = set(i, j);
      if (!valid)
        continue;
      QSet<Tile *> tiles;
      for (int i = 0; i < m_image.size(); ++i)
        for (int j = 0; j < m_image.size(); ++j)
          tiles.insert(m_image[i][j].first);
      if (tiles.size() == m_tiles.size())
        return true;
    }
    reset();
    return false;
  }

  QList<Tile> m_tiles;
  QMap<Int, Tile *> m_by_index;
  QVector<QVector<QPair<Tile *, QString>>> m_image;
};

class CleanImage : public TileData {
public:
  CleanImage(const Image &image) : TileData() {
    int n = static_cast<int>(image.image()[0][0].first->data().length());
    int N = n * image.image().size();
    m_data.resize(N);
    m_width = N;
    for (auto i = 0; i < N; ++i)
      m_data[i].resize(N);
    for (auto i = 0; i < image.image().size(); ++i) {
      for (auto tile_row = 0; tile_row < n; ++tile_row) {
        for (auto j = 0; j < image.image()[i].size(); ++j)
          for (auto k = 0u;
               k < image.image()[i][j].first->data()[tile_row].size(); ++k) {
            m_data[n * i + tile_row][n * j + k] =
                image.image()[i][j].first->data()[tile_row][k];
          }
      }
    }
  }

  CleanImage(const CleanImage &other) : TileData() {
    m_data = other.data();
    m_width = other.width();
  }

  CleanImage findMonster() const {
    CleanImage copy{*this};
    for (const Transformation &t : transformations) {
      const TileData trans = transformed(t.toString());
      copy.transform(t.toString());
      for (auto i = 0u; i < trans.length(); ++i)
        for (auto j = 0u; j < trans.width(); ++j)
          trans.markMonster(i, j, copy);
      copy.transformBack(t.toString());
    }
    return copy;
  }

  Int nbSharps() const {
    Int count = 0;
    for (auto i = 0u; i < length(); ++i)
      for (auto j = 0u; j < width(); ++j)
        if (m_data[i][j] == '#')
          ++count;
    return count;
  }
};

} // namespace puzzle_2020_20

void Solver_2020_20_1::solve(const QString &input) {
  using namespace puzzle_2020_20;
  Image im(input);
  emit finished(im.puzzle_1());
}

void Solver_2020_20_2::solve(const QString &input) {
  using namespace puzzle_2020_20;
  Image im(input);
  im.removeBorders();
  CleanImage cleaned(im);
  CleanImage monsters = cleaned.findMonster();
  emit finished(QString::number(monsters.nbSharps()));
}
