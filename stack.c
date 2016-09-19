#include "stack.h"

#include <stdlib.h>

typedef struct data_stack {
	long long height, value;
	struct data_stack* next;
} data_stack;

data_stack* data_stack_push(data_stack* stack, long long value) {
	data_stack* new_top = (data_stack*) malloc(sizeof(data_stack));
	new_top->height = stack ? stack->height + 1 : 1;
	new_top->value = value;
	new_top->next = stack;
	return new_top;
}

long long data_stack_peek(data_stack* stack) {
	return stack ? stack->value : 0;
}

long long data_stack_height(data_stack* stack) {
	return stack ? stack->height : 0;
}

data_stack* data_stack_pop(data_stack* stack) {
	data_stack* new_top;
	if (!stack) return NULL;
	new_top = stack->next;
	free(stack);
	return new_top;
}

data_stack* data_stack_reverse(data_stack* stack) {
	data_stack* next, *prev = NULL;
	while(stack != NULL) {
		next = stack->next;
		stack->next = prev;
		prev = stack;
		stack = next;
	}
	return prev;
}

void data_stack_free(data_stack* stack) {
	while (stack) stack = data_stack_pop(stack);
}
