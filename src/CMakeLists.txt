project(lib_scheduler)		

add_subdirectory(locks)
add_subdirectory(scheduler)
add_subdirectory(structures)

set(SOURCE_LIB default.c)

add_library(lib_scheduler STATIC ${SOURCE_LIB})

target_link_libraries (lib_scheduler LINK_PUBLIC locks scheduler structures)