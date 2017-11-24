#ifndef __interpreter_h_
#define __interpreter_h_

#include <stdio.h>
#include "stack.h"

enum {
  SUCCESS = 0,
  ERR_INTERP_BUG_NILAD = -1,
  ERR_INTERP_BUG_MONAD = -2,
  ERR_MISMATCHED_BRACES = 1,
  ERR_UNMATCHED_CLOSE_BRACE = 2,
  ERR_UNMATCHED_OPEN_BRACE = 3,
  ERR_ADJACENT_MISMATCHED_BRACES = 4,
  ERR_MISMATCHED_SKIPPED_LOOP = 5,
  ERR_UNMATCHED_OPEN_BRACES = 7
} interpreter_status;

typedef struct interpreter interpreter;

interpreter* interpreter_new(FILE* source, data_stack* on, data_stack* off);

int interpreter_print_status(interpreter* interp, FILE* out);

int interpreter_run(interpreter* interp);

// When this method is called the active stack is set to NULL in the interpreter to avoid potential memory problems
data_stack* interpreter_remove_active_stack(interpreter* interp);

void interpreter_free(interpreter* interp);

#endif //__interpreter_h_