cmake_minimum_required(VERSION 3.29.0)

project(OpenGL_Study VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(src)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/Intermediate)