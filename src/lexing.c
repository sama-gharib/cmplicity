#include <stdlib.h>
#include <stdio.h>
#include "lexing.h"

#define AUTOMATON_VECTOR_BUFFER_SIZE 4

AutomatonVector CreateAutomatonVector() {
	return (AutomatonVector) {
		malloc(sizeof(Automaton) * AUTOMATON_VECTOR_BUFFER_SIZE),
		0
	};
}
void DestroyAutomatonVector(AutomatonVector * av) {
	for (size_t i = 0; i < av->length; i++) {
		UnloadAutomaton(&av->data[i]);
	}

	free(av->data);
	av->data = NULL;
	av->length = 0;
}
void PushAutomatonVector(AutomatonVector * av, Automaton a) {
	if (av->length%AUTOMATON_VECTOR_BUFFER_SIZE == 0 && av->length != 0) {
		av->data = realloc(av->data, sizeof(Automaton) * (av->length + AUTOMATON_VECTOR_BUFFER_SIZE));
	}
	av->data[av->length] = a;
	av->length ++;
}

AutomatonVector LoadRegex(char * src) {
	FILE * stream = fopen(src, "r");
	if (stream == NULL) {
		fprintf(stderr, "Could not open file '%s'\n", src);
	}

	ParsingBuffer buffer = InitParsingBuffer();
	char current = '\0';
	AutomatonVector parsed = CreateAutomatonVector();

	while (fread(&current, 1, 1, stream)) {
		if (current == '\n') {
			PushAutomatonVector(&parsed, CompileRegex(buffer.content));
			ClearParsingBuffer(&buffer);
		} else {
			PushParsingBuffer(&buffer, current);
		}
	}

	return parsed;
}