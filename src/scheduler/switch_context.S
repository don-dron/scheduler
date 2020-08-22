#define FUNCTION_NAME(name) name

.global FUNCTION_NAME(switch_from_to)

FUNCTION_NAME(switch_from_to):
    
    // For example: call from fiber code

    // Save registers on stack

    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12

    pushq %rbx
    pushq %rbp
    
    // Change stack pointer swithc run -------------------------------->   
    movq %rsp, (%rdi)
                                                                                    // For example: scheduler code
    // Return after switch             <--------------------------------
    movq (%rsi), %rsp 

    // Load registers from stack
    popq %rbp
    popq %rbx

    popq %r12
    popq %r13
    popq %r14
    popq %r15

    retq

    // For example: return to fiber code