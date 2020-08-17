cmake_minimum_required(VERSION 3.5)
project(Scheduler)

SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin
        CACHE PATH
        "Single Directory for all"
    )

SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin
        CACHE PATH
        "Single Directory for all"
    )

SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/lib
        CACHE PATH
        "Single Directory for all"
    )

SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -O3 -Wextra -Wall -Wfloat-equal -Wundef -Wswitch-enum -Wconversion -Wunreachable-code -ggdb -Werror-implicit-function-declaration")
SET(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ")

enable_testing()


include_directories(
  ${PROJECT_SOURCE_DIR}/include
)

add_subdirectory(src)
add_subdirectory(tests)