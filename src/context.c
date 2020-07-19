#pragma once

#include <context.h>

void AlignNextPush(stack_builder *builder, size_t alignment)
{
  size_t shift = (size_t)(builder->top - builder->word_size) % alignment;
  builder->top -= shift;
}

void Allocate(stack_builder *builder, size_t bytes)
{
  builder->top -= bytes;
}

size_t PagesToBytes(size_t count)
{
  static const size_t kPageSize = 4096;

  return count * kPageSize;
}