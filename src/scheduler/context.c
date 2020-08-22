#include <scheduler/context.h>

unsigned long switch_count_atom = 0;
unsigned long switch_count = 0;
unsigned long interrupt_count = 0;
unsigned long interrupt_failed_count = 0;

inline void switch_context(execution_context *from, execution_context *to)
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
  stat.interrupt_count = interrupt_count;
  stat.interrupt_failed_count = interrupt_failed_count;
  return stat;
}

void print_statistic()
{
  statistic stat = get_statistic();
  printf("Atomic switch counter   %ld \n", stat.switch_count_atom);
  printf("Switch counter   %ld \n", stat.switch_count);
  printf("Interrupted routines %ld\n", stat.interrupt_count);
  printf("Interrupt failed count %ld\n", stat.interrupt_failed_count);
}