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


######################################
# Lock free stack tests.
######################################


add_executable(lock_free_stack_test lock_free_stack_test.c)
add_dependencies(lock_free_stack_test lib_scheduler)
target_link_libraries(lock_free_stack_test PRIVATE Threads::Threads lib_scheduler)
add_test(NAME Lock_Free_Stack_Tests COMMAND lock_free_stack_test)

set_tests_properties(Lock_Free_Stack_Tests PROPERTIES TIMEOUT ${TIMEOUT})



######################################
# Free list test.
######################################


add_executable(free_list_test free_list_test.c)
add_dependencies(free_list_test lib_scheduler)
target_link_libraries(free_list_test PRIVATE Threads::Threads lib_scheduler)
add_test(NAME Free_List_Tests COMMAND free_list_test)

set_tests_properties(Free_List_Tests PROPERTIES TIMEOUT ${TIMEOUT})




######################################
# Scheduler tests.
######################################


add_executable(scheduler_test scheduler_test.c)
add_dependencies(scheduler_test lib_scheduler)
target_link_libraries(scheduler_test PRIVATE Threads::Threads lib_scheduler)
add_test(NAME Scheduler_Tests COMMAND scheduler_test)

set_tests_properties(Scheduler_Tests PROPERTIES TIMEOUT ${TIMEOUT})


