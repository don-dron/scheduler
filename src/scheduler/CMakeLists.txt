project(scheduler)

set(CAN_USE_ASSEMBLER TRUE)
enable_language(ASM)
set_property(SOURCE switch_context.S PROPERTY LANGUAGE C)

set(SOURCE_LIB 
    local_queues_with_steal_scheduler.c
    context.c
    coroutine.c 
    fiber.c
    scheduler.c
    switch_context.S )


add_library(scheduler STATIC ${SOURCE_LIB})
target_link_libraries (scheduler LINK_PUBLIC rt)
