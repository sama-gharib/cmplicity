#include <stdlib.h>
#include <stdio.h>
#include "lexing.h"
#include "vector.h"

#define AUTOMATON_VECTOR_BUFFER_SIZE 4

LexingToken * Tokenize(char * regex, char * code) {
	
	AutomatonVector exprs = LoadRegex(regex);
	if (exprs.length == 0) {
		DestroyAutomatonVector(&exprs);
		fprintf(stderr, "Failed to lead regex from '%s'\n", regex);
		return NULL;
	}

	FILE * src = fopen(code, "r");
	if (src == NULL) {
		fprintf(stderr, "Could not open file '%s'\n", code);
		return NULL;
	}

	size_t char_num = 0;

	const size_t result_buffer_size = 8;
	LexingToken * tokens = malloc(sizeof(LexingToken) * (result_buffer_size + 1));
	size_t tokens_count = 0;

	while (true) {

		LexingToken new_token = GetNextToken(exprs, src);
		if (new_token.begin == new_token.end) {
			fprintf(stderr, "Unknown token at character %llu: Interpreting as EOF\n", char_num);
			break;
		} else {
			new_token.begin = char_num;
			new_token.end += char_num;
			char_num = new_token.end - 1;
			if (tokens_count != 0 && tokens_count%result_buffer_size == 0) {
				tokens = realloc(tokens, sizeof(LexingToken) * (tokens_count + result_buffer_size + 1/*So that we can add EOF token later*/));
			}
			tokens[tokens_count] = new_token;
			tokens_count++;
		}

		char_num++;
	}

	// Adding EOF token
	tokens[tokens_count] = (LexingToken) {char_num, char_num + 1, "EOF"};

	fclose(src);
	DestroyAutomatonVector(&exprs);

	return tokens;
}

LexingToken GetNextToken(AutomatonVector automatons, FILE * source) {
	ParsingBuffer buffer = InitParsingBuffer();

	size_t i = 0;
	char current = '\0';
	LexingToken to_return = {0, 0, "Unknown Token"};

	size_t initial = ftell(source);

	while (i < PARSING_BUFFER_SIZE && fread(&current, 1, 1, source)) {

		for (size_t a = 0; a < automatons.length; a++) {
			if (Match(automatons.data[a].initial, buffer.content, 0)) {
				strcpy(to_return.id, automatons.names[a]);
				to_return.end = i;
			}
		}
		PushParsingBuffer(&buffer, current);

		i++;
	}
	fseek(source, initial + to_return.end, 0);

	return to_return;
}

AutomatonVector CreateAutomatonVector() {
	return (AutomatonVector) {
		malloc(sizeof(Automaton) * AUTOMATON_VECTOR_BUFFER_SIZE),
		malloc(sizeof(char*) * AUTOMATON_VECTOR_BUFFER_SIZE),
		0
	};
}
void DestroyAutomatonVector(AutomatonVector * av) {
	for (size_t i = 0; i < av->length; i++) {
		UnloadAutomaton(&av->data[i]);
		free(av->names[i]);
	}

	free(av->data);
	free(av->names);
	av->data = NULL;
	av->names = NULL;
	av->length = 0;
}
void PushAutomatonVector(AutomatonVector * av, Automaton a, char * n) {
	if (av->length%AUTOMATON_VECTOR_BUFFER_SIZE == 0 && av->length != 0) {
		size_t new_length = (av->length + AUTOMATON_VECTOR_BUFFER_SIZE);
		av->data = realloc(av->data, sizeof(Automaton) * new_length);
		av->names = realloc(av->data, sizeof(char*) * new_length);
	}
	av->data[av->length] = a;
	av->names[av->length] = AllocateString(n);
	av->length ++;
}

AutomatonVector LoadRegex(char * src) {
	FILE * stream = fopen(src, "r");
	AutomatonVector parsed = CreateAutomatonVector();

	if (stream == NULL) {
		fprintf(stderr, "Could not open file '%s'\n", src);
		return parsed;	
	}

	ParsingBuffer name_buffer = InitParsingBuffer();
	ParsingBuffer expr_buffer = InitParsingBuffer();
	char current = '\0';

	int state = 0;

	while (true) {
		
		bool read = fread(&current, 1, 1, stream);

		switch(state) {
			case 0:
				if (!read) {
					goto outside;
				} else if (current == ' ') {
					state = 1;
				} else {
					PushParsingBuffer(&name_buffer, current);
				}
			break;
			case 1:
				if (!read || current == '\n') {
					PushAutomatonVector(&parsed, CompileRegex(expr_buffer.content), name_buffer.content);
					ClearParsingBuffer(&name_buffer);
					ClearParsingBuffer(&expr_buffer);
					state = 0;
					if (!read) {
						goto outside;
					}
				} else {
					PushParsingBuffer(&expr_buffer, current);
				}
			break;
		}
	}
	outside:

	fclose(stream);

	return parsed;
}

char * AllocateString(char * s) {
	size_t size = 0;
	while (s[size] != '\0') {
		size ++;
	}
	size ++;
	char * new_string = malloc(sizeof(char) * size);
	memcpy(new_string, s, size);
	return new_string;
}