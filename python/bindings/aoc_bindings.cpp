#include <boost/python.hpp>

#include <solvers/solvers.h>

#include <QObject>

namespace bp = boost::python;

namespace aoc_bindings {

struct Result {
  Result() = default;
  Result(const QString &solution) : solution{solution.toStdString()} {}

  std::string str() const { return solution; }

  std::string solution;
  bp::list output;
  bp::list display;
};

class Aggregator : public QObject {
  Q_OBJECT

public:
  Result result;

  Aggregator(Solver *solver) {
    connect(solver, SIGNAL(output(QString)), this,
            SLOT(onOutputReceived(QString)));
    connect(solver, SIGNAL(display(DisplayData)), this,
            SLOT(onDisplayDataReceived(DisplayData)));
    connect(solver, SIGNAL(finished(QString)), this,
            SLOT(onFinishedReceived(QString)));
  }

public slots:
  void onOutputReceived(const QString &data) {
    result.output.append(data.toStdString());
  }

  void onDisplayDataReceived(const DisplayData &data) {
    for (const auto &rectangle : data.rectangles) {
      result.display.append(rectangle);
    }
  }

  void onFinishedReceived(const QString &solution) {
    result.solution = solution.toStdString();
  }
};

struct StaticSolvers {
  static Result solve(int year, int day, int puzzle, const std::string &input) {
    auto *solver = s_solvers(year, day, puzzle);
    if (solver == nullptr) {
      return Result(
          QString("cannot find solver for year %1, day %2 and puzzle %3")
              .arg(year)
              .arg(day)
              .arg(puzzle));
    }
    const auto aggregator = Aggregator(solver);
    solver->solve(QString(input.c_str()));
    return aggregator.result;
  }

  static Solvers s_solvers;
};

Solvers StaticSolvers::s_solvers = Solvers();

Result solve(int year, int day, int puzzle, const std::string &input) {
  return StaticSolvers::solve(year, day, puzzle, input);
}

} // namespace aoc_bindings

BOOST_PYTHON_MODULE(aoc) {
  bp::class_<aoc_bindings::Result>("Result")
      .def(bp::init<>())
      .def_readwrite("solution", &aoc_bindings::Result::solution)
      .def_readwrite("output", &aoc_bindings::Result::output)
      .def_readwrite("display", &aoc_bindings::Result::display)
      .def("__str__", &aoc_bindings::Result::str);

  bp::def("solve", &aoc_bindings::solve);
}

#include "aoc_bindings.moc"
