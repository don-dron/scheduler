set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


set(TIMEOUT 40)

############
# Test list:
############

######################################
# Coroutine test, switch context test.
######################################

add_executable(coroutine_suite_test coroutine_suite_test.c)
add_dependencies(coroutine_suite_test lib_scheduler)
target_link_libraries(coroutine_suite_test PRIVATE Threads::Threads lib_scheduler)
add_test(NAME Coroutine_Test COMMAND coroutine_suite_test)

set_tests_properties(Coroutine_Test PROPERTIES TIMEOUT ${TIMEOUT})

