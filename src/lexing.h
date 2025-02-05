#ifndef LEXING_H_INCLUDED
#define LEXING_H_INCLUDED

#include <stdio.h>
#include "automaton.h"
#include "regex.h"

typedef struct {
	Automaton * data;
	char ** names;
	size_t length;

} AutomatonVector;

typedef struct {
	size_t begin;
	size_t end;

	char id[64];
} LexingToken;

LexingToken * Tokenize(char * regex, char * code);

LexingToken GetNextToken(AutomatonVector, FILE *, size_t);

AutomatonVector CreateAutomatonVector();
void DestroyAutomatonVector(AutomatonVector*);
void PushAutomatonVector(AutomatonVector*, Automaton, char*);

AutomatonVector LoadRegex(char * src);

char * AllocateString(char *);

#endif