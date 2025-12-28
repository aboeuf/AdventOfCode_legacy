add_library(cpp_solvers
  STATIC
    src/2025/solve_2025_01.cpp
    src/2025/solve_2025_02.cpp
    src/2025/solve_2025_08.cpp
    src/aoc.cpp
    src/converters.cpp
    src/parsers.cpp
    src/utils.cpp
)

target_include_directories(cpp_solvers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(cpp_solvers PROPERTIES POSITION_INDEPENDENT_CODE ON)
