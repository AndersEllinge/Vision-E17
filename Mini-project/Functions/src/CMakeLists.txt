cmake_minimum_required(VERSION 2.8.11)
project(Functions)
find_package(OpenCV 3 REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../Analysis)
add_executable(hist Analysis/hist.cpp)
add_executable(freq Analysis/freq.cpp)
target_link_libraries(hist ${OpenCV_LIBS})
target_link_libraries(freq ${OpenCV_LIBS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../Processing)
add_executable(median Processing/median.cpp)
target_link_libraries(hist ${OpenCV_LIBS})
