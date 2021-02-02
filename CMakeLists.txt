cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)

project(
    CURSES_DISP
    VERSION 0.1.0
    DESCRIPTION "RAII wrapper to curses"
)

find_package(Curses REQUIRED)
find_package(Threads REQUIRED)

include_directories(includes/cursesDisp)
include_directories(.)
file(GLOB DISP_SOURCES "src/*cpp")
add_library(curses_disp STATIC ${DISP_SOURCES})
target_link_libraries(curses_disp PRIVATE curses)
target_link_libraries(curses_disp PRIVATE Threads::Threads)

add_executable(example "examples/adapter_example.cpp")
target_link_libraries(example PRIVATE curses)
target_link_libraries(example PRIVATE curses_disp)
target_link_libraries(example PRIVATE Threads::Threads)
