cmake_minimum_required(VERSION 2.8.11)
project(Analysis)
find_package(OpenCV 3 REQUIRED)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../Analysis)
add_executable(hist hist.cpp)
add_executable(freq freq.cpp)
target_link_libraries(hist ${OpenCV_LIBS})
target_link_libraries(freq ${OpenCV_LIBS})

