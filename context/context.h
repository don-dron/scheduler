#pragma once

#include <stdio.h>

#ifndef CONTEXT
#define CONTEXT
 
static const size_t STACK_SIZE = 1024*1024;

struct ExecutionContext;

typedef struct ExecutionContext ExecutionContext;

extern void SwitchContext(ExecutionContext* from, ExecutionContext* to);

struct ExecutionContext {
  void *rsp_; 
}; 

struct StackBuilder { 
  int kWordSize;
  char* top_;
};

typedef struct StackBuilder StackBuilder;

void* Setup(void (*Trampoline)(),ExecutionContext* context);

#endif