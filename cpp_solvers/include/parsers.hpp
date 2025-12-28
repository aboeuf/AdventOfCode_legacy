#pragma once

#include <converters.hpp>
#include <deque>

namespace aoc {

std::deque<std::string> split(const std::string &str, char delimiter = ',',
                              bool remove_empty = true);

std::deque<std::string> splitLines(const std::string &str,
                                   bool remove_empty = true);

std::deque<short int> splitToShortInt(const std::string &str,
                                      char delimiter = ',');

std::deque<int> splitToInt(const std::string &str, char delimiter = ',');

std::deque<long int> splitToLongInt(const std::string &str,
                                    char delimiter = ',');

std::deque<long long int> splitToLongLongInt(const std::string &str,
                                             char delimiter = ',');

std::deque<unsigned short int> splitToUShortInt(const std::string &str,
                                                char delimiter = ',');

std::deque<unsigned int> splitToUInt(const std::string &str,
                                     char delimiter = ',');

std::deque<unsigned long int> splitToULongInt(const std::string &str,
                                              char delimiter = ',');

std::deque<unsigned long long int> splitToULongLongInt(const std::string &str,
                                                       char delimiter = ',');

std::deque<float> splitToFloat(const std::string &str, char delimiter = ',');

std::deque<double> splitToDouble(const std::string &str, char delimiter = ',');

} // namespace aoc
