#pragma once

#include <locks/wait_group.h>
#include <locks/atomics.h>

typedef struct wait_group
{
    unsigned int count;
} wait_group;

wait_group init();

void add(wait_group* group);

void done(wait_group* group);

void wait(wait_group* group);
