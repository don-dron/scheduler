set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


set(TIMEOUT 40)

############
# Test list:
############

######################################
# Coroutine test, switch context test.
######################################

add_executable(coroutine_suite_test.test coroutine_suite_test.c)
add_dependencies(coroutine_suite_test.test lib_scheduler)
target_link_libraries(coroutine_suite_test.test PRIVATE Threads::Threads lib_scheduler)
add_test(NAME Coroutine_Test COMMAND coroutine_suite_test.test)

set_tests_properties(Coroutine_Test PROPERTIES TIMEOUT ${TIMEOUT})


######################################
# Lock free stack tests.
######################################


add_executable(lock_free_stack_test.test lock_free_stack_test.c)
add_dependencies(lock_free_stack_test.test lib_scheduler)
target_link_libraries(lock_free_stack_test.test PRIVATE Threads::Threads lib_scheduler)
add_test(NAME Lock_Free_Stack_Tests COMMAND lock_free_stack_test.test)

set_tests_properties(Lock_Free_Stack_Tests PROPERTIES TIMEOUT ${TIMEOUT})


######################################
# List test.
######################################


add_executable(list_test.test list_test.c)
add_dependencies(list_test.test lib_scheduler)
target_link_libraries(list_test.test PRIVATE Threads::Threads lib_scheduler)
add_test(NAME List_Tests COMMAND list_test.test)

set_tests_properties(List_Tests PROPERTIES TIMEOUT ${TIMEOUT})


######################################
# Scheduler tests.
######################################


add_executable(scheduler_test.test scheduler_test.c)
add_dependencies(scheduler_test.test lib_scheduler)
target_link_libraries(scheduler_test.test PRIVATE Threads::Threads lib_scheduler)
add_test(NAME Scheduler_Tests COMMAND scheduler_test.test)

set_tests_properties(Scheduler_Tests PROPERTIES TIMEOUT ${TIMEOUT})


######################################
# Sleeping tests.
######################################


add_executable(sleep_test.test sleep_test.c)
add_dependencies(sleep_test.test lib_scheduler)
target_link_libraries(sleep_test.test PRIVATE Threads::Threads lib_scheduler)
add_test(NAME Sleep_Tests COMMAND sleep_test.test)

set_tests_properties(Sleep_Tests PROPERTIES TIMEOUT ${TIMEOUT})


######################################
# Multiply schedulers
######################################


add_executable(multiply_scheduler.test multiply_scheduler_test.c)
add_dependencies(multiply_scheduler.test lib_scheduler)
target_link_libraries(multiply_scheduler.test PRIVATE Threads::Threads lib_scheduler)
add_test(NAME Multiply_Schedulers COMMAND multiply_scheduler.test)

set_tests_properties(Multiply_Schedulers PROPERTIES TIMEOUT ${TIMEOUT})



######################################
# Tree benchmark
######################################


add_executable(tree_benchmark.test tree_benchmark.c)
add_dependencies(tree_benchmark.test lib_scheduler)
target_link_libraries(tree_benchmark.test PRIVATE Threads::Threads lib_scheduler)
add_test(NAME Tree_Benchmark COMMAND tree_benchmark.test)

set_tests_properties(Tree_Benchmark PROPERTIES TIMEOUT ${TIMEOUT})


######################################
# Timeouts tests.
######################################


add_executable(timeout_scheduler.test timeout_scheduler.c)
add_dependencies(timeout_scheduler.test lib_scheduler)
target_link_libraries(timeout_scheduler.test PRIVATE Threads::Threads lib_scheduler)
add_test(NAME Timeout_Tests COMMAND timeout_scheduler.test)

set_tests_properties(Timeout_Tests PROPERTIES TIMEOUT ${TIMEOUT})