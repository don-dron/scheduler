#define FUNCTION_NAME(name) name

.global FUNCTION_NAME(switch_from_to)

FUNCTION_NAME(switch_from_to):
    
    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12

    pushq %rbx
    pushq %rbp
    
    movq %rsp, (%rdi)
    movq (%rsi), %rsp 

    popq %rbp
    popq %rbx

    popq %r12
    popq %r13
    popq %r14
    popq %r15

    retq