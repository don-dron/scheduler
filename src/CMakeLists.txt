set(CAN_USE_ASSEMBLER TRUE)
enable_language(ASM)
set_property(SOURCE switch_context.S PROPERTY LANGUAGE C)


project(lib_scheduler)		

set(SOURCE_LIB 
 
    coroutine.c 
    fiber.c
    lf_stack.c 
    scheduler.c
    
    switch_context.S
    thin_heap.c  )

add_library(lib_scheduler STATIC ${SOURCE_LIB})