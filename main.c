#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>

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
	uint8_t ascii_in = 0, ascii_out = 0;
	char c, *arg_file = NULL, *program = NULL;
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
	return 0;
}
