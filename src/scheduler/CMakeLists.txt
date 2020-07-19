project(scheduler)

set(CAN_USE_ASSEMBLER TRUE)
enable_language(ASM)
set_property(SOURCE switch_context.S PROPERTY LANGUAGE C)

set(SOURCE_LIB 
    context.c
    coroutine.c 
    fiber.c
    scheduler.c
    switch_context.S )


add_library(scheduler STATIC ${SOURCE_LIB})
