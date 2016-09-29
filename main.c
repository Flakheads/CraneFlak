#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "interpreter.h"
#include "stack.h"

void print_help_string(char* prog) {
	fprintf(stderr, "Usage: %s [switches] [--] [programfile] [arguments]\n"
	                "  -a          Convert arguments to their ASCII codepoint values and output in\n"
	                "                decimal. This overrides previous -A and -c flags.\n"
	                "  -A          Treat arguments as decimal numbers and convert output to ASCII\n"
	                "                text, modding by 256 before doing so. This overrides previous -a\n"
	                "                and -c flags.\n"
					"  -c          Convert arguments to their ASCII codepoint values and convert\n"
	                "                output to ASCII text, modding by 256 before doing so. This\n"
	                "                overrides previous -a and -A flags.\n"
	                "  -e program  Runs specified program instead of reading from the source file.\n"
	                "  -f file     Read arguments for Brain-Flak program from the specified file,\n"
	                "                ignore arguments from command line.\n"
	                "  -h          Display this information.\n"
	                "  -v          Display interpreter version info.\n",
	prog);
}

int main(int argc, char* argv[]) {
	FILE* source, *args;
	uint8_t ascii_in = 0, ascii_out = 0;
	int i, interp_err;
	unsigned j;
	long long arg;
	char c, *arg_file = NULL, *program = NULL, *check;
	data_stack* arg_stack, *result_stack;
	interpreter* interp;
	while ((c = getopt(argc, argv, "+aAce:f:hv")) != -1) {
		switch (c) {
			case 'a':
				ascii_in  = 1;
				ascii_out = 0;
				break;
			case 'A':
				ascii_in  = 0;
				ascii_out = 1;
				break;
			case 'c':
				ascii_in  = 1;
				ascii_out = 1;
				break;
			case 'e':
				program = optarg;
				break;
			case 'f':
				arg_file = optarg;
				break;
			case 'h':
				print_help_string(argv[0]);
				return 0;
			case 'v':
				fprintf(stderr, "CraneFlak version 1.0.0 DEV\n");
				return 0;
			case '?':
				fprintf(stderr, "Try '%s -h' for more information.\n", argv[0]);
				return 1;
		}
	}
	if (program != NULL) {
		source = fmemopen(program, strlen(program), "r");
	} else if (optind == argc) {
		source = stdin;
	} else {
		source = fopen(argv[optind], "r");
		if (!source) {
			fprintf(stderr, "%s: %s -- '%s'", argv[0], strerror(errno), argv[optind]);
			return errno;
		}
		++optind;
	}
	if (arg_file != NULL) {
		args = fopen(arg_file, "r");
		if (!args) {
			fprintf(stderr, "%s: %s -- '%s'", argv[0], strerror(errno), arg_file);
			if (source != stdin) fclose(source);
			return errno;
		}
		if (ascii_in) {
			fseek(args, 0L, SEEK_END);
			while (fseek(args, -1L, SEEK_CUR) == 0) {
				arg = (long long) fgetc(args);
				arg_stack = data_stack_push(arg_stack, arg);
			}
			// fseek sets errno to EINVAL when the offset would go negative
			errno = 0;
		} else {
			while (!feof(args)) {
				if (!fscanf(args, " %lli ", &arg)) {
					fprintf(stderr, "%s: invalid integer argument in argument file -- %s\n", argv[0], arg_file);
					fclose(args);
					if (source != stdin) fclose(source);
					return 1;
				}
				arg_stack = data_stack_push(arg_stack, arg);
			}
			arg_stack = data_stack_reverse(arg_stack);
		}
		fclose(args);
	} else {
		for (i = argc - 1; i >= optind; --i) {
			if (ascii_in) {
				for (j = strlen(argv[i]); j > 0; --j) {
					arg_stack = data_stack_push(arg_stack, (long long) argv[i][j]);
				}
				if (i != optind) arg_stack = data_stack_push(arg_stack, (long long) ' ');
			} else {
				arg = strtol(argv[i], &check, 0);
				if (*check != '\0') {
					fprintf(stderr, "%s: invalid integer argument -- '%s'\n", argv[0], argv[i]);
					if (source != stdin) fclose(source);
					return 1;
				}
				arg_stack = data_stack_push(arg_stack, arg);
			}
		}
	}
	interp = interpreter_new(source, arg_stack, NULL);
	interp_err = interpreter_run(interp);
	if (interp_err) {
		// TODO make human readable error messages
		fprintf(stderr, "Interpreter error %d\n", interp_err);
		interpreter_free(interp);
		if (source != stdin) fclose(source);
		return interp_err;
	}
	result_stack = interpreter_remove_active_stack(interp);
	while (result_stack) {
		if (ascii_out) {
			printf("%c", (char) (data_stack_peek(result_stack) % 256));
		} else {
			printf("%lld\n", data_stack_peek(result_stack));
		}
		result_stack = data_stack_pop(result_stack);
	}
	interpreter_free(interp);
	if (source != stdin) {
		fclose(source);
	}
	return 0;
}
