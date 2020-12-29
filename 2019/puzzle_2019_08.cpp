#include <2019/puzzle_2019_08.h>
#include <2019/display/puzzle_2019_08/puzzle_2019_08_display.h>
#include <common.h>
#include <QDebug>

namespace puzzle_2019_08
{

constexpr int width = 25;
constexpr int height = 6;

Layer::Layer()
{
  QVector<uint> row;
  row.fill(0, width);
  this->fill(row, height);
}

uint Layer::nbOccurences(int value) const
{
  uint sum = 0;
  for (int i = 0; i < this->size(); ++i)
    for (int j = 0; j < this->at(i).size(); ++j)
      if (this->at(i).at(j) == value)
        ++sum;
  return sum;
}

void Layer::debug() const
{
  qDebug() << "Layer" << m_index;
  for (int i = 0; i < this->size(); ++i) {
    QString s;
    for (int j = 0; j < this->at(i).size(); ++j)
      s += QString::number(this->at(i).at(j));
    qDebug() << s.toStdString().c_str();
  }
  qDebug() << "";
}

QString Layer::toString() const
{
  QString s;
  for (int i = 0; i < this->size(); ++i) {
    for (int j = 0; j < this->at(i).size(); ++j)
      s += this->at(i).at(j) == 1 ? "0" : "_";
    s += "\n";
  }
  return s;
}

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

  void debug() const
  {
    for (const Layer& layer : m_layers)
      layer.debug();
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


} // namepspace puzzle_2019_08

void Solver_2019_08_1::solve(const QString& input)
{
  using namespace puzzle_2019_08;
  Image image(input);
  emit finished(QString::number(image.puzzle_1()));
}

void Solver_2019_08_2::solve(const QString& input)
{
  using namespace puzzle_2019_08;
  Image image(input);
  Layer decoded = image.decode();
  Puzzle_2019_08_Display* display = new Puzzle_2019_08_Display(decoded);
  display->show();
  emit finished(decoded.toString());
}

