#pragma once

#include <scheduler/context.h>

unsigned long switch_count_atom = 0;
unsigned long switch_count = 0;

extern inline void switch_context(execution_context *from, execution_context *to)
{
  // Data Race (atomic operations is syncronization - lows performance)
  switch_count++;
  switch_from_to(from, to);
}

void align_next_push(stack_builder *builder, size_t alignment)
{
  size_t shift = (size_t)(builder->top - builder->word_size) % alignment;
  builder->top -= shift;
}

void allocate(stack_builder *builder, size_t bytes)
{
  builder->top -= bytes;
}

size_t pages_to_bytes(size_t count)
{
  static const size_t kPageSize = 4096;

  return count * kPageSize;
}

statistic get_statistic()
{
  statistic stat;
  stat.switch_count_atom = switch_count_atom;
  stat.switch_count = switch_count;
  return stat;
}

void print_statistic()
{
  statistic stat = get_statistic();
  printf("Atomic switch counter   %d \n", stat.switch_count_atom);
  printf("Switch counter   %d \n", stat.switch_count);
}