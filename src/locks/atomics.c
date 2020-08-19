#include <locks/atomics.h>

inline unsigned long inc(unsigned long *variable)
{
    return __atomic_fetch_add(variable, 1, __ATOMIC_SEQ_CST);
}

inline unsigned long dec(unsigned long *variable)
{
    return __atomic_fetch_sub(variable, 1, __ATOMIC_SEQ_CST);
}
