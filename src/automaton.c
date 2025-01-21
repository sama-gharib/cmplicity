#include "automaton.h"

#include <stdio.h>

Transition ClassicTransition(char c, State * target) {
	return (Transition) {
		c,
		c,
		target 
	};
}
Transition RangeTransition(char min, char max, State * target) {
	return (Transition) {
		min,
		max,
		target
	};
}

State CreateState(bool f) {
	return (State) {
		CreateVector(),
		f
	};
}
void DestroyState(State* s) {
	DestroyVector(&s->successors);
}

void AddTransition(State * s, Transition t) {
	PushVector(&s->successors, (Element) {&t, sizeof(Transition)});
}

bool Match(State * s, char * word, size_t cursor) {
	if (word[cursor] == '\0') {
		return s->final;
	} else {
		for(size_t i = 0; i < s->successors.length; i++) {
			Transition * t = (Transition*) s->successors.content[i].data;
			if (t->min <= word[cursor] && t->max >= word[cursor]) {
				if (Match(t->target, word, cursor + 1)) {
					return true;
				}
			}
		}

		return false;
	}
}

void ClearParsingBuffer(ParsingBuffer * b) {
	b->length = 0;
	b->content[0] = '\0';
}
void PushParsingBuffer(ParsingBuffer * b, char c) {
	if (b->length == PARSING_BUFFER_SIZE - 1) {
		fprintf(stderr, "%s\n", "Error : Tried to push on full ParsingBuffer");
	} else {
		b->content[b->length] = c;
		b->length ++;
		b->content[b->length] = '\0';
	}
}

Automaton LoadAutomaton(char * src) {
	Automaton result = {CreateVector(), NULL};

	AutomatonParserState state = APSDefault;
	size_t index = 0;

	ParsingBuffer buffer = {0, {0}};

	Vector states      = CreateVector();
	Vector finals      = CreateVector();
	Vector initial     = CreateVector();
	Vector transitions = CreateVector();

	bool success = true;
	int done = 0;

	while (src[index] != '\0') {
		char current = src[index];
		// Discard blank characters
		if (current == '\t' || current == ' ') continue;

		switch (state) {
			case APSDefault:
				PushParsingBuffer(&buffer, current);

				if (strcmp(buffer.content, "States:") == 0) {
					SwitchState(&state, APSStates, &buffer);
				} else if (strcmp(buffer.content, "Finals:") == 0) {
					SwitchState(&state, APSFinals, &buffer);
				} else if (strcmp(buffer.content, "Initial:") == 0) {
					SwitchState(&state, APSInitial, &buffer);
				} else if (strcmp(buffer.content, "Transitions") == 0) {
					SwitchState(&state, APSTransitions, &buffer);
				} else if (buffer.length > 15) {
					success = false;
					fprintf(stderr, "Error: invalid identifier : '%s'\n", buffer.content);
					break;
				}

				if (done & state) {
					fprintf(stderr, "Error: automaton file cannot declare multiple times the same field\n");
					success = false;
					break;
				}
			break;
			case APSStates:
				if (current == ',' || current == ';') {
					PushVector(&states, (Element) {buffer.content, buffer.length});
				} else {
					PushParsingBuffer(&buffer, current);
				}

				if (current == ';') {
					done = done | APSStates;
					SwitchState(&state, APSDefault, &buffer);
				}
			break;
			case APSInitial:
				if (current == ';') {
					PushVector(&states, (Element) {buffer.content, buffer.length});
					SwitchState(&state, APSDefault, &buffer);
					done = done | APSInitial;
				} else if (current == ',') {
					fprintf(stderr, "Error: Cannot have multiple initial states.");
					success = false;
					break;
				} else {
					PushParsingBuffer(&buffer, current);
				}
			break;
			case APSFinals:
			break;
			case APSTransitions:
			break;
		}
	}

	if (!success) {
		UnloadAutomaton(&result);
	} else {
		// TODO: Translate vectors in Automaton
		fprintf(stderr, "Vector translation is not yet implemented !\n");
	}

	return result;
}
void UnloadAutomaton(Automaton * a) {
	DestroyVector(&a->states);
}

void SwitchState(AutomatonParserState* target, AutomatonParserState new_state, ParsingBuffer* buffer) {
	*target = new_state;
	ClearParsingBuffer(buffer);
}