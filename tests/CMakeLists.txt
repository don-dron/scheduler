set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


set(TIMEOUT 40)

function(_ADD_TEST NAME)
    add_executable(${NAME}.test ${NAME}.c test_utils.h)
    add_dependencies(${NAME}.test lib_scheduler)
    target_link_libraries(${NAME}.test PRIVATE Threads::Threads lib_scheduler)
    add_test(NAME ${NAME} COMMAND ${NAME}.test)
    set_tests_properties(${NAME} PROPERTIES TIMEOUT ${TIMEOUT})

endfunction(_ADD_TEST NAME)


_ADD_TEST(multiply_scheduler)
_ADD_TEST(scheduler)
_ADD_TEST(sleep)
_ADD_TEST(timeout_scheduler)
_ADD_TEST(tree_benchmark)
_ADD_TEST(tree_benchmark_without_sleep)
_ADD_TEST(tree_benchmark_with_recursive)