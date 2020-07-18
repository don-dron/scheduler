#pragma once

struct spinlock;
typedef struct spinlock spinlock;

struct spinlock
{
    int lock;
};

static void lock_spinlock(spinlock *spin_lock)
{
    while (__atomic_exchange_n(&spin_lock->lock, 1, __ATOMIC_SEQ_CST))
    {
        while (__atomic_load_n(&spin_lock->lock, __ATOMIC_SEQ_CST))
        {
            usleep(1);
        }
    }
}

static void unlock_spinlock(spinlock *spin_lock)
{
    __atomic_store_n(&spin_lock->lock, 0, __ATOMIC_SEQ_CST);
}

static int try_lock_spinlock(spinlock *spin_lock)
{
    return !__atomic_exchange_n(&spin_lock->lock, 1, __ATOMIC_SEQ_CST);
}