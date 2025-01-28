#include "regex.h"

#include <stdio.h>

Automaton CompileRegex(char * src) {

	printf("\tTokenization...\n");
	RegexToken * tokens = TokenizeRegex(src);
	size_t i = 0;
	printf("'");
	while (tokens[i].terminal != RTEnd) {
		char * to_print;
		switch (tokens[i].terminal) {
			case RTStar:
				printf("*");
			break;
			case RTPlus:
				printf("+");
			break;
			case RTOption:
				printf("?");
			break;
			case RTAny:
				printf(".");
			break;
			case RTOpen:
				printf("(");
			break;
			case RTClose:
				printf(")");
			break;
			case RTOpenBracket:
				printf("[");
			break;
			case RTCloseBracket:
				printf("]");
			break;
			case RTUnion:
				printf("|");
			break;
			case RTRangeTo:
				printf("-");
			break;
			case RTNegation:
				printf("^");
			break;
			case RTEmpty:
				printf("Empty");
			break;
			case RTLetter:
				printf("\\%c", tokens[i].symbol);
			break;
			case RTEnd:
				printf("$");
			break;
		}

		i++;
	}

	printf("'\n\tDone.\n");

	size_t cursor = 0;

	printf("\tBuilding automaton for '%s'...\n", src);
	Automaton result = ParserUnion(tokens, &cursor, true);
	printf("\tDone.\n");

	free(tokens);

	return result;
}

RegexToken * TokenizeRegex(char * src) {

	size_t cursor = 0;
	while (src[cursor] != '\0') {
		cursor ++;
	}

	RegexToken * token_array = malloc(sizeof(RegexToken) * (cursor+1));

	cursor = 0;
	bool escaped = false;
	size_t offset = 0;
	while (src[cursor] != '\0') {
		size_t lag = cursor - offset;
		char c = src[cursor];

		if (escaped) {
			token_array[lag] = (RegexToken) {
				true,
				RTLetter,
				c
			};
			escaped = false;
		} else {
			switch (src[cursor]) {
				case '\\':
					escaped = true;
					offset ++;
				break;
				case '(':
					token_array[lag] = (RegexToken) {
						false,
						RTOpen,
						'('
					};
				break;
				case ')':
					token_array[lag] = (RegexToken) {
						false,
						RTClose,
						')'
					};
				break;
				case '|':
					token_array[lag] = (RegexToken) {
						false,
						RTUnion,
						'|'
					};
				break;
				case '*':
					token_array[lag] = (RegexToken) {
						false,
						RTStar,
						'*'
					};
				break;
				case '+':
					token_array[lag] = (RegexToken) {
						false,
						RTPlus,
						'+'
					};
				break;
				case '[':
					token_array[lag] = (RegexToken) {
						false,
						RTOpenBracket,
						'['
					};
				break;
				case ']':
					token_array[lag] = (RegexToken) {
						false,
						RTCloseBracket,
						']'
					};
				break;
				case '-':
					token_array[lag] = (RegexToken) {
						false,
						RTRangeTo,
						'-'
					};
				break;
				case '^':
					token_array[lag] = (RegexToken) {
						false,
						RTNegation,
						'^'
					};
				break;
				case '?':
					token_array[lag] = (RegexToken) {
						false,
						RTOption,
						'?'
					};
				break;
				case '.':
					token_array[lag] = (RegexToken) {
						false,
						RTAny,
						'.'
					};
				break;
				default:
					token_array[lag] = (RegexToken) {
						true,
						RTLetter,
						c
					};
				break; 
			}
		}

		cursor ++;
	}

	token_array[cursor - offset] = (RegexToken) {
		false,
		RTEnd,
		'$'
	};

	return token_array;

}

// All of the following constitute the
// Regex LL(1) recursive parser
Automaton ParserUnion(RegexToken * src, size_t * ind, bool allow_epsilon) {
	printf("\t\tUnion, cursor = %llu\n", *ind);

	RegexToken t = src[*ind];

	if (t.is_letter
	 || t.terminal == RTOpen
	 || t.terminal == RTOpenBracket
	 || t.terminal == RTAny ) {
		Automaton left = ParserConcat(src, ind);
		OperationOrder right = ParserUnionRight(src, ind);
		if (right.type == OTUnion) {
			return AutomatonUnion(&left, &right.on);
		} else {
			UnloadAutomaton(&right.on);
			return left;
		}
	} else if (allow_epsilon &&
		(	
		t.terminal == RTClose
	 || t.terminal == RTEnd)) {
		return DefaultAutomaton();
	} else {
		fprintf(stderr, "Error: Unexpected token: %c\n", t.symbol);
		// TODO: Better error handling please
		return DefaultAutomaton();
	}

}
OperationOrder ParserUnionRight(RegexToken * src, size_t * ind) {
	printf("\t\tUnionRight, cursor = %llu\n", *ind);


	RegexToken t = src[*ind];

	if (t.terminal == RTUnion) {
		(*ind) ++;
		Automaton c  = ParserConcat(src, ind);
		Automaton up = ParserUnion(src, ind, false);

		return (OperationOrder) {
			OTUnion,
			AutomatonUnion(
				&c,
				&up
			)
		};
	} else if (t.terminal == RTClose || t.terminal == RTEnd) {
		return (OperationOrder) {
			OTNone,
			DefaultAutomaton()
		};
	} else {
		fprintf(stderr, "Error: Unexpected token: %c\n", t.symbol);
		return (OperationOrder) {
			OTNone,
			DefaultAutomaton()
		};
	}
}
Automaton ParserConcat(RegexToken * src, size_t * ind) {
	printf("\t\tConcat, cursor = %llu\n", *ind);


	RegexToken t = src[*ind];

	if (t.is_letter
	 || t.terminal == RTOpen
	 || t.terminal == RTOpenBracket
	 || t.terminal == RTAny ) {
		Automaton left = ParserRepeater(src, ind);
		OperationOrder right = ParserConcatRight(src, ind);

		if (right.type == OTConcat) {
			Automaton c = AutomatonConcatenation(&left, &right.on);

			return c;
		} else {
			printf("Received OTNone.\n");
			printf("laoe: %p : %d\n", left.initial, Match(left.initial, "aaab", 0));
			printf("Passed.\n");
			UnloadAutomaton(&right.on);
			return left;
		}
	} else {
		fprintf(stderr, "Error: Unexpected token: %c\n", t.symbol);
		// TODO: Better error handling please
		return DefaultAutomaton();
	}

}
OperationOrder ParserConcatRight(RegexToken * src, size_t * ind) {
	printf("\t\tConcatRight, cursor = %llu\n", *ind);


	RegexToken t = src[*ind];

	if (
		t.terminal == RTOpen
	 || t.is_letter
	 || t.terminal == RTOpenBracket
	 || t.terminal == RTAny
	) {
		printf("ConcatRight calls Concat\n");
		Automaton n = ParserConcat(src, ind);
		printf("ConcatRight.Concat finished\n");

		printf("Crash here :\n");
		Match(n.initial, "b", 0);
		printf("Plu d'idée..\n");

		return (OperationOrder) {
			OTConcat,
			n
		};
	} else if (
		t.terminal == RTClose
	 || t.terminal == RTEnd
	 || t.terminal == OTUnion
	 ) {
	 	printf("Returning default automaton...\n");
		OperationOrder order = (OperationOrder) {
			OTNone,
			DefaultAutomaton()
		};

		printf("ConcatRight returning : %p\n", order.on.initial);

		return order;
	} else {
		fprintf(stderr, "Error: Unexpected token: %c\n", t.symbol);
		return (OperationOrder) {
			OTNone,
			DefaultAutomaton()
		};
	}
}
Automaton ParserRepeater(RegexToken * src, size_t * ind) {
	printf("\t\tRepeater, cursor = %llu\n", *ind);

	RegexToken t = src[*ind];

	if (t.is_letter
	 || t.terminal == RTOpen
	 || t.terminal == RTOpenBracket
	 || t.terminal == RTAny ) {
		Automaton left = ParserUnit(src, ind);
		
		printf("Call to repeater right\n");
		OperationOrder right = ParserRepeaterRight(src, ind);
		printf("Finished call to repeater right\n");

		printf("LEFT: %d\n", Match(left.initial, "b", 0));
		printf("Right is %p\n", right.on.initial);
		printf("RIGHT: %d\n", Match(right.on.initial, "aa", 0));

		UnloadAutomaton(&right.on);

		if (right.type == OTStar) {
			return AutomatonStar(&left);
		} else if (right.type == OTPlus) {
			return AutomatonPlus(&left);
		} else if (right.type == OTOption) {
			return AutomatonOption(&left);
		} else {
			return left;
		}

	} else {
		fprintf(stderr, "Error: Unexpected token: %c\n", t.symbol);
		// TODO: Better error handling please
		return DefaultAutomaton();
	}

}
OperationOrder ParserRepeaterRight(RegexToken * src, size_t * ind) {
	printf("\t\tRepeaterRight, cursor = %llu\n", *ind);

	RegexToken t = src[*ind];

	if (
		t.terminal == RTStar
	 || t.terminal == RTPlus
	 || t.terminal == RTOption
	) {
		(*ind) ++;

		OperationType ot = OTNone;
		switch (t.terminal) {
			case RTStar: ot = OTStar;
			break;
			case RTPlus: ot = OTPlus;
			break;
			case RTOption: ot = OTOption;
			break;
		}

		Automaton n = {0};
		printf("RECURSIVE Call to repeater right\n");
		OperationOrder oo = ParserRepeaterRight(src, ind);
		printf("FINISHED RECURSIVE Call to repeater right\n");
		if (oo.type != OTNone) {
			if (oo.type == OTStar) {
				n = AutomatonStar(&oo.on);
			} else if (oo.type == OTPlus) {
				n = AutomatonPlus(&oo.on);
			}else if (oo.type == OTStar) {
				n = AutomatonOption(&oo.on);
			}
		} else {
			n = DefaultAutomaton();
		}

		return (OperationOrder) {
			ot,
			n
		};
	} else if (
		t.terminal == RTOpen
	 || t.terminal == RTClose
	 || t.is_letter
	 || t.terminal == RTOpenBracket
	 || t.terminal == RTAny
	 || t.terminal == RTEnd
	 || t.terminal == OTUnion
	 ) {
	 	Automaton on = DefaultAutomaton();
		printf("%p : %d\n", on.initial, Match(on.initial, "bb", 0));
		return (OperationOrder) {
			OTNone,
			on
		};
	} else {
		fprintf(stderr, "Error: Unexpected token: %c\n", t.symbol);
		return (OperationOrder) {
			OTNone,
			DefaultAutomaton()
		};
	}
}

Automaton ParserUnit(RegexToken * src, size_t * ind) {
	printf("\t\tUnit, cursor = %llu\n", *ind);

	RegexToken t = src[*ind];

	(*ind) ++;

	if (t.terminal == RTOpen) {
		return ParserUnion(src, ind, true);
	} else if(t.terminal == RTLetter) {
		return SingleLetterAutomaton(t.symbol, t.symbol);
	} else if (t.terminal == RTOpenBracket) {
		(*ind) --;
		return ParserRange(src, ind);
	} else if (t.terminal == RTAny) {
		return SingleLetterAutomaton(' ', '~');
	} else {
		fprintf(stderr, "Error: Unexpected token: %c\n", t.symbol);
		return DefaultAutomaton();
	}
}
Automaton ParserRange(RegexToken * src, size_t * ind) {
	printf("\t\tRange, cursor = %llu\n", *ind);
	
	RegexToken t = src[*ind];	

	if (t.terminal == RTOpenBracket) {
		size_t first_ind = *ind;
		(*ind) ++;
		bool negation = ParserNegation(src, ind);
		if (src[*ind].is_letter) {
			(*ind) ++;
			RangeType rt = ParserUpper(src, ind);
			Automaton result;
			if (rt == TrueRange) {
				result = SingleLetterAutomaton(src[*ind-3].symbol, src[*ind-1].symbol);
			} else {
				result = SingleLetterAutomaton(src[first_ind].symbol, src[first_ind].symbol);
				for (size_t i = first_ind + 1; i < *ind; i++) {
					Automaton to_add = SingleLetterAutomaton(src[i].symbol, src[i].symbol);
					result = AutomatonUnion(&result, &to_add);
				}
			}

			if (src[*ind].terminal != RTCloseBracket) {
				fprintf(stderr, "Expected ']', found '%c'\n", src[*ind].symbol);
			}
			(*ind) ++;

			return result;
		}
	} else {
		fprintf(stderr, "Unexpected character : '%c'\n", t.symbol);
		return DefaultAutomaton();
	}
}
RangeType ParserUpper(RegexToken * src, size_t * ind) {
	printf("\t\tUpper, cursor = %llu\n", *ind);


	RegexToken t = src[*ind];

	if (t.is_letter) {
		(*ind) ++;
		ParserWord(src, ind);
		return Enumeration;
	} else if (t.terminal == RTRangeTo) {
		*ind += 2;
		return TrueRange;
	} else {
		fprintf(stderr, "Unexpected character : '%c'\n", t.symbol);		
		return Enumeration;
	}
}
void ParserWord(RegexToken * src, size_t * ind) {
	printf("\t\tWord, cursor = %llu\n", *ind);
	

	RegexToken t = src[*ind];

	if (t.is_letter) {
		(*ind) ++;
		ParserWord(src, ind);
	} else if (t.terminal == RTCloseBracket) {
		fprintf(stderr, "Unexpected character : '%c'\n", t.symbol);		
	}
}
bool ParserNegation(RegexToken * src, size_t * ind) {
	printf("\t\tNegation, cursor = %llu\n", *ind);
	

	RegexToken t = src[*ind];

	if (t.terminal == RTNegation) {
		(*ind) ++;
		fprintf(stderr, "Range negation not yet implemented !\n", t.symbol);		
		return true;
	} else if (t.is_letter) {
		return false;
	} else {
		fprintf(stderr, "Unexpected character : '%c'\n", t.symbol);		
	}
}