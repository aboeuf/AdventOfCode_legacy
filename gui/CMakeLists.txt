add_executable(aoc_gui
  display/display.cpp
  display/view.cpp
  leaderboard.cpp
  main.cpp
  mainwindow.cpp
  resources.qrc
)

target_include_directories(aoc_gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(aoc_gui aoc_solvers Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::Sql)
