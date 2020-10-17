project(scheduler)

set(CAN_USE_ASSEMBLER TRUE)
enable_language(ASM)
set_property(SOURCE switch_context.S PROPERTY LANGUAGE C)

set(SOURCE_LIB 
    rb_tree_scheduler.c
    local_queues_scheduler.c
    local_queues_with_steal_scheduler.c
    thin_heap_scheduler.c
    elastic_tree_scheduler.c
    splay_tree_scheduler.c
    context.c
    coroutine.c 
    fiber.c
    scheduler.c
    switch_context.S )


add_library(scheduler STATIC ${SOURCE_LIB})
target_link_libraries (scheduler LINK_PUBLIC rt pthread)
