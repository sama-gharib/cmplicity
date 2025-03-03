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

ParsingBuffer InitParsingBuffer() {
	return (ParsingBuffer) {1, {0}};
}

void ClearParsingBuffer(ParsingBuffer * b) {
	b->length = 1;
	b->content[0] = '\0';
}
void PushParsingBuffer(ParsingBuffer * b, char c) {
	if (b->length == PARSING_BUFFER_SIZE) {
		fprintf(stderr, "%s : '%s'\n", "Error : Tried to push on full ParsingBuffer", b->content);
	} else {
		b->content[b->length-1] = c;
		b->content[b->length] = '\0';
		b->length ++;
	}
}

Automaton LoadAutomaton(char * src) {
	Automaton result = {CreateVector(), NULL};

	AutomatonParserState state = APSDefault;
	size_t index = 0;

	ParsingBuffer buffer = {1, {0}};
	ParsingBuffer initial = {1, {0}};

	Vector states      = CreateVector();
	Vector finals      = CreateVector();
	Vector transitions = CreateVector();

	int done = APSDefault;
	int parenthesis_level = 0;

	while (src[index] != '\0') {
		char current = src[index];

		// Discard blank characters
		if (current != '\t' && current != ' ' && current != '\n') {
			if (current == '(') {
				parenthesis_level ++;
			} else if (current == ')') {
				parenthesis_level --;
			}

			switch (state) {
				case APSDefault:
					PushParsingBuffer(&buffer, current);

					AutomatonParserState old = state;

					if (strcmp(buffer.content, "States:") == 0) {
						SwitchState(&state, APSStates, &buffer);
					} else if (strcmp(buffer.content, "Finals:") == 0) {
						SwitchState(&state, APSFinals, &buffer);
					} else if (strcmp(buffer.content, "Initial:") == 0) {
						SwitchState(&state, APSInitial, &buffer);
					} else if (strcmp(buffer.content, "Transitions:") == 0) {
						SwitchState(&state, APSTransitions, &buffer);
					} else if (buffer.length > 15) {
						fprintf(stderr, "Error: invalid identifier : '%s'\n", buffer.content);
						goto lexing_end;
					}

					if (old != state && done & state) {
						fprintf(stderr, "Error: automaton file cannot declare multiple times the same field\n");
						goto lexing_end;
					}
				break;
				case APSStates:
					if (current == ',' || current == ';') {
						PushVector(&states, (Element) {buffer.content, buffer.length});
						ClearParsingBuffer(&buffer);
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
						memcpy(initial.content, buffer.content, buffer.length);
						SwitchState(&state, APSDefault, &buffer);
						done = done | APSInitial;
					} else if (current == ',') {
						fprintf(stderr, "Error: Cannot have multiple initial states.");
						goto lexing_end;
					} else {
						PushParsingBuffer(&buffer, current);
					}
				break;
				case APSFinals:
					if (current == ',' || current == ';') {
						PushVector(&finals, (Element) {buffer.content, buffer.length});
						ClearParsingBuffer(&buffer);
					} else {
						PushParsingBuffer(&buffer, current);
					}

					if (current == ';') {
						done = done | APSFinals;
						SwitchState(&state, APSDefault, &buffer);
					}
				break;
				case APSTransitions:
					if (parenthesis_level == 0 && (current == ',' || current == ';')) {
						PushVector(&transitions, (Element) {buffer.content, buffer.length});
						ClearParsingBuffer(&buffer);
					} else {
						PushParsingBuffer(&buffer, current);
					}

					if (current == ';') {
						done = done | APSTransitions;
						SwitchState(&state, APSDefault, &buffer);
					}
				break;
			}
		}

		index ++;
	}

	lexing_end:

	if (done != APSAllDone) {
		UnloadAutomaton(&result);
		fprintf(stderr, "Failed to parse automaton from string, code %d.\n", done);
	} else {

		for (size_t i = 0; i < states.length; i++) {
			State new_state = CreateState(false);
			// Set final
			for (size_t j = 0; j < finals.length; j++) {
				if (strcmp(finals.content[j].data, states.content[i].data) == 0) {
					new_state.final = true;
					break;
				}
			}

			PushVector(&result.states, (Element) {&new_state, sizeof(State)});
			// /!\ NOT DESTROYING new_state IS VOLONTARY /!\
			// If we were to destroy new_state, its successors.content
			// would be freed BUT PushVector(v, e) copy of e->data
			// is not deep !! Thus, new_state.successors.content is
			// destroyed when destroying result.
			
			// Set initial
			if (strcmp(states.content[i].data, initial.content) == 0) {
				result.initial = result.states.content[result.states.length-1].data;
			}
		}

		// Setting transisitions
		for (size_t i = 0; i < transitions.length; i++) {
			TransitionToken tt = CreateTransitionToken(transitions.content[i].data);
			if (tt.predecessor != NULL && tt.successor != NULL) {
				for(size_t p = 0; p < states.length; p++) {
					for(size_t s = 0; s < states.length; s++)
					{
						if (strcmp(states.content[p].data, tt.predecessor) == 0
						 && strcmp(states.content[s].data, tt.successor) == 0) {
							State * pred = (State*) result.states.content[p].data;
							State * succ = (State*) result.states.content[s].data;
							Transition new_transition = {
								tt.min,
								tt.max,
								succ
							};
							PushVector(&pred->successors, (Element) {&new_transition, sizeof(Transition)});
						}
					}
				}
			}
			DestroyTransitionToken(&tt);
		}

		if (result.initial == NULL) {
			fprintf(stderr, "Initial state is not declared in states\n");
			goto parsing_end;
		}



	}

	parsing_end:

	DestroyVector(&states);
	DestroyVector(&finals);
	DestroyVector(&transitions);

	return result;
}
Automaton DefaultAutomaton() {
	return LoadAutomaton("\
		States: s;\
		Initial: s;\
		Finals: ;\
		Transitions: ;\
	");
}
Automaton SingleLetterAutomaton(char min, char max) {
	Automaton l = DefaultAutomaton();

	l.initial->final = false;
	State new_state = CreateState(true);
	PushVector(&l.states, (Element) {&new_state, sizeof(State)});
	AddTransition(
		l.initial,
		(Transition) {
			min,
			max,
			l.states.content[1].data
		}
	);

	return l;
}

void UnloadAutomaton(Automaton * a) {
	for (size_t i = 0; i < a->states.length; i++) {
		DestroyState(a->states.content[i].data);
	}
	DestroyVector(&a->states);
}

// Moves out A and B
Automaton AutomatonUnion(Automaton * a, Automaton * b) {
	size_t new_length = a->states.length + b->states.length + 1;
	Automaton result = {
		CreateVector(),
		NULL
	};

	// Creating new initial state
	size_t successors_count = a->initial->successors.length + b->initial->successors.length;
	State new_initial = {
		CreateVector(),
		a->initial->final || b->initial->final
	};

	
	// Adding transitions
	for (size_t i = 0; i < a->initial->successors.length; i++) {
		PushVector(&new_initial.successors, a->initial->successors.content[i]);
	}
	for (size_t i = 0; i < b->initial->successors.length; i++) {
		PushVector(&new_initial.successors, b->initial->successors.content[i]);
	}

	// Adding states in new automaton
	PushVector(&result.states, (Element) {&new_initial, sizeof(State)});

	result.states.content = realloc(result.states.content, sizeof(Element) * new_length);
	result.states.allocated = new_length;
	
	for (size_t i = 0; i < a->states.length; i++) {
		result.states.content[i+1] = a->states.content[i];
	}
	for (size_t i = 0; i < b->states.length; i++) {
		result.states.content[i+a->states.length+1] = b->states.content[i];
	}
	result.states.length = new_length;

	result.initial = result.states.content[0].data;

	// Cleaning out garbage
	free(a->states.content);
	free(b->states.content);
	a->states.content = NULL;
	b->states.content = NULL;
	a->initial = NULL;
	b->initial = NULL;

	return result;
}

// Moves out a and b
Automaton AutomatonConcatenation(Automaton * a, Automaton * b) {
	
	// Adding new transitions
	for (size_t i = 0; i < a->states.length; i++) {
		State * current = (State*) a->states.content[i].data;
		if (current->final) {
			for (size_t j = 0; j < b->initial->successors.length; j++) {
				current->final = b->initial->final;
				PushVector(&current->successors, b->initial->successors.content[j]);
			}
		}
	}

	// Merging a and b into result
	size_t new_length = a->states.length + b->states.length;
	Automaton result = {
		(Vector) {
			malloc(sizeof(Element) * new_length),
			new_length,
			new_length
		},
		a->initial
	};

	for (size_t i = 0; i < a->states.length; i++) {
		result.states.content[i] = a->states.content[i];
	}
	for (size_t i = 0; i < b->states.length; i++) {
		result.states.content[i+a->states.length] = b->states.content[i];
	}

	// Cleaning out garbage
	free(a->states.content);
	free(b->states.content);
	a->states.content = NULL;
	b->states.content = NULL;
	a->initial = NULL;
	b->initial = NULL;

	return result;
}

// Moves out a
Automaton AutomatonStar(Automaton * a) {
	for (size_t i = 0; i < a->states.length; i++) {
		State * state = (State*) a->states.content[i].data;
		if (state->final && state != a->initial) {
			for (size_t j = 0; j < a->initial->successors.length; j++) {
				PushVector(&state->successors, a->initial->successors.content[j]);
			}
		}
	}

	// Moving out a into result for safety concerns
	Automaton result = {
		(Vector) {
			malloc(sizeof(Element) * a->states.length),
			a->states.length,
			a->states.length
		},
		a->initial
	};
	result.initial->final = true;

	for (size_t i = 0; i < a->states.length; i++) {
		result.states.content[i] = a->states.content[i];
	}

	// Cleaning leftovers
	free(a->states.content);
	a->states.content = NULL;
	a->initial = NULL;

	return result;
}

// Moves out a
Automaton AutomatonPlus(Automaton * a) {
	// Copying a into b
	Automaton b = {
		CreateVector(),
		NULL
	};

	// Adding states
	for (size_t i = 0; i < a->states.length; i++) {
		State * current_state = (State*) a->states.content[i].data;
		State s = CreateState(current_state->final);
		// No need to destroy s since its vector is
		// moved.
		PushVector(&b.states, (Element) {
			&s,
			sizeof(State)
		});
		if (a->initial == current_state) {
			b.initial = b.states.content[i].data;
		}
	}

	// Copying transitions
	for (size_t i = 0; i < a->states.length; i++) {
		State * current_state = (State*) a->states.content[i].data;
		for (size_t j = 0; j < current_state->successors.length; j++) {
			Transition * current_transition = (Transition*) current_state->successors.content[j].data;
			for (size_t k = 0; k < a->states.length; k++) {
				State * current_target = (State*) a->states.content[k].data;
				if (current_target == current_transition->target) {
					AddTransition(
						(State*) b.states.content[i].data,
						(Transition) {
							current_transition->min,
							current_transition->max,
							b.states.content[k].data
						}
					);
				}
			}
		}
	}

	b.initial->final = true;

	Automaton starred_b = AutomatonStar(&b);

	Automaton result = AutomatonConcatenation(a, &starred_b);

	// No need to free b, a nor starred_b operations
	// on automatons use the move semantic.

	return result;
}

Automaton AutomatonOption(Automaton * a) {

	State s = CreateState(true);
	PushVector(&a->states, (Element) {
		&s,
		sizeof(State)
	});
	// No need to destroy s since its vector value is
	// moved out
	State * last_state = (State*) a->states.content[a->states.length-1].data;
	for (size_t j = 0; j < a->initial->successors.length; j++) {
		PushVector(&last_state->successors, a->initial->successors.content[j]);
	}

	// Moving out a into result for safety concerns
	Automaton result = {
		(Vector) {
			malloc(sizeof(Element) * a->states.length),
			a->states.length,
			a->states.length
		},
		last_state
	};
	for (size_t i = 0; i < a->states.length; i++) {
		result.states.content[i] = a->states.content[i];
	}

	// Cleaning leftovers
	free(a->states.content);
	a->states.content = NULL;
	a->initial = NULL;

	return result;
}


void SwitchState(AutomatonParserState* target, AutomatonParserState new_state, ParsingBuffer* buffer) {
	*target = new_state;
	ClearParsingBuffer(buffer);
}

TransitionToken CreateTransitionToken(char * s) {
	size_t index = 0;
	TransitionParserState state = TPSDefault;

	TransitionToken result = {NULL, '\0', '\0', NULL};
	size_t p_size = 0;
	size_t s_size = 0;

	ParsingBuffer buffer = InitParsingBuffer();

	bool interrupt = false;

	while (s[index] != '\0' && !interrupt) {
		char c = s[index];
		if (c!=' ' && c != '\n'  && c != '\t') {
			switch (state) {
				case TPSDefault:
					if (c == '(') {
						state = TPSPredecessor;
					} else {
						fprintf(stderr, "Unexpected character before transition : '%c'\n (Expected '(')", c);
						interrupt = true;
					}
				break;
				case TPSPredecessor:
					if (c == ',') {
						result.predecessor = malloc(sizeof(char) * buffer.length);
						memcpy(result.predecessor, buffer.content, buffer.length);
						ClearParsingBuffer(&buffer);
						state = TPSTerminal;
					} else {
						PushParsingBuffer(&buffer, c);
					}
				break;
				case TPSTerminal:
					if (c == '[') {
						state = TPSMin;
					} else {
						result.min = c;
						result.max = c;
						state = TPSTerminalDone;
					}
				break;
				case TPSTerminalDone:
					if (c == ',') {
						state = TPSSuccessor;
					} else {
						fprintf(stderr, "Unexpected character after terminal : '%c'. (expected ',')\n", c);
						interrupt = true;
					}
				break;
				case TPSMin:
					result.min = c;
					state = TPSDash;
				break;
				case TPSDash:
					if (c == '-') {
						state = TPSMax;
					} else {
						fprintf(stderr, "Unexpected character after min terminal : '%c'. (expected '-')\n", c);
						interrupt = true;
					}
				break; 
				case TPSMax:
					result.max = c;
					state = TPSRangeClose;
				break;
				case TPSRangeClose:
					 if (c == ']') {
						state = TPSTerminalDone;
					} else {
						fprintf(stderr, "Unexpected character after max terminal : '%c'. (expected ']')\n", c);
						interrupt = true;
					}
				break;
				case TPSSuccessor:
					if (c == ')') {
						result.successor = malloc(sizeof(char) * buffer.length);
						
						memcpy(result.successor, buffer.content, buffer.length);

						ClearParsingBuffer(&buffer);
						state = TPSFinished;
					} else {
						PushParsingBuffer(&buffer, c);
					}
				break;
				case TPSFinished:
					fprintf(stderr, "Found character after transition : '%c'. Expected ',' or ';'.\n", c);
					interrupt = true;
				break;
			}
		}
		index ++;
	}

	if (state != TPSFinished || interrupt) {
		DestroyTransitionToken(&result);
		
		return (TransitionToken) {NULL, '\0', '\0', NULL};
	} else {
		return result;
	}
}

void DestroyTransitionToken(TransitionToken * tt) {
	if (tt->predecessor != NULL)
		free(tt->predecessor);
	if (tt->successor != NULL)
		free(tt->successor);
}