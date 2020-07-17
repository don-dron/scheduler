cmake_minimum_required(VERSION 3.5)
project(scheduler VERSION 0.1.0)

include(CTest)
enable_testing()

set(CAN_USE_ASSEMBLER TRUE)
enable_language(ASM)

set(CMAKE_BUILD_TYPE Debug)

set_property(SOURCE context/switch_context.S PROPERTY LANGUAGE C)

add_executable(scheduler main.c coroutine/coroutine.h coroutine/coroutine.c context/context.h context/switch_context.S 
lockfree/stack/stack.c lockfree/stack/stack.h)

include_directories("${CMAKE_SOURCE_DIR}/context"
    "${CMAKE_SOURCE_DIR}/coroutine" "${CMAKE_SOURCE_DIR}/lockfree")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})


include(CPack)
