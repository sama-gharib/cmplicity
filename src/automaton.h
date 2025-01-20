#ifndef AUTOMATON_H_INCLUDED
#define AUTOMATON_H_INCLUDED

#include <stdbool.h>
#include "vector.h"

typedef struct {
	Vector successors;
	bool final;
} State;

typedef struct {
	char min;
	char max;

	State * target;

} Transition;

Transition ClassicTransition(char c, State * target);
Transition RangeTransition(char min, char max, State * target);
State CreateState(bool f);
void DestroyState(State* s);
void AddTransition(State * s, Transition t);
bool Match(State * s, char * word, size_t cursor);

#endif