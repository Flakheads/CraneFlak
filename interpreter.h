#ifndef __interpreter_h_
#define __interpreter_h_

#include <stdio.h>

#include "stack.h"

typedef struct interpreter interpreter;

void interpreter_init(interpreter* interp, FILE* source, data_stack* on, data_stack* off);

int interpreter_run(interpreter* interp);

// When this method is called the active stack is set to NULL in the interpreter to avoid potential memory problems
data_stack* interpreter_remove_active_stack(interpreter* interp);

void interpreter_free(interpreter* interp);

#endif//__interpreter_h_
