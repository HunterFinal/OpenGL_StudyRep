cmake_minimum_required(VERSION 3.29.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

add_executable(
  OpenGL_Study
  WIN32
  ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
)

target_compile_features(
  OpenGL_Study
  PRIVATE
  cxx_std_23
)

set_target_properties(
  OpenGL_Study
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/Execution/Debug"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/Execution/Release"
)

if(MSVC)

else()
  target_compile_options(
    OpenGL_Study
    PRIVATE
    -Wall
    -std=c++23
  )
endif()


add_custom_command(TARGET OpenGL_Study POST_BUILD
  COMMAND &{CMAKE_COMMAND} -E copy_if_different
)