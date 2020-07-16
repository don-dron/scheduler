#include "stdlib.h"
#include "context.h"

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

StackBuilder *NewStackBuilder(char *bottom)
{
  StackBuilder *stackBuilder = malloc(sizeof(StackBuilder));
  stackBuilder->top_ = bottom;
  stackBuilder->kWordSize = 8;
  return stackBuilder;
}

void AlignNextPush(StackBuilder* builder,size_t alignment)
{
  size_t shift = (size_t)(builder->top_ - builder->kWordSize) % alignment;
  builder->top_ -= shift;
}

void Push(StackBuilder* builder,void *value)
{
  builder->top_ -=  builder->kWordSize;
  (*builder->top_) = value;
}

void Allocate(StackBuilder* builder,size_t bytes)
{
  builder->top_ -= bytes;
}

void Setup(void (*trampoline)(),ExecutionContext* context)
{ 
  void * mem = malloc(1024*1024);

  StackBuilder* stackBuilder = NewStackBuilder(mem+1024*1024);
  AlignNextPush(stackBuilder,16);
  Allocate(stackBuilder,sizeof(StackSavedContext));


  StackSavedContext *saved_context = stackBuilder->top_;
  saved_context->rip = (void *)trampoline;

  context->rsp_ = saved_context;
}