#include <QMap>
#include <solvers/common.h>
#include <set>
#include <solvers/2022/puzzle_2022_07.h>

namespace puzzle_2022_07 {

using Id = std::size_t;
using Int = long long unsigned int;

class Directory {
public:
  Directory() = default;
  Directory(Id ID, const QString &name,
            const std::optional<Id> &parent_directory_ID = std::nullopt)
      : m_ID{ID}, m_name{name}, m_parent_directory_ID{parent_directory_ID} {}

  Id ID() const { return m_ID; }
  const QString &name() const { return m_name; }
  const std::optional<Id> &parentDirectoryID() const {
    return m_parent_directory_ID;
  }
  const QMap<QString, Int> &files() const { return m_files; }
  const std::set<Id> &childrenDirectoriesIDs() const {
    return m_children_directories_IDs;
  }

  void addFile(const QString &name, Int size) {
    if (not m_files.contains(name))
      m_files.insert(name, size);
  }

  void addChild(Id ID) { m_children_directories_IDs.insert(ID); }

private:
  Id m_ID;
  QString m_name;
  std::optional<Id> m_parent_directory_ID;
  QMap<QString, Int> m_files{};
  std::set<Id> m_children_directories_IDs{};
};

class FileSystem {
public:
  FileSystem(const QString &input) {
    m_directories.emplace_back(0, "/");
    auto curr_ID = Id{0};
    const auto lines = common::splitLines(input);
    for (auto line : lines) {
      line.remove('\n');
      if (line.isEmpty())
        continue;
      if (line == "$ cd /") {
        curr_ID = Id{0};
        continue;
      }
      if (line.startsWith("$")) {
        line.remove(0, 2);
        if (line.startsWith("cd")) {
          line.remove(0, 3);
          if (line == "..")
            curr_ID = m_directories[curr_ID].parentDirectoryID().value();
          else
            curr_ID = addDirectory(line, curr_ID);
        }
      } else {
        if (line.front().isDigit()) {
          const auto values = common::splitValues(line, ' ');
          m_directories[curr_ID].addFile(values[1], values[0].toUInt());
        } else if (line.startsWith("dir ")) {
          line.remove(0, 4);
          addDirectory(line, curr_ID);
        }
      }
    }
  }

  Int puzzleOne() const {
    auto sum = Int{0};
    for (auto ID = Id{0}; ID < m_directories.size(); ++ID) {
      const auto size = getSize(ID);
      if (size <= Int{100000})
        sum += size;
    }
    return sum;
  }

  Int puzzleTwo() const {
    const auto used_space = getSize(0);
    const auto unused_space = Int{70000000} - used_space;
    const auto needed_space = Int{30000000} - unused_space;
    auto size_min = std::numeric_limits<Int>::max();
    for (auto ID = Id{0}; ID < m_directories.size(); ++ID) {
      const auto size = getSize(ID);
      if (size >= needed_space) {
        if (size < size_min)
          size_min = size;
      }
    }
    return size_min;
  }

private:
  std::optional<Id> getID(const QString &name, Id parent_ID) const {
    for (const auto &ID : m_directories[parent_ID].childrenDirectoriesIDs())
      if (ID < m_directories.size() and m_directories[ID].name() == name)
        return ID;
    return std::nullopt;
  }

  Id addDirectory(const QString &name, Id parent_ID) {
    const auto ID = getID(name, parent_ID);
    if (ID.has_value())
      return *ID;
    m_directories.emplace_back(m_directories.size(), name, parent_ID);
    m_directories[parent_ID].addChild(m_directories.back().ID());
    return m_directories.back().ID();
  }

  Int getSize(Id ID) const {
    auto size = 0;
    for (const auto &name : m_directories[ID].files().keys())
      size += m_directories[ID].files().value(name);
    for (const auto &child_ID : m_directories[ID].childrenDirectoriesIDs())
      size += getSize(child_ID);
    return size;
  }

  std::vector<Directory> m_directories;
};

} // namespace puzzle_2022_07

void Solver_2022_07_1::solve(const QString &input) {
  const auto fs = puzzle_2022_07::FileSystem{input};
  emit finished(QString("%1").arg(fs.puzzleOne()));
}

void Solver_2022_07_2::solve(const QString &input) {
  const auto fs = puzzle_2022_07::FileSystem{input};
  emit finished(QString("%1").arg(fs.puzzleTwo()));
}
