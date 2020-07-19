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

extern unsigned long switch_context(execution_context *from, execution_context *to);

struct stack
{
  void *memory;
  size_t size;
};

typedef struct stack stack;

char *bottom(stack *stack);

struct stack_builder
{
  int word_size;
  char *top;
};

typedef struct stack_builder stack_builder;

static void AlignNextPush(stack_builder *builder, size_t alignment)
{
  size_t shift = (size_t)(builder->top - builder->word_size) % alignment;
  builder->top -= shift;
}

static void Allocate(stack_builder *builder, size_t bytes)
{
  builder->top -= bytes;
}

static size_t PagesToBytes(size_t count)
{
  static const size_t kPageSize = 4096;

  return count * kPageSize;
}