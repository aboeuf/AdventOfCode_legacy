#include <2018/puzzle_2018_08.h>
#include <common.h>

namespace puzzle_2018_08 {

class Data {
public:
  Data(const QString &input) : m_data{common::toUInt(input, ' ')} {}

  uint at(const uint index) const {
    const auto casted = static_cast<int>(index);
    if (casted >= m_data.size()) {
      common::throwInvalidArgumentError(
          QString(
              "puzzle_2018_08::Data::at: index %1 is out of range (size is %2)")
              .arg(casted)
              .arg(m_data.size()));
    }
    return m_data.at(casted);
  }

private:
  QVector<uint> m_data;
};

class Node {
public:
  Node(const Data &data, uint start = 0u) : m_data{data}, m_start{start} {
    m_nb_children = m_data.at(start);
    m_nb_metadata_entries = m_data.at(start + 1u);
    auto current = start + 2u;
    for (auto i = 0u; i < m_nb_children; ++i) {
      m_children << new Node(m_data, current);
      current = m_children.back()->stop();
    }
    m_metadata_start = current;
    m_stop = m_metadata_start + m_nb_metadata_entries;
  }

  ~Node() {
    for (auto *child : m_children) {
      delete child;
    }
  }

  const QList<Node *> &children() const { return m_children; }

  uint nbChildren() const { return m_nb_children; }
  uint nbMetadataEntries() const { return m_nb_metadata_entries; }

  uint start() const { return m_start; }
  uint stop() const { return m_stop; }
  uint metaDataStart() const { return m_metadata_start; }

  uint metaDataSum() const {
    auto sum = 0u;
    for (auto i = m_metadata_start; i < m_stop; ++i) {
      sum += m_data.at(i);
    }
    for (const auto *child : m_children) {
      sum += child->metaDataSum();
    }
    return sum;
  }

  uint value() const {
    if (m_nb_children == 0u) {
      return metaDataSum();
    }
    auto sum = 0u;
    for (auto i = m_metadata_start; i < m_stop; ++i) {
      const auto index = m_data.at(i);
      if (index > 0u and index <= m_nb_children) {
        sum += m_children.at(static_cast<int>(index - 1u))->value();
      }
    }
    return sum;
  }

private:
  const Data &m_data;
  QList<Node *> m_children;
  uint m_nb_children;
  uint m_nb_metadata_entries;
  uint m_start;
  uint m_stop;
  uint m_metadata_start;
};

class Tree {
public:
  Tree(const QString &input)
      : m_data{common::splitLines(input, true).front()}, m_root{m_data} {}

  QString solveOne() const { return QString("%1").arg(m_root.metaDataSum()); }
  QString solveTwo() const { return QString("%1").arg(m_root.value()); }

private:
  Data m_data;
  Node m_root;
};

} // namespace puzzle_2018_08

void Solver_2018_08_1::solve(const QString &input) {
  const auto tree = puzzle_2018_08::Tree(input);
  emit finished(tree.solveOne());
}

void Solver_2018_08_2::solve(const QString &input) {
  const auto tree = puzzle_2018_08::Tree(input);
  emit finished(tree.solveTwo());
}
