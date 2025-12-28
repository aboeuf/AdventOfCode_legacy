set(CMAKE_SHARED_MODULE_PREFIX "")

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(Boost COMPONENTS python REQUIRED)

add_library(aoc_bindings
  MODULE
    aoc_bindings.cpp
)

set_target_properties(aoc_bindings PROPERTIES OUTPUT_NAME "aoc")

target_include_directories(aoc_bindings PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(aoc_bindings
  cpp_solvers
  Boost::python
  Python3::Module
  Qt5::Core
)
