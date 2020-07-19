#pragma once

#include <stdio.h>
#include "stdlib.h"
#include <sys/mman.h>

// Fiber , coroutine stack size = 64 Kbytes
static const size_t STACK_SIZE = 64 * 1024;

struct execution_context;
typedef struct execution_context execution_context;

struct stack_saved_context;
typedef struct stack_saved_context stack_saved_context;

struct stack;
typedef struct stack stack;

struct stack_builder;
typedef struct stack_builder stack_builder;

struct execution_context
{
  void *rsp;
};

struct stack_saved_context
{
  void *rbp;
  void *rbx;

  void *r12;
  void *r13;
  void *r14;
  void *r15;

  void *rip;
};

struct stack
{
  void *memory;
  size_t size;
};

struct stack_builder
{
  int word_size;
  char *top;
};

extern unsigned long switch_context(execution_context *from, execution_context *to);

void AlignNextPush(stack_builder *builder, size_t alignment);

void Allocate(stack_builder *builder, size_t bytes);

size_t PagesToBytes(size_t count);