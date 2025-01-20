#include "automaton.h"

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
			// Transition * t = (Transition*) s->successors.content[i];
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