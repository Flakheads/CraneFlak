#include "stack.h"

#include <stdlib.h>

typedef struct data_stack {
	unsigned height;
	long value;
	struct data_stack* next;
} data_stack;

data_stack* data_stack_push(data_stack* stack, long value) {
	data_stack* new_top = (data_stack*) malloc(sizeof(data_stack));
	new_top->height = stack ? stack->height + 1 : 1;
	new_top->value = value;
	new_top->next = stack;
	return new_top;
}

long data_stack_peek(data_stack* stack) {
	return stack ? stack->value : 0;
}

data_stack* data_stack_pop(data_stack* stack) {
	data_stack* new_top;
	if (!stack) return NULL;
	new_top = stack->next;
	free(stack);
	return new_top;
}

void data_stack_free(data_stack* stack) {
	while (stack) stack = data_stack_pop(stack);
}
