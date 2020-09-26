cmake_minimum_required(VERSION 3.0)
project(Stereovision)
find_package(OpenCV)

set(CMAKE_CXX_STANDARD 20)

file(GLOB CXX_EXEC "src/*.cpp")

add_executable(Stereovision ${CXX_EXEC})
target_include_directories(Stereovision PUBLIC inc/ ${OpenCV_INCLUDE_DIRS})
target_link_libraries(Stereovision ${OpenCV_LIBS})
