#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
	                "  -e program  Runs the specified program instead of reading from the source file.\n"
	                "  -f file     Read arguments for Brain-Flak program from the specified file,\n"
	                "                ignore arguments from command line.\n"
	                "  -h          Display this information.\n"
	                "  -v          Display interpreter version info.\n",
	prog);
}

int main(int argc, char* argv[]) {
	FILE* source, *args;
	uint8_t ascii_in = 0, ascii_out = 0;
	int i;
	long arg;
	char c, *arg_file = NULL, *program = NULL, *check;
	data_stack* arg_stack;
	while ((c = getopt(argc, argv, "+aAcef:hv")) != -1) {
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
		//arg_stack = read_args_from_file(arg_file);
	} else {
		for (i = argc - 1; i >= optind; --i) {
			arg = strtol(argv[i], &check, 0);
			if (*check != '\0') {
				fprintf(stderr, "%s: invalid integer argument -- '%s'\n", argv[0], argv[i]);
				if (source != stdin) fclose(source);
				return 1;
			}
			arg_stack = data_stack_push(arg_stack, arg);
		}
	}

	// Stuff

	if (source != stdin) {
		fclose(source);
	}
	return 0;
}
