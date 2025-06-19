cmake_minimum_required(VERSION 3.10.0)
project(nn VERSION 0.1.0 LANGUAGES C CXX)

add_executable(nn main.cpp)

target_include_directories(nn PRIVATE ${CMAKE_SOURCE_DIR}/packages/eigen-3.4.0)

