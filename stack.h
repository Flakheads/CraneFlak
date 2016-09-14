#ifndef __stack_h_
#define __stack_h_

typedef struct data_stack data_stack;

data_stack* data_stack_push(data_stack* stack, long value);

long data_stack_peek(data_stack* stack);

data_stack* data_stack_pop(data_stack* stack);

data_stack* data_stack_reverse(data_stack* stack);

void data_stack_free(data_stack* stack);

#endif//ifndef __stack_h_
