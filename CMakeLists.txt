cmake_minimum_required(VERSION 3.31)
project(AdoCpp)

set(CMAKE_CXX_STANDARD 20)

include(${PROJECT_SOURCE_DIR}/GetRapidJSON.cmake)

add_subdirectory(AdoCpp)
add_subdirectory(AdoCppGame)
add_subdirectory(AdoCppMacro)
add_subdirectory(test)
