project(structures)

set(SOURCE_LIB 
    lf_stack.c 
    list.c
    thin_heap.c  )

add_library(structures STATIC ${SOURCE_LIB})

target_link_libraries (structures LINK_PUBLIC locks)