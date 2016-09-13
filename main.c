#include <stdio.h>

void print_help_string(char* prog) {
	fprintf(stderr, "CraneFlak: a Brain-Flak Interpreter in C\n"
	                "Usage: %s [options] file [args]...\n"
	                "Options:\n"
	                "  -f <file>   Read arguments for Brain-Flak program from the specified file,\n"
	                "                ignore arguments from command line.\n"
	                "  -a          Convert arguments to their ASCII codepoint values and output in\n"
	                "                decimal. This overrides previous -A and -c flags.\n"
	                "  -A          Treat arguments as decimal numbers and convert output to ASCII\n"
	                "                text, modding by 256 before doing so. This overrides previous -a\n"
	                "                and -c flags.\n"
					"  -c          Convert arguments to their ASCII codepoint values and convert\n"
	                "                output to ASCII text, modding by 256 before doing so. This\n"
	                "                overrides previous -a and -A flags.\n"
	                "  -h          Display this information.\n"
	                "  -v          Display interpreter version info.\n",
	prog);
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "%1$s: missing source file\nTry '%1$s -h' for more information.\n", argv[0]);
		return 1;
	}
	return 0;
}
