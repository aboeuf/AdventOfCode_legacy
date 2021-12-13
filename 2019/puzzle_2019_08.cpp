#include <2019/puzzle_2019_08.h>
#include <common.h>
#include <QDebug>

constexpr int width = 25;
constexpr int height = 6;

struct Layer : public QVector<QVector<uint>>
{
  Layer()
  {
    QVector<uint> row;
    row.fill(0, width);
    this->fill(row, height);
  }

  uint nbOccurences(int value) const
  {
    uint sum = 0;
    for (int i = 0; i < this->size(); ++i)
      for (int j = 0; j < this->at(i).size(); ++j)
        if (this->at(i).at(j) == value)
          ++sum;
    return sum;
  }

  void display(Display* display) const
  {
    if (not display)
      return;
    display->scene()->clear();
    QPen pen;
    pen.setWidthF(0.01);
    QBrush brush(QColor("Black"));
    for (int i = 0; i < size(); ++i) {
      for (int j = 0; j < (*this)[i].size(); ++j) {
        if ((*this)[i][j] == 1) {
          display->scene()->addRect(j, -i, 1.0, 1.0, pen, brush);
        }
      }
    }
    display->show();
    display->fit();
  }

  uint m_index{0};
};

struct Image
{
  Image(const QString& input)
  {
    m_layers.fill(Layer{}, input.size() / (width * height));
    for (int i = 0; i < m_layers.size(); ++i)
      m_layers[i].m_index = i;
    int i = 0, j = 0, k = 0;
    for (const QChar& c : input) {
      if (k >= width) {
        k = 0;
        ++j;
        if (j >= height) {
          j = 0;
          ++i;
        }
      }
      m_layers[i][j][k] = QString(c).toInt();
      ++k;
    }
  }

  int getMinLayer(int value) const
  {
    int min_index = 0;
    int min_occurences = m_layers[0].nbOccurences(value);
    for (int i = 1; i < m_layers.size(); ++i) {
      int nb_occurences = m_layers[i].nbOccurences(value);
      if (nb_occurences < min_occurences) {
        min_occurences = nb_occurences;
        min_index = i;
      }
    }
    return min_index;
  }

  uint puzzle_1() const
  {
    int min_layer = getMinLayer(0);
    return m_layers[min_layer].nbOccurences(1) *
        m_layers[min_layer].nbOccurences(2);
  }

  Layer decode() const
  {
    Layer decoded;
    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        int k = 0;
        for (; k < m_layers.size() && m_layers[k][i][j] == 2; ++k) {}
        decoded[i][j] = k < m_layers.size() ? m_layers[k][i][j] : 2;
      }
    }
    return decoded;
  }

  QVector<Layer> m_layers;
};

void Solver_2019_08_1::solve(const QString& input)
{
  Image image(input);
  emit finished(QString::number(image.puzzle_1()));
}

void Solver_2019_08_2::solve(const QString& input)
{
  if (not m_display) {
    emit finished("No display");
    return;
  }
  Image image(input);
  Layer decoded = image.decode();
  decoded.display(m_display);
  emit finished("");
}

