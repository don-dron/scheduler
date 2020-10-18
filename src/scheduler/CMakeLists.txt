project(scheduler)

set(CAN_USE_ASSEMBLER TRUE)
enable_language(ASM)
set_property(SOURCE switch_context.S PROPERTY LANGUAGE C)

if(RB_TREE)
    set(IMPL rb_tree_scheduler.c)
endif(RB_TREE)

if(LOCAL_QUEUES_WITH_STEAL)
    set(IMPL local_queues_with_steal_scheduler.c)
endif(LOCAL_QUEUES_WITH_STEAL)

if(LOCAL_QUEUES)
    set(IMPL local_queues_scheduler.c)
endif(LOCAL_QUEUES)

if(THIN_HEAP)
    set(IMPL thin_heap_scheduler.c)
endif(THIN_HEAP)

if(SPLAY_TREE)
    set(IMPL splay_tree_scheduler.c)
endif(SPLAY_TREE)

if(ELASTIC_TREE)
    set(IMPL elastic_tree_scheduler.c)
endif(ELASTIC_TREE)

message(STATUS "${IMPL}")
set(SOURCE_LIB 
    ${IMPL}

    context.c
    coroutine.c 
    fiber.c
    scheduler.c
    switch_context.S )


add_library(scheduler STATIC ${SOURCE_LIB})
target_link_libraries (scheduler LINK_PUBLIC rt pthread)
