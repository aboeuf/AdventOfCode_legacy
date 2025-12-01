#include <optional>
#include <solvers/2021/puzzle_2021_18.h>
#include <solvers/common.h>

struct Pair {
  Pair(uint number) { m_number = number; }

  Pair(const Pair &left, const Pair &right) : m_parent{nullptr}, m_depth{0} {
    m_left = new Pair(left);
    m_left->setParent(this);
    m_right = new Pair(right);
    m_right->setParent(this);
    reduce();
  }

  Pair(const Pair &p) {
    m_number = p.m_number;
    if (p.m_left)
      m_left = new Pair(*p.m_left);
    if (p.m_right)
      m_right = new Pair(*p.m_right);
  }

  Pair(const QString &input, Pair *parent = nullptr)
      : m_parent{parent}, m_depth{0} {
    if (parent)
      m_depth = parent->m_depth + 1;
    if (input.startsWith('[') and input.endsWith(']')) {
      auto nb_in_left = 1;
      auto count = 0;
      for (; nb_in_left < input.size() - 1; ++nb_in_left) {
        if ((input[nb_in_left] == ',') and count == 0)
          break;
        if (input[nb_in_left] == '[')
          ++count;
        else if (input[nb_in_left] == ']')
          --count;
      }
      if (count != 0 or input[nb_in_left] != ",")
        return;
      const auto left = input.mid(1, nb_in_left - 1);
      if (left.isEmpty())
        return;
      const auto nb_in_right = input.size() - nb_in_left - 2;
      const auto right = input.mid(nb_in_left + 1, nb_in_right);
      if (right.isEmpty())
        return;
      m_left = new Pair(left, this);
      m_right = new Pair(right, this);
      return;
    }
    if (not std::all_of(std::begin(input), std::end(input),
                        [](const auto &c) { return c.isDigit(); }))
      return;
    bool ok;
    m_number = input.toUInt(&ok);
    if (not ok)
      m_number = std::nullopt;
  }

  bool valid() const {
    if (m_number.has_value())
      return m_left == nullptr and m_right == nullptr;
    if (not m_left or not m_right)
      return false;
    if (m_left->m_parent != this)
      return false;
    if (m_right->m_parent != this)
      return false;
    return m_left->valid() and m_right->valid();
  }

  QString toString() const {
    if (not valid())
      return "*";
    if (m_number)
      return QString("%1").arg(*m_number);
    return "[" + m_left->toString() + "," + m_right->toString() + "]";
  }

  Pair *leftNumber() const {
    if (not m_parent)
      return nullptr;
    if (m_parent->m_left == this)
      return m_parent->leftNumber();
    if (m_parent->m_right == this) {
      auto *current = m_parent->m_left;
      while (current) {
        if (current->m_number)
          return current;
        current = current->m_right;
      }
    }
    return nullptr;
  }

  Pair *rightNumber() const {
    if (not m_parent)
      return nullptr;
    if (m_parent->m_right == this)
      return m_parent->rightNumber();
    if (m_parent->m_left == this) {
      auto *current = m_parent->m_right;
      while (current) {
        if (current->m_number)
          return current;
        current = current->m_left;
      }
    }
    return nullptr;
  }

  bool explode() {
    if (m_depth < 4) {
      if (m_number)
        return false;
      if (m_left and m_left->explode())
        return true;
      if (m_right and m_right->explode())
        return true;
      return false;
    }
    if (not m_left or not m_right)
      return false;
    const auto exp = [](auto *child, auto *neighbor) {
      if (child and child->m_number and neighbor and neighbor->m_number) {
        *(neighbor->m_number) += child->m_number.value();
      }
    };
    exp(m_left, leftNumber());
    exp(m_right, rightNumber());
    delete m_left;
    m_left = nullptr;
    delete m_right;
    m_right = nullptr;
    m_number = 0;
    return true;
  }

  bool split() {
    if (m_number) {
      if (*m_number > 9) {
        m_left = new Pair(*m_number / 2);
        m_left->setParent(this);
        m_right = new Pair(*m_number / 2);
        m_right->setParent(this);
        if (*m_number % 2 == 1)
          *(m_right->m_number) += 1;
        m_number = std::nullopt;
        return true;
      }
      return false;
    }
    if (m_left and m_left->split())
      return true;
    if (m_right and m_right->split())
      return true;
    return false;
  }

  void reduce() {
    while (valid()) {
      if (explode())
        continue;
      if (split())
        continue;
      break;
    }
  }

  void setParent(Pair *parent) {
    m_parent = parent;
    if (parent)
      m_depth = parent->m_depth + 1;
    else
      m_depth = 0;
    if (m_left)
      m_left->setParent(this);
    if (m_right)
      m_right->setParent(this);
  }

  uint magnitude() const {
    if (m_number)
      return *m_number;
    auto mag = 0;
    if (m_left)
      mag += 3 * m_left->magnitude();
    if (m_right)
      mag += 2 * m_right->magnitude();
    return mag;
  }

  Pair *m_parent{nullptr};
  uint m_depth;
  std::optional<uint> m_number{std::nullopt};
  Pair *m_left{nullptr};
  Pair *m_right{nullptr};
};

class Homework {
public:
  Homework(const QString &input) {
    const auto lines = common::splitLines(input);
    m_pairs.reserve(lines.size());
    for (const auto &line : lines) {
      m_pairs.emplace_back(line);
      if (m_pairs.back().toString() != line) {
        m_invalid_input = line;
        return;
      }
    }
  }

  const QString &invalidInput() const { return m_invalid_input; }

  QString solvePuzzleOne() const {
    if (m_pairs.empty())
      return "empty";
    if (m_pairs.size() == 1)
      return QString("%1").arg(m_pairs.front().magnitude());
    auto current = new Pair(m_pairs[0], m_pairs[1]);
    for (auto i = 2u; current->valid() and i < m_pairs.size(); ++i) {
      auto *new_pair = new Pair(*current, m_pairs[i]);
      std::swap(current, new_pair);
      delete new_pair;
    }
    const auto res = QString("%1").arg(current->magnitude());
    delete current;
    return res;
  }

  QString solvePuzzleTwo() const {
    auto max = 0u;
    for (auto i = 0u; i < m_pairs.size(); ++i)
      for (auto j = 0u; j < m_pairs.size(); ++j)
        if (i != j)
          max = std::max(max, Pair(m_pairs[i], m_pairs[j]).magnitude());
    return QString("%1").arg(max);
  }

private:
  QString m_invalid_input;
  std::vector<Pair> m_pairs;
};

void Solver_2021_18_1::solve(const QString &input) {
  const Homework h(input);
  if (not h.invalidInput().isEmpty()) {
    emit finished("Invalid input: " + h.invalidInput());
    return;
  }
  emit finished(h.solvePuzzleOne());
}

void Solver_2021_18_2::solve(const QString &input) {
  const Homework h(input);
  if (not h.invalidInput().isEmpty()) {
    emit finished("Invalid input: " + h.invalidInput());
    return;
  }
  emit finished(h.solvePuzzleTwo());
}
