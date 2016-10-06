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
	long index, buf_offset, buf_size, buf_len;
	scope_stack* scope;
	data_stack* data[2];
	data_stack** active_stack;
	long long current_value;
	int status;
} interpreter;

interpreter* interpreter_new(FILE* source, data_stack* on, data_stack* off) {
	interpreter* interp = (interpreter*) malloc(sizeof(interpreter));
	interp->source = source;
	interp->buf = NULL;
	interp->index = 0;
	interp->buf_offset = 0;
	interp->buf_size = 0;
	interp->buf_len = -1;
	interp->scope = NULL;
	interp->data[0] = on;
	interp->data[1] = off;
	interp->active_stack = interp->data;
	interp->current_value = 0;
	interp->status = 0;
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

void interpreter_buf_push(interpreter* interp, int c) {
	if (interp->buf_len == -1) return;
	if (interp->buf_len >= interp->buf_size) {
		if (interp->buf_size == 0) {
			interp->buf_size = 4;
		} else {
			interp->buf_size *= 2;
		}
		interp->buf = (char*) realloc(interp->buf, sizeof(char)*interp->buf_size);
	}
	interp->buf[interp->buf_len] = c;
	++interp->buf_len;
}

int interpreter_getc(interpreter* interp) {
	int c;
	if (interp->index >= interp->buf_offset + interp->buf_len) {
		c = fgetc(interp->source);
		if (c != EOF) {
			interpreter_buf_push(interp, c);
		}
	} else {
		c = interp->buf[interp->index - interp->buf_offset];
	}
	return c;
}

int interpreter_skip_loop(interpreter* interp, char first) {
	char* buf;
	int c = first, first_iter = 1;
	long buf_size = 4, buf_pos = 0;
	buf = (char*) malloc(sizeof(char)*buf_size);
	buf[0] = '{';
	while (buf_pos >= 0) {
		if (!first_iter) {
			++interp->index;
			c = interpreter_getc(interp);
		}
		first_iter = 0;
		if (c == EOF) {
			free(buf);
			// error: unmatched opening brace(s)
			return 3;
		}
		if (is_open_brace(c)) {
			++buf_pos;
			if (buf_pos == buf_size) {
				buf_size *= 2;
				buf = (char*) realloc(buf, sizeof(char)*buf_size);
			}
			buf[buf_pos] = c;
		} else if (is_matching_brace(buf[buf_pos], c)) {
			--buf_pos;
		} else if (is_close_brace(c)) {
			// error: mismatched braces
			free(buf);
			return 1;
		}
	}
	free(buf);
	return 0;
}

int interpreter_run(interpreter* interp) {
	unsigned curly_depth = 0;
	long last_index = -1;
	char next, last = 0;
	if (interp->status != 0) return interp->status;
	while(!feof(interp->source)) {
		next = interpreter_getc(interp);
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
					--curly_depth;
					if (curly_depth == 0) {
						interp->buf_len = -1;
					}
					break;
				case '<':
					if (interp->active_stack == interp->data) {
						interp->active_stack = interp->data + 1;
					} else {
						interp->active_stack = interp->data;
					}
					break;
				default:
					// should never happen but erroring here helps with debugging
					interp->status = -1;
					return interp->status;
			}
			last = 0;
		} else if (is_close_brace(next)) {
			if (last) {
				// error: mismatched braces
				interp->status = 1;
				return interp->status;
			}
			if (!interp->scope) {
				// error: unmatched closing brace
				interp->status = 2;
				return interp->status;
			}
			if (!is_matching_brace(interp->scope->symbol, next)) {
				// error: mismatched braces
				interp->status = 1;
				return interp->status;
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
						interp->index = interp->scope->index;
						interp->scope->current_value += interp->current_value;
						interp->current_value = 0;
					} else {
						--curly_depth;
						if (curly_depth == 0) {
							interp->buf_len = -1;
						}
					}
					break;
				default:
					// should never happen but erroring here may help with debugging
					interp->status = -2;
					return interp->status;
			}
			if (next != '}' || !data_stack_peek(*interp->active_stack)) {
				interp->current_value += interp->scope->current_value;
				interp->scope = scope_stack_pop(interp->scope);
			}
		} else if (is_open_brace(next)) {
			if (last == '{' && !data_stack_peek(*interp->active_stack)) {
				--curly_depth;
				if (curly_depth == 0) {
					interp->buf_len = -1;
				}
				if ((interp->status = interpreter_skip_loop(interp, next)) != 0) {
					// error: various errors in skip loop
					interp->status += 4;
					return interp->status;
				}
				next = 0;
			} else if (last) {
				interp->scope = scope_stack_push(interp->scope, interp->current_value, last, last_index);
				interp->current_value = 0;
			}
			// this tells the program to start storing code in buffer
			if (next == '{') {
					if (interp->buf_len == -1) {
						interp->buf_offset = interp->index + 1;
						interp->buf_len = 0;
					}
					++curly_depth;
			}
			last = next;
			last_index = interp->index;
		}
		++interp->index;
	}
	if (last || interp->scope) {
		// error: unmatched opening brace(s)
		interp->status = 3;
		return interp->status;
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

