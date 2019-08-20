cmake_minimum_required(VERSION 3.14)
project(c8emu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "~/.qt/5.13.0/gcc_64/lib/cmake")

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

add_executable(c8emu
        src/main.cc
        src/window.ui src/window.cc
        src/chip8.h src/chip8.cc
)

target_link_libraries(c8emu Qt5::Widgets)
