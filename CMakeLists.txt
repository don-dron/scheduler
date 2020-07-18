cmake_minimum_required(VERSION 3.5)
project(Scheduler VERSION 0.1.0)

include(CTest)
enable_testing()

set(CAN_USE_ASSEMBLER TRUE)
enable_language(ASM)

set(CMAKE_BUILD_TYPE Debug)

set_property(SOURCE context/switch_context.S PROPERTY LANGUAGE C)

add_executable(Scheduler main.c coroutine/coroutine.h coroutine/coroutine.c context/context.h context/switch_context.S 
lockfree/stack/stack.c lockfree/stack/stack.h scheduler/fiber.c scheduler/fiber.h scheduler/scheduler.h scheduler/scheduler.c)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(Scheduler PRIVATE Threads::Threads)

include_directories("${CMAKE_SOURCE_DIR}/context"
    "${CMAKE_SOURCE_DIR}/coroutine" "${CMAKE_SOURCE_DIR}/lockfree"  "${CMAKE_SOURCE_DIR}/scheduler")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})


include(CPack)
