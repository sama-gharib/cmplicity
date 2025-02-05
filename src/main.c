#include "automaton.h"
#include "regex.h"
#include "lexing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char ** argv) {

	if (argc != 4) {
		printf("Usage : cmp ['automaton' filepath | 'regex' your_regex | 'lexing' regex_file code_file] word_to_test\n");

		return EXIT_FAILURE;
	}

	int exit_code = EXIT_SUCCESS;

	if (strcmp(argv[1], "automaton") == 0) {

		FILE * source = fopen(argv[2], "r");
		if (source == NULL) {
			fprintf(stderr, "Could not read file '%s'\n", argv[2]);
			return EXIT_FAILURE;
		}

		// Calculating file size
		fseek(source, 0, SEEK_END);
		size_t file_length = ftell(source);
		rewind(source);

		// Reading file
		char * file_content = malloc(sizeof(char) * (file_length + 1));
		file_content[file_length] = '\0';
		size_t read_cursor = 0;
		while (fread(file_content+read_cursor, 1, 1, source)) {
			read_cursor ++;
		}
		fclose(source);

		Automaton test = LoadAutomaton(file_content);

		free(file_content);

		if (test.initial == NULL) {
			printf("Could not load automaton.\n");
			exit_code = EXIT_FAILURE;
		} else {
			printf("The word '%s' is ", argv[3]);
			if (Match(test.initial, argv[3], 0)) {
				printf("recognized.\n");
			} else {
				printf("not recognized.\n");
			}

			UnloadAutomaton(&test);
		}
	} else if (strcmp(argv[1], "regex") == 0) {
		Automaton regex = CompileRegex(argv[2]);

		printf("Your regex did ");
		if (Match(regex.initial, argv[3], 0)) {
			printf("successfully ");
		} else {
			printf("not ");
		}
		printf("match the word '%s'.\n", argv[3]);

		UnloadAutomaton(&regex);

	} else if (strcmp(argv[1], "lexing") == 0) {

		LexingToken * tokens = Tokenize(argv[2], argv[3]);
		size_t index = 0;
		while ( strcmp(tokens[index].id, "EOF") != 0 ) {
			printf("%s from %llu to %llu\n", tokens[index].id, tokens[index].begin, tokens[index].end);
			index++;
		}
		printf("EOF from %llu to %llu\n", tokens[index].begin, tokens[index].end);
		free(tokens);

	} else {
		fprintf(stderr, "Unknown mode : '%s'\n", argv[1]);
		exit_code = EXIT_FAILURE;
	}

	return exit_code;

}