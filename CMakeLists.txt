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


option(DEBUG "With debug mode" OFF)

option(FIBER_STAT "With fiber history" OFF)
option(INTERRUPT_ENABLED "With interrupts" ON)
option(THREAD_STAT "With thread history" OFF)
option(LOCAL_QUEUES_WITH_STEAL "Local queues with steal fibers" ON)

if(DEBUG)
    add_definitions(-DDEBUG=1)
else(DEBUG)
    add_definitions(-DDEBUG=0)
endif(DEBUG)

if(FIBER_STAT)
    add_definitions(-DFIBER_STAT=1)
else(FIBER_STAT)
    add_definitions(-DFIBER_STAT=0)
endif(FIBER_STAT)

if(THREAD_STAT)
    add_definitions(-DTHREAD_STAT=1)
else(THREAD_STAT)
    add_definitions(-DTHREAD_STAT=0)
endif(THREAD_STAT)

if(INTERRUPT_ENABLED)
    add_definitions(-DINTERRUPT_ENABLED=1)
else(INTERRUPT_ENABLED)
    add_definitions(-DINTERRUPT_ENABLED=0)
endif(INTERRUPT_ENABLED)

if(LOCAL_QUEUES_WITH_STEAL)
    add_definitions(-DLOCAL_QUEUES_WITH_STEAL=1)
else(LOCAL_QUEUES_WITH_STEAL)
    add_definitions(-DLOCAL_QUEUES_WITH_STEAL=0)
endif(LOCAL_QUEUES_WITH_STEAL)

add_subdirectory(src)
add_subdirectory(tests)