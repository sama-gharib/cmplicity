#ifndef LEXING_H_INCLUDED
#define LEXING_H_INCLUDED

#include "automaton.h"
#include "regex.h"

typedef struct {
	Automaton * data;
	size_t length;

} AutomatonVector;

AutomatonVector CreateAutomatonVector();
void DestroyAutomatonVector(AutomatonVector*);
void PushAutomatonVector(AutomatonVector*, Automaton);

AutomatonVector LoadRegex(char * src);

#endif