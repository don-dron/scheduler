project(locks)


set(SOURCE_LIB 
    atomics.c
    spinlock.c
    wait_group.c
    )


add_library(locks STATIC ${SOURCE_LIB})