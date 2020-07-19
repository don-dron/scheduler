project(locks)


set(SOURCE_LIB 
    spinlock.c
    )


add_library(locks STATIC ${SOURCE_LIB})