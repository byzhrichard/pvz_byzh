cmake_minimum_required(VERSION 3.26)
project(pvz_byzh)

set(CMAKE_CXX_STANDARD 17)


link_libraries(libeasyx.a libgdi32.a libole32.a)

add_executable(pvz_byzh main.cpp
        tools/tools.cpp)
