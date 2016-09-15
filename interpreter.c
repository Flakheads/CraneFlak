#include "interpreter.h"

#include <inttypes.h>
#include <stdlib.h>

typedef struct scope_stack {
	long long current_value;
	char symbol;
	long index;
	struct scope_stack* next;
} scope_stack;

scope_stack* scope_stack_push(scope_stack* stack, long long current_value, char symbol, long index) {
	scope_stack* new_top = (scope_stack*) malloc(sizeof(scope_stack));
	new_top->current_value = current_value;
	new_top->symbol = symbol;
	new_top->index = index;
	new_top->next = stack;
	return new_top;
}

scope_stack* scope_stack_pop(scope_stack* stack) {
	return stack ? stack->next : NULL;
}

void scope_stack_free(scope_stack* stack) {
	while (stack) stack = scope_stack_pop(stack);
}

typedef struct interpreter {
	FILE* source;
	scope_stack* scope;
	data_stack* data[2];
	data_stack** active_stack;
	long long current_value;
} interpreter;

void interpreter_init(interpreter* interp, FILE* source, data_stack* on, data_stack* off) {
	interp->source = source;
	interp->data[0] = on;
	interp->data[1] = off;
	interp->active_stack = interp->data;
	interp->current_value = 0;
}

int interpreter_run(interpreter* interp) {
	return 0; // TODO write more here
}

data_stack* interpreter_remove_active_stack(interpreter* interp) {
	data_stack* active_stack = *interp->active_stack;
	*interp->active_stack = NULL;
	return active_stack;
}

void interpreter_free(interpreter* interp) {
	scope_stack_free(interp->scope);
	data_stack_free(interp->data[0]);
	data_stack_free(interp->data[1]);
}

