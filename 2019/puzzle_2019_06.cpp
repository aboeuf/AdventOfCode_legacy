#include <2019/puzzle_2019_06.h>
#include <common.h>
#include <QMap>
#include <QSet>
#include <QVector>

namespace puzzle_2019_06 {

struct Node
{
  Node() = default;
  Node(const QString& label) : m_label{label} {}

  void addChild(Node* node)
  {
    if (node) {
      node->m_parent = this;
      m_children << node;
    }
  }

  void setDepth()
  {
    m_depth = m_parent ? m_parent->m_depth + 1 : 0;
    for (Node* child : m_children)
      child->setDepth();
  }

  QVector<Node*> ancestors() const
  {
    QVector<Node*> result;
    result.reserve(m_depth);
    Node *current = m_parent;
    while (current) {
      result << current;
      current = current->m_parent;
    }
    return result;
  }

  uint m_depth{0};
  Node* m_parent{nullptr};
  QSet<Node*> m_children{};
  QString m_label{};
};

struct Tree
{
  Tree(const QString& input)
  {
    const QStringList lines = common::splitLines(input);
    QVector<QStringList> orbits;
    orbits.reserve(lines.size());
    for (const QString& line : lines) {
      const QStringList labels = line.split(')');
      if (labels.size() == 2) {
        for (const QString& label : labels)
          if (!m_nodes.contains(label))
            m_nodes[label] = new Node(label);
        orbits << labels;
      }
    }
    for (const QStringList& orbit : orbits)
      m_nodes[orbit[0]]->addChild(m_nodes[orbit[1]]);
    m_root = m_nodes.value("COM", nullptr);
    if (m_root)
      m_root->setDepth();
    for (Node* node : m_nodes.values()) {
      if (node->m_children.empty()) {
        m_leaves << node;
        m_depth_max = std::max(m_depth_max, node->m_depth);
      }
    }
  }

  ~Tree()
  {
    for (Node* node : m_nodes.values())
      delete node;
  }

  uint getNbOrbits() const
  {
    uint sum = 0;
    for (const Node* node : m_nodes.values())
      sum += node->m_depth;
    return sum;
  }

  uint getNbOrbitalTransfers(const QString& A, const QString& B) const
  {
    Node* a = m_nodes.value(A, nullptr);
    if (!a)
      return std::numeric_limits<uint>::max();
    Node* b = m_nodes.value(B, nullptr);
    if (!b)
      return std::numeric_limits<uint>::max();
    if (a == b)
      return 0;
    if (a == m_root)
      return b->m_depth - 1;
    if (b == m_root)
      return a->m_depth - 1;
    QVector<Node*> a_ancestors = a->ancestors();
    if (a_ancestors.contains(b))
      return b->m_depth - a->m_depth;
    QVector<Node*> b_ancestors = b->ancestors();
    if (b_ancestors.contains(a))
      return a->m_depth - b->m_depth;
    Node* deepest_common_ancestor{nullptr};
    int i_a = a_ancestors.size() - 1;
    int i_b = b_ancestors.size() - 1;
    while (i_a >= 0 && i_b >= 0) {
      if (a_ancestors[i_a] == b_ancestors[i_b])
        deepest_common_ancestor = a_ancestors[i_a];
      --i_a;
      --i_b;
    }
    if (!deepest_common_ancestor)
      return std::numeric_limits<uint>::max();
    return a->m_depth + b->m_depth - 2 * (deepest_common_ancestor->m_depth + 1);
  }

  QMap<QString, Node*> m_nodes{};
  Node* m_root{nullptr};
  QList<Node*> m_leaves{};
  uint m_depth_max{0};
};

} // namepspace puzzle_2019_06

void Solver_2019_06_1::solve(const QString& input)
{
  using namespace puzzle_2019_06;
  const Tree tree(input);
  if (!tree.m_root)
    finished("FAILURE");
  emit finished(QString::number(tree.getNbOrbits()));
}

void Solver_2019_06_2::solve(const QString& input)
{
  using namespace puzzle_2019_06;
  const Tree tree(input);
  if (!tree.m_root)
    finished("FAILURE");
  emit finished(QString::number(tree.getNbOrbitalTransfers("YOU", "SAN")));
}

