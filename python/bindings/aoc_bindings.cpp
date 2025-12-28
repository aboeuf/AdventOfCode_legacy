#include <aoc.hpp>
#include <boost/python.hpp>
#include <memory>

namespace bp = boost::python;

namespace aoc_bindings {

bp::list getPolygonPoints(const std::shared_ptr<aoc::Polygon> &polygon) {
  auto points = bp::list();
  for (const auto &point : polygon->points) {
    points.append(point);
  }
  return points;
}

bp::list getResultOutput(const std::shared_ptr<aoc::Result> &result) {
  auto output = bp::list();
  for (const auto &line : result->output) {
    output.append(line);
  }
  return output;
}

bp::dict getResultDisplay(const std::shared_ptr<aoc::Result> &result) {
  auto display = bp::dict();

  auto rectangles = bp::list();
  for (const auto &rectangle : result->display.rectangles) {
    auto display_object = bp::dict();
    display_object["object"] = rectangle.object;
    display_object["style"] = rectangle.style;
    rectangles.append(display_object);
  }
  display["rectangles"] = rectangles;

  auto polygons = bp::list();
  for (const auto &polygon : result->display.polygons) {
    auto display_object = bp::dict();
    display_object["object"] = polygon.object;
    display_object["style"] = polygon.style;
    polygons.append(display_object);
  }
  display["polygons"] = polygons;

  return display;
}

std::string resultToString(const std::shared_ptr<aoc::Result> &result) {
  return result->success ? "Part one: " + result->part_one_solution +
                               "\nPart two: " + result->part_two_solution
                         : "failure";
}

} // namespace aoc_bindings

BOOST_PYTHON_MODULE(aoc) {

  bp::class_<aoc::Position>("Position")
      .def(bp::init<>())
      .def_readwrite("x", &aoc::Position::x)
      .def_readwrite("y", &aoc::Position::y);

  bp::class_<aoc::Pose>("Pose")
      .def(bp::init<>())
      .def_readwrite("position", &aoc::Pose::position)
      .def_readwrite("orientation", &aoc::Pose::orientation);

  bp::class_<aoc::Rectangle>("Rectangle")
      .def(bp::init<>())
      .def_readwrite("center", &aoc::Rectangle::center)
      .def_readwrite("width", &aoc::Rectangle::width)
      .def_readwrite("length", &aoc::Rectangle::length);

  bp::class_<aoc::Polygon>("Polygon")
      .def(bp::init<>())
      .def("points", &aoc_bindings::getPolygonPoints);

  bp::class_<aoc::DisplayStyle>("DisplayStyle")
      .def(bp::init<>())
      .def_readwrite("edge_line_width", &aoc::DisplayStyle::edge_line_width)
      .def_readwrite("edge_color", &aoc::DisplayStyle::edge_color)
      .def_readwrite("face_color", &aoc::DisplayStyle::face_color);

  bp::class_<aoc::Result>("Result")
      .def(bp::init<>())
      .def_readwrite("success", &aoc::Result::success)
      .def_readwrite("part_one_solution", &aoc::Result::part_one_solution)
      .def_readwrite("part_two_solution", &aoc::Result::part_two_solution)
      .def("output", &aoc_bindings::getResultOutput)
      .def("display", &aoc_bindings::getResultDisplay)
      .def("__str__", &aoc_bindings::resultToString);

  bp::def("has_solver", &aoc::hasSolver);
  bp::def("solve", &aoc::solve);
}
