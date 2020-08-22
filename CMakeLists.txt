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

SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -O3 -Werror -Wextra -Wall -g -g3 -Wfloat-equal -Wundef -Wswitch-enum -Wconversion -Wunreachable-code -ggdb -Werror-implicit-function-declaration -Wpointer-arith -Wunused -Wno-unused-parameter -Wmissing-declarations -Wmissing-prototypes")
SET(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -W1,-01,-W1,--hash-style=gnu -W1m --as-needed -pie -W1, -z, relro,now -W1,-z, noexecstack ")

enable_testing()


include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/tests
)

add_definitions(-DDEBUG=0)
add_definitions(-DINTERRUPT_ENABLED=1)
add_definitions(-DLOCAL_QUEUES_WITH_STEAL=1)

add_subdirectory(src)
add_subdirectory(tests)