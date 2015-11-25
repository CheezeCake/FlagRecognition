cmake_minimum_required(VERSION 2.8)

project(FlagRecognition)

find_package(OpenCV REQUIRED)

file(GLOB source_files *.cpp)
add_executable(flagRecognition ${source_files})
target_link_libraries(flagRecognition ${OpenCV_LIBS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
