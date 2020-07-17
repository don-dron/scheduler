#pragma once

#include <stdio.h>
#include "stdlib.h"
#include <sys/mman.h>



#ifndef CONTEXT
#define CONTEXT

struct ExecutionContext;

typedef struct ExecutionContext ExecutionContext;

extern unsigned long SwitchContext(ExecutionContext *from, ExecutionContext *to);

struct ExecutionContext
{
  void *rsp_;
  void *stack;
};

struct StackSavedContext
{
  void *rbp;
  void *rbx;

  void *r12;
  void *r13;
  void *r14;
  void *r15;

  void *rip;
};

typedef struct StackSavedContext StackSavedContext;

#endif