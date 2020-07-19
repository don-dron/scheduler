cmake_minimum_required(VERSION 3.5)
project(Scheduler)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")


include_directories(
  ${PROJECT_SOURCE_DIR}/include
)

add_subdirectory(src)
add_subdirectory(tests)