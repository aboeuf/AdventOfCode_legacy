#include <solvers/2016/puzzle_2016_04.h>
#include <solvers/common.h>
#include <solvers/qchar_hash.hpp>

#include <set>

namespace puzzle_2016_04 {

const auto alphabet = QString{"abcdefghijklmnopqrstuvwxyz"};
const auto nb_letters = static_cast<std::size_t>(alphabet.size());

const auto char_to_index = []() {
  auto map = std::unordered_map<QChar, std::size_t>{};
  for (auto i = 0u; i < nb_letters; ++i) {
    map[alphabet[i]] = i;
  }
  return map;
}();

inline QChar shiftChar(const QChar &c, std::size_t distance) {
  const auto it = char_to_index.find(c);
  if (it == std::cend(char_to_index)) {
    return '#';
  }
  return alphabet[static_cast<int>((it->second + distance) % nb_letters)];
}

class Room {
public:
  Room(const QString &input) : m_tokens{common::splitValues(input, '-')} {
    for (auto i = 0; i + 1 < m_tokens.size(); ++i) {
      for (const auto &c : m_tokens[i]) {
        ++m_nb_occurrences_by_characters.insert(std::make_pair(c, 0u))
              .first->second;
      }
    }
    for (const auto &[character, nb_occurences] :
         m_nb_occurrences_by_characters) {
      m_characters_by_nb_occurrences[nb_occurences].insert(character);
    }
    for (auto nit = std::crbegin(m_characters_by_nb_occurrences);
         m_real_checksum.size() < 5 and
         nit != std::crend(m_characters_by_nb_occurrences);
         ++nit) {
      for (auto cit = std::cbegin(nit->second);
           m_real_checksum.size() < 5 and cit != std::cend(nit->second);
           ++cit) {
        m_real_checksum.push_back(*cit);
      }
    }
    const auto splitted_last = common::splitValues(m_tokens.last(), '[');
    m_id = static_cast<std::size_t>(splitted_last[0].toUInt());
    m_given_checksum = splitted_last[1].mid(0, 5);
  }

  std::size_t id() const { return m_id; }

  bool isReal() const { return m_real_checksum == m_given_checksum; }

  QString decipher() const {
    auto result = QString();
    for (auto i = 0; i + 1 < m_tokens.size(); ++i) {
      for (const auto &c : m_tokens[i]) {
        result.push_back(shiftChar(c, m_id));
      }
      if (i + 2 < m_tokens.size()) {
        result.push_back(' ');
      }
    }
    return result;
  }

private:
  QStringList m_tokens;
  std::unordered_map<QChar, uint> m_nb_occurrences_by_characters;
  std::map<uint, std::set<QChar>> m_characters_by_nb_occurrences;
  QString m_real_checksum;
  QString m_given_checksum;
  std::size_t m_id;
};

class Rooms {
public:
  Rooms(const QString &input) {
    const auto lines = common::splitLines(input);
    m_rooms.reserve(lines.size());
    for (const auto &line : lines)
      m_rooms.emplace_back(line);
  }

  QString solveOne() const {
    auto sum = 0u;
    for (const auto &room : m_rooms) {
      if (room.isReal()) {
        sum += room.id();
      }
    }
    return QString("%1").arg(sum);
  }

  QString solveTwo() const {
    for (const auto &room : m_rooms) {
      if (room.isReal() and room.decipher() == "northpole object storage") {
        return QString("%1").arg(room.id());
      }
    }
    return "ERROR";
  }

private:
  std::vector<Room> m_rooms;
};

} // namespace puzzle_2016_04

void Solver_2016_04_1::solve(const QString &input) {
  const auto rooms = puzzle_2016_04::Rooms{input};
  emit finished(rooms.solveOne());
}

void Solver_2016_04_2::solve(const QString &input) {
  const auto rooms = puzzle_2016_04::Rooms{input};
  emit finished(rooms.solveTwo());
}
