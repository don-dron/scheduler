#pragma once

#include <unistd.h>

typedef struct spinlock
{
    int lock;
} spinlock;

void lock_spinlock(spinlock *spin_lock);

void unlock_spinlock(spinlock *spin_lock);

int try_lock_spinlock(spinlock *spin_lock);