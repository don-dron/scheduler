#pragma once

#include <stdio.h>

#ifndef CONTEXT
#define CONTEXT
 
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

void Setup(void (*Trampoline)(),ExecutionContext* context);

#endif