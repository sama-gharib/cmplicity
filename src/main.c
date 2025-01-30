#include "automaton.h"
#include "regex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char ** argv) {

	if (argc != 4) {
		printf("Usage : cmp ['automaton' filepath | 'regex' your_regex] word_to_test\n");

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

	} else {
		fprintf(stderr, "Unknown mode : '%s'\n", argv[1]);
		exit_code = EXIT_FAILURE;
	}

	return exit_code;

	/*
	// Regex testing code

	printf("Compiling regex...\n");
	Automaton test = CompileRegex("[a-z]*\\.[a-z]*(@(gmail\\.com|outlook\\.fr))?");
	printf("Regex compiled !\n");

	printf("Initial : %p\n", test.initial);
	for (size_t i = 0; i < test.states.length; i++) {
		State* current_state = (State*) test.states.content[i].data;
		if (current_state->final) {
			printf("%p is final\n", current_state);
		}
		for (size_t j = 0; j < current_state->successors.length; j++) {
			Transition* current_transition = (Transition*) current_state->successors.content[j].data;
			printf("(%p, %c, %c, %p)\n", current_state, current_transition->min, current_transition->max, current_transition->target);
		}
	}

	bool matches[] = {
		Match(test.initial, "sama.gharib", 0),
		Match(test.initial, "sama.gharib@gmail.com", 0),
		Match(test.initial, "sama.gharib@", 0)
	};

	UnloadAutomaton(&test);

	printf("Results: %d %d %d\n", matches[0], matches[1], matches[2]);

	return EXIT_SUCCESS;
	*/

	/*
	// Automaton Star test code
	Automaton quote = LoadAutomaton("\
States: OpeningQuote, Content, ClosingQuote;\
Finals: ClosingQuote;\
Initial: OpeningQuote;\
Transitions:\
	(OpeningQuote, \", Content),\
	(Content, [a-z], Content),\
	(Content, \", ClosingQuote);\
");

	Automaton value = AutomatonPlus(&quote);

	bool matches[] = {
		Match(value.initial, "\"abc\"\"abc\"", 0),
		Match(value.initial, "\"hello\"", 0),
		Match(value.initial, "", 0)
	};

	UnloadAutomaton(&value);

	printf("Results: %d %d %d\n", matches[0], matches[1], matches[2]);

	return EXIT_SUCCESS;
	*/

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
		Match(value.initial, "+123.145", 0),
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

	/*
	// Automaton Concatenation test code
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
	Automaton value = AutomatonConcatenation(&nbr, &quote);


	bool matches[] = {
		Match(value.initial, "+123.145\"dab\"", 0),
		Match(value.initial, "\"hello\"87", 0),
		Match(value.initial, "999", 0)
	};

	UnloadAutomaton(&value);

	printf("Results: %d %d %d\n", matches[0], matches[1], matches[2]);

	return EXIT_SUCCESS;
	*/
}