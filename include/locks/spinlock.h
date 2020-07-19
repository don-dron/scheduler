#pragma once

#include <unistd.h>

struct spinlock;
typedef struct spinlock spinlock;

struct spinlock
{
    int lock;
};

void lock_spinlock(spinlock *spin_lock);

void unlock_spinlock(spinlock *spin_lock);

int try_lock_spinlock(spinlock *spin_lock);