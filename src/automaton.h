#ifndef AUTOMATON_H_INCLUDED
#define AUTOMATON_H_INCLUDED

#include <stdbool.h>
#include <string.h>
#include "vector.h"

#define PARSING_BUFFER_SIZE 32

typedef struct {
	Vector successors;
	bool final;
} State;

typedef struct {
	char min;
	char max;

	State * target;

} Transition;

typedef struct {
	Vector states;
	State * initial;

} Automaton;

typedef struct {
	size_t length;
	char content[PARSING_BUFFER_SIZE];

} ParsingBuffer;

typedef enum {
	APSDefault     = 0b00001,
	APSStates      = 0b00010,
	APSInitial     = 0b00100,
	APSFinals      = 0b01000,
	APSTransitions = 0b10000

} AutomatonParserState;

Transition ClassicTransition(char c, State * target);
Transition RangeTransition(char min, char max, State * target);
State CreateState(bool f);
void DestroyState(State* s);
void AddTransition(State * s, Transition t);
bool Match(State * s, char * word, size_t cursor);

void ClearParsingBuffer(ParsingBuffer *);
void PushParsingBuffer(ParsingBuffer *, char);

Automaton LoadAutomaton(char*);
void UnloadAutomaton(Automaton*);

void SwitchState(AutomatonParserState*, AutomatonParserState, ParsingBuffer*);

#endif