set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(TARGET_NAME test_main)

set(TARGET_SRC 
    main.c
    )


add_executable(${TARGET_NAME} ${TARGET_SRC})

add_dependencies(${TARGET_NAME} lib_scheduler)

target_link_libraries(${TARGET_NAME} PRIVATE Threads::Threads lib_scheduler)
