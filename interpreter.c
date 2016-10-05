#include "interpreter.h"

#include <errno.h>
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
	scope_stack* new_top;
	if (!stack) return NULL;
	new_top = stack->next;
	free(stack);
	return new_top;
}

void scope_stack_free(scope_stack* stack) {
	while (stack) stack = scope_stack_pop(stack);
}

typedef struct interpreter {
	FILE* source;
	char* buf;
	long buf_size, buf_len, buf_index;
	scope_stack* scope;
	data_stack* data[2];
	data_stack** active_stack;
	long long current_value;
	int status;
} interpreter;

interpreter* interpreter_new(FILE* source, data_stack* on, data_stack* off) {
	int errno_tmp = errno;
	interpreter* interp = (interpreter*) malloc(sizeof(interpreter));
	interp->source = source;
	errno = 0;
	// if source is not seekable then set up a buffer
	if (fseek(source, 0L, SEEK_CUR) == -1 && errno == EBADF) {
		interp->buf_size = 0;
		interp->buf_len = 0;
		interp->buf_index = 0;
		interp->buf = NULL;
	} else {
		interp->buf_size = -1;
	}
	errno = errno_tmp;
	interp->data[0] = on;
	interp->data[1] = off;
	interp->active_stack = interp->data;
	interp->current_value = 0;
	return interp;
}

int is_open_brace(char c) {
	return c == '(' || c == '[' || c == '{' || c == '<';
}

int is_close_brace(char c) {
	return c == ')' || c == ']' || c == '}' || c == '>';
}

int is_matching_brace(char b1, char b2) {
	return (b1 == '(' && b2 == ')') || (b1 == '[' && b2 == ']') || (b1 == '{' && b2 == '}') || (b1 == '<' && b2 == '>');
}

int skip_loop(FILE* source) {
	char c;
	int paren_scope = 0, squar_scope = 0, curly_scope = 1, angle_scope = 0;
	while (curly_scope != 0) {
		if (feof(source)) {
			return 1;
		}
		c = fgetc(source);
		switch (c) {
			case '(':
				++paren_scope;
				break;
			case ')':
				--paren_scope;
				break;
			case '[':
				++squar_scope;
				break;
			case ']':
				--squar_scope;
				break;
			case '{':
				++curly_scope;
				break;
			case '}':
				--curly_scope;
				break;
			case '<':
				++angle_scope;
				break;
			case '>':
				--angle_scope;
				break;
		}
		if (paren_scope < 0 || squar_scope < 0 || angle_scope < 0) return 1;
	}
	if (paren_scope || squar_scope || angle_scope) return 1;
	return 0;
}

int interpreter_run(interpreter* interp) {
	char next, last = 0;
	while(!feof(interp->source)) {
		next = fgetc(interp->source);
		if (is_matching_brace(last, next)) { // nilads
			switch (last) {
				case '(':
					++interp->current_value;
					break;
				case '[':
					interp->current_value += data_stack_height(*interp->active_stack);
					break;
				case '{':
					interp->current_value += data_stack_peek(*interp->active_stack);
					*interp->active_stack = data_stack_pop(*interp->active_stack);
					break;
				case '<':
					if (interp->active_stack == interp->data) {
						interp->active_stack = interp->data + 1;
					} else {
						interp->active_stack = interp->data;
					}
					break;
				default:
					fprintf(stderr, "Error\n");
					// should never happen but erroring here helps with debugging
			}
			last = 0;
		} else if (is_close_brace(next)) {
			if (last) {
				// error mismatched braces
			}
			if (!interp->scope) {
				// error unmatched closing brace
			}
			if (!is_matching_brace(interp->scope->symbol, next)) {
				// error mismatched braces
			}
			switch(next) {
				case ')':
					*interp->active_stack = data_stack_push(*interp->active_stack, interp->current_value);
					break;
				case ']':
					interp->current_value *= -1;
					break;
				case '>':
					interp->current_value = 0;
					break;
				case '}':
					if (data_stack_peek(*interp->active_stack)) {
						fseek(interp->source, interp->scope->index, SEEK_SET);
						interp->scope->current_value += interp->current_value;
						interp->current_value = 0;
					}
					break;
				default:
					fprintf(stderr, "Error\n");
					// should never happen but erroring here may help with debugging
			}
			if (next != '}' || !data_stack_peek(*interp->active_stack)) {
				interp->current_value += interp->scope->current_value;
				interp->scope = scope_stack_pop(interp->scope);
			}
		} else if (is_open_brace(next)) {
			if (last == '{') {
				if (data_stack_peek(*interp->active_stack)) {
					scope_stack_push(interp->scope, interp->current_value, last, ftell(interp->source)); // index needs to be fixed
					interp->current_value = 0;
				} else {
					if (!skip_loop(interp->source)) {
						// error
					}
					next = 0;
				}
			} else if (last) {
				scope_stack_push(interp->scope, interp->current_value, last, ftell(interp->source)); // index needs to be fixed
				interp->current_value = 0;
			}
			last = next;
		}
	}
	if (last || interp->scope) {
		// error
	}
	return 0;
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
	if (interp->buf_size > 0) {
		free(interp->buf);
	}
	free(interp);
}

