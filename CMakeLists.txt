cmake_minimum_required(VERSION 3.5)
project(Scheduler)

set(CMAKE_C_COMPILER ${CC})

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


include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/tests
)


option(DEBUG "With debug mode" ON)
option(REALIME "With realtime priority" OFF)
option(FIBER_STAT "With fiber history" OFF)
option(INTERRUPT_ENABLED "With interrupts" OFF)
option(THREAD_STAT "With thread history" OFF)

option(RB_TREE "Rb tree" ON)
option(LOCAL_QUEUES_WITH_STEAL "Local queues with steal fibers" OFF)
option(LOCAL_QUEUES "Local queues" OFF)
option(THIN_HEAP "Thin heap" OFF)
option(SPLAY_TREE "Splay tree" OFF)

if(DEBUG)
    add_compile_options(-DDEBUG=1)
else(DEBUG)
    add_compile_options(-DDEBUG=0)
endif(DEBUG)

if(REALIME)
    add_compile_options(-DREALTIME=1)
else(REALIME)
    add_compile_options(-DREALTIME=0)
endif(REALIME)

if(FIBER_STAT)
    add_compile_options(-DFIBER_STAT=1)
else(FIBER_STAT)
    add_compile_options(-DFIBER_STAT=0)
endif(FIBER_STAT)

if(THREAD_STAT)
    add_compile_options(-DTHREAD_STAT=1)
else(THREAD_STAT)
    add_compile_options(-DTHREAD_STAT=0)
endif(THREAD_STAT)

if(INTERRUPT_ENABLED)
    add_compile_options(-DINTERRUPT_ENABLED=1)
else(INTERRUPT_ENABLED)
    add_compile_options(-DINTERRUPT_ENABLED=0)
endif(INTERRUPT_ENABLED)

if(RB_TREE)
    message(STATUS "Rb tree")
    add_compile_options(-DRB_TREE=1)
else(RB_TREE)
    message(STATUS "NO Rb tree")
    add_compile_options(-DRB_TREE=0)
endif(RB_TREE)

if(LOCAL_QUEUES_WITH_STEAL)
    add_compile_options(-DLOCAL_QUEUES_WITH_STEAL=1)
else(LOCAL_QUEUES_WITH_STEAL)
    add_compile_options(-DLOCAL_QUEUES_WITH_STEAL=0)
endif(LOCAL_QUEUES_WITH_STEAL)

if(LOCAL_QUEUES)
    add_compile_options(-DLOCAL_QUEUES=1)
else(LOCAL_QUEUES)
    add_compile_options(-DLOCAL_QUEUES=0)
endif(LOCAL_QUEUES)

if(THIN_HEAP)
    add_compile_options(-DTHIN_HEAP=1)
else(THIN_HEAP)
    add_compile_options(-DTHIN_HEAP=0)
endif(THIN_HEAP)

if(ELASTIC_TREE)
    add_compile_options(-DELASTIC_TREE=1)
else(ELASTIC_TREE)
    add_compile_options(-DELASTIC_TREE=0)
endif(ELASTIC_TREE)

if(SPLAY_TREE)
    add_compile_options(-DSPLAY_TREE=1)
else(SPLAY_TREE)
    add_compile_options(-DSPLAY_TREE=0)
endif(SPLAY_TREE)

SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -O3 -Werror -g -fno-omit-frame-pointer -Wextra -Wall -g3 -Wfloat-equal -Wundef -Wswitch-enum -Wconversion -Wunreachable-code -ggdb -Werror-implicit-function-declaration -Wpointer-arith -Wunused -Wno-unused-parameter -Wmissing-declarations -Wmissing-prototypes")
SET(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -static-libasan -g -fno-omit-frame-pointer,-lasan, -W1,-01,-W1,--hash-style=gnu -W1m --as-needed -pie -W1, -z, relro,now -W1,-z, noexecstack ")

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)