#include <2024/puzzle_2024_09.h>
#include <common.h>

namespace puzzle_2024_09 {

using Int = unsigned long long;

struct Block {
  Block(Int size) : size{size}, id{std::nullopt} {}
  Block(Int size, std::optional<Int> id) : size{size}, id{id} {}
  Int size;
  std::optional<Int> id;
};

using Iterator = QList<Block>::Iterator;

class Memory {
public:
  Memory(const QString &disk_map) {
    auto id = Int{0};
    auto free = false;
    for (const auto &c : disk_map) {
      const auto size = QString(c).toULongLong();
      if (size > Int{0}) {
        if (free) {
          m_blocks << Block(size);
        } else {
          m_blocks << Block(size, id);
          ++id;
        }
      }
      free = not free;
    }
    defragment();
  }

  Int checkSum() const {
    auto check_sum = Int{0};
    auto position = Int{0};
    for (const auto &block : m_blocks) {
      if (block.id.has_value()) {
        const auto last_position = position + block.size;
        for (; position < last_position; ++position) {
          check_sum += position * *block.id;
        }
      } else {
        position += block.size;
      }
    }
    return check_sum;
  }

private:
  int findFirstFreeContaining(int source_index) {
    const auto &source_block = m_blocks.at(source_index);
    for (auto dest_index = 0; dest_index < source_index; ++dest_index) {
      const auto &dest_block = m_blocks.at(dest_index);
      if (not dest_block.id.has_value() and
          dest_block.size >= source_block.size) {
        return dest_index;
      }
    }
    return -1;
  }

  void defragment() {
    for (auto current_index = m_blocks.size() - 1; current_index > -1;
         --current_index) {
      if (m_blocks[current_index].id.has_value()) {
        const auto first_free = findFirstFreeContaining(current_index);
        if (first_free > -1) {
          const auto extra_free_size =
              m_blocks[first_free].size - m_blocks[current_index].size;
          m_blocks[first_free].size = m_blocks[current_index].size;
          m_blocks[first_free].id = m_blocks[current_index].id;
          m_blocks[current_index].id.reset();
          if (extra_free_size > Int{0}) {
            m_blocks.insert(first_free + 1, Block(extra_free_size));
            ++current_index;
          }
          while (current_index > 0 and
                 not m_blocks[current_index - 1].id.has_value()) {
            --current_index;
          }
          while (current_index + 1 < m_blocks.size() and
                 not m_blocks[current_index + 1].id.has_value()) {
            m_blocks[current_index].size += m_blocks[current_index + 1].size;
            m_blocks.removeAt(current_index + 1);
          }
        }
      }
    }
  }

  QList<Block> m_blocks;
};

class DiskMap {
public:
  DiskMap(const QString &input)
      : m_disk_map{common::splitLines(input).front()} {}

  QString solveOne() const {
    auto free_blocks = std::vector<Int>{};
    free_blocks.reserve(m_disk_map.size() / 2);
    auto free = false;

    auto occupied_blocks = std::vector<Int>{};
    occupied_blocks.reserve(m_disk_map.size() % 2 == 0
                                ? m_disk_map.size() / 2
                                : m_disk_map.size() / 2 + 1);
    auto total_occupied_size = Int{0};

    for (const auto &c : m_disk_map) {
      if (free) {
        free_blocks.emplace_back(QString(c).toULongLong());
      } else {
        occupied_blocks.emplace_back(QString(c).toULongLong());
        total_occupied_size += occupied_blocks.back();
      }
      free = not free;
    }

    auto current_occupied_index_front = std::size_t{0};
    auto current_occupied_index_back = std::size(occupied_blocks) - 1u;
    auto current_occupied_index = current_occupied_index_front;
    auto current_free_index = std::size_t{0};
    auto check_sum = Int{0};
    auto at_front = true;
    for (auto index = Int{0}; index < total_occupied_size; ++index) {
      check_sum += index * current_occupied_index;
      --occupied_blocks[current_occupied_index];
      if (at_front) {
        if (occupied_blocks[current_occupied_index] == Int{0}) {
          ++current_occupied_index_front;
          if (free_blocks[current_free_index] > Int{0}) {
            current_occupied_index = current_occupied_index_back;
            at_front = false;
          } else {
            ++current_occupied_index;
            ++current_free_index;
          }
        }
      } else {
        if (occupied_blocks[current_occupied_index] == Int{0}) {
          --current_occupied_index_back;
          --current_occupied_index;
        }
        --free_blocks[current_free_index];
        if (free_blocks[current_free_index] == Int{0}) {
          ++current_free_index;
          if (current_occupied_index_front != current_occupied_index_back) {
            current_occupied_index = current_occupied_index_front;
            at_front = true;
          }
        }
      }
    }
    return QString("%1").arg(check_sum);
  }

  QString solveTwo() const {
    const auto memory = Memory(m_disk_map);
    return QString("%1").arg(memory.checkSum());
  }

private:
  QString m_disk_map;
};

} // namespace puzzle_2024_09

void Solver_2024_09_1::solve(const QString &input) {
  const auto disk_map = puzzle_2024_09::DiskMap{input};
  emit finished(disk_map.solveOne());
}

void Solver_2024_09_2::solve(const QString &input) {
  const auto disk_map = puzzle_2024_09::DiskMap{input};
  emit finished(disk_map.solveTwo());
}
