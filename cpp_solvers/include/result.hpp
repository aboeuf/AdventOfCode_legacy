#pragma once

#include <functional>
#include <utils.hpp>

namespace aoc {

struct Position {
  double x;
  double y;
};

struct Pose {
  Position position;
  double orientation;
};

struct Rectangle {
  Pose center;
  double width;
  double length;
};

struct Polygon {
  std::vector<Position> points;
};

struct DisplayStyle {
  double edge_line_width;
  std::string edge_color;
  std::string face_color;
};

template <typename ObjectType> struct DisplayObject {
  ObjectType object;
  DisplayStyle style;
};

struct Display {
  std::deque<DisplayObject<Rectangle>> rectangles;
  std::deque<DisplayObject<Polygon>> polygons;
};

struct Result {
  Result() : success{true} {}

  Result(const std::string &error_message) : success{false} {
    output.emplace_back(error_message);
  }

  bool success;
  std::string part_one_solution;
  std::string part_two_solution;
  std::deque<std::string> output;
  Display display;
};

using Solver = std::function<Result(const std::string &)>;

} // namespace aoc
