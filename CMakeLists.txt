cmake_minimum_required(VERSION 3.0)
project(json)

set(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS}")

aux_source_directory(${PROJECT_SOURCE_DIR} SRC_LIST)

add_executable(${PROJECT_NAME} ${SRC_LIST})
