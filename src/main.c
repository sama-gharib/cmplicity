#include "automaton.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char ** argv) {

	if (argc != 4) {
		printf("Usage : cmp ['automaton' | 'regex'] filepath word_to_test\n");

		return EXIT_FAILURE;
	}

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

	int exit_code = EXIT_SUCCESS;

	if (strcmp(argv[1], "automaton") == 0) {

		Automaton test = LoadAutomaton(file_content);

		if (test.initial == NULL) {
			printf("Could not load automaton.\n");
			return EXIT_FAILURE;
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
		fprintf(stderr, "Mode regex is not yet implemented.\n");
		exit_code = EXIT_FAILURE;
	} else {
		fprintf(stderr, "Unknown mode : '%s'\n", argv[1]);
		exit_code = EXIT_FAILURE;
	}

	free(file_content);

	return exit_code;

	/*
	// Automaton Union test code
	Automaton nbr = LoadAutomaton("\
States: Sign, FirstDigitAfterSign, IntegerPart, FloatPart;\
Finals: IntegerPart, FloatPart;\
Initial: Sign;\
Transitions:\
    (Sign, +, FirstDigitAfterSign),\
    (Sign, -, FirstDigitAfterSign),\
    (FirstDigitAfterSign, [0-9], IntegerPart),\
    (Sign, [0-9], IntegerPart),\
    (IntegerPart, [0-9], IntegerPart),\
    (IntegerPart, ., FloatPart),\
    (FloatPart, [0-9], FloatPart);\
\
");
	Automaton quote = LoadAutomaton("\
States: OpeningQuote, Content, ClosingQuote;\
Finals: ClosingQuote;\
Initial: OpeningQuote;\
Transitions:\
	(OpeningQuote, \", Content),\
	(Content, [a-z], Content),\
	(Content, \", ClosingQuote);\
");
	Automaton value = AutomatonUnion(&nbr, &quote);

	bool matches[] = {
		Match(value.initial, "123.145", 0),
		Match(value.initial, "\"hello\"", 0),
		Match(value.initial, "yooo", 0)
	};

	printf("value.states : ");
	for(size_t i = 0; i < value.states.length; i++) {
		printf("%p ", value.states.content[i].data);
	}
	printf("\n");

	UnloadAutomaton(&value);

	printf("Results: %d %d %d\n", matches[0], matches[1], matches[2]);

	return EXIT_SUCCESS;
	*/
}