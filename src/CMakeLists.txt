cmake_minimum_required(VERSION 3.29.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API OFF)

add_executable(
  OpenGL_Study
  WIN32
  ${CMAKE_CURRENT_SOURCE_DIR}/Main.cpp
)

file(GLOB_RECURSE RENDER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Render/*.cpp")

target_sources(
  OpenGL_Study
  PRIVATE
  ${RENDER_SOURCES}
)

# glad
add_library(glad
  ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/src/glad/glad.c
)

# stb_image
target_sources(
  OpenGL_Study
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/src/OpenGL_Study_stb_image.cpp
)

target_include_directories(
  glad
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/header
)

target_link_libraries(
  OpenGL_Study
  PRIVATE
  glad
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

# Include third party headers
target_include_directories(
  OpenGL_Study
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/header
)

# Third party libraries
if (MSVC)
  file(GLOB THIRD_PARTY_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/lib/*.lib")
else()
  file(GLOB THIRD_PARTY_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/lib/*.a")
endif()

# Link third party libraries
foreach(LIB ${THIRD_PARTY_LIBS})
  get_filename_component(LIB_NAME ${LIB} NAME_WE)
  message("Detected library: ${LIB_NAME}")
endforeach()

target_link_libraries(
  OpenGL_Study
  PRIVATE
  ${THIRD_PARTY_LIBS}
)

target_link_libraries(
  OpenGL_Study
  PRIVATE
  stdc++exp
)

# Third party libraries(DLL)
file(GLOB THIRD_PARTY_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/lib/*.dll")

# Copy DLL
add_custom_command(TARGET OpenGL_Study POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${THIRD_PARTY_DLLS} $<TARGET_FILE_DIR:OpenGL_Study>

  VERBATIM
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
