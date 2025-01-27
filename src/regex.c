#include "regex.h"

Automaton CompileRegex(char * src) {

	RegexToken * tokens = TokenizeRegex(src);

	size_t cursor = 0;
	Automaton result = ParserUnion(tokens, &cursor, true);

	free(tokens);

	return result;
}

RegexToken * TokenizeRegex(char * src) {

	size_t cursor = 0;
	while (src[cursor] != '\0') {
		cursor ++;
	}

	RegexToken * token_array = malloc(sizeof(RegexToken) * (cursor+1));
	token_array[cursor] = (RegexToken) {
		false,
		RTEnd,
		'$'
	};

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
			}
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
					}
				break;
				case ')':
					token_array[lag] = (RegexToken) {
						false,
						RTClose,
						')'
					}
				break;
				case '|':
					token_array[lag] = (RegexToken) {
						false,
						RTUnion,
						'|'
					}
				break;
				case '*':
					token_array[lag] = (RegexToken) {
						false,
						RTStar,
						'*'
					}
				break;
				case '+':
					token_array[lag] = (RegexToken) {
						false,
						RTPlus,
						'+'
					}
				break;
				case '[':
					token_array[lag] = (RegexToken) {
						false,
						RTOpenBracket,
						'['
					}
				break;
				case ']':
					token_array[lag] = (RegexToken) {
						false,
						RTCloseBracket,
						']'
					}
				break;
				case '-':
					token_array[lag] = (RegexToken) {
						false,
						RTRangeTo,
						'-'
					}
				break;
				case '^':
					token_array[lag] = (RegexToken) {
						false,
						RTNegation,
						'^'
					}
				break;
				case '?':
					token_array[lag] = (RegexToken) {
						false,
						RTOption,
						'?'
					}
				break;
				case '.':
					token_array[lag] = (RegexToken) {
						false,
						RTAny,
						'.'
					}
				break;
				default:
					token_array[lag] = (RegexToken) {
						true,
						RTLetter,
						c
					}
				break; 
			}
		}

		cursor ++;
	}

	return token_array;

}

// All of the following constitute the
// Regex LL(1) recursive parser
Automaton ParserUnion(RegexToken * src, size_t * ind, bool allow_epsilon) {
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
	RegexToken t = src[*ind];

	if (t.terminal == RTUnion) {
		*ind ++;
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
	RegexToken t = src[*ind];

	if (t.is_letter
	 || t.terminal == RTOpen
	 || t.terminal == RTOpenBracket
	 || t.terminal == RTAny ) {
		Automaton left = ParserRepeater(src, ind);
		OperationOrder right = ParserConcatRight(src, ind);
		if (right.type == OTConcat) {
			return AutomatonConcatenation(&left, &right.on);
		} else {
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
	RegexToken t = src[*ind];

	if (
		t.terminal == RTOpen
	 || t.terminal.is_letter
	 || t.terminal == RTOpenBracket
	 || t.terminal == RTAny
	) {
		Automaton n = ParserConcat(src, ind);

		return (OperationOrder) {
			OTConcat,
			n
		};
	} else if (
		t.terminal == RTClose
	 || t.terminal == RTEnd
	 || t.terminal == OTUnion
	 ) {
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
Automaton ParserRepeater(RegexToken * src, size_t * ind) {
	RegexToken t = src[*ind];

	if (t.is_letter
	 || t.terminal == RTOpen
	 || t.terminal == RTOpenBracket
	 || t.terminal == RTAny ) {
		Automaton left = ParserUnit(src, ind);
		OperationOrder right = ParserRepeaterRight(src, ind);
		if (right.type == OTStar) {
			return AutomatonStar(&left);
		} else if (right.type == OTPlus) {
			return AutomatonPlus(&left);
		}else if (right.type == OTStar) {
			return AutomatonOption(&left);
		}

		UnloadAutomaton(&right.on);
	} else {
		fprintf(stderr, "Error: Unexpected token: %c\n", t.symbol);
		// TODO: Better error handling please
		return DefaultAutomaton();
	}

}
OperationOrder ParserRepeaterRight(RegexToken * src, size_t * ind) {
	RegexToken t = src[*ind];

	if (
		t.terminal == RTStar
	 || t.terminal == RTPlus
	 || t.terminal == RTOption
	) {
		*ind ++;

		OperationType ot = OTNone;
		switch r.terminal{
			case RTStar: ot = OTStar;
			break;
			case RTPlus: ot = OTPlus;
			break;
			case RTOption: ot = OTOption;
			break;
		}

		Automaton n;
		OperationOrder oo = ParserRepeaterRight(src, ind);
		if (oo.type != OTNone) {
			if (oo.type == OTStar) {
				n = AutomatonStar(&oo.on);
			} else if (oo.type == OTPlus) {
				n = AutomatonPlus(&oo.on);
			}else if (oo.type == OTStar) {
				n = AutomatonOption(&oo.on);
			}
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

Automaton ParserUnit(RegexToken * src, size_t * ind) {
	RegexToken t = src[*ind];

	if (t.terminal == RTOpen) {
		*ind ++;
		return ParserUnion(src, ind);
	} else if(t.terminal == RTLetter) {
		*ind ++;

		return SingleLetterAutomaton(t.symbol, t.symbol);
	} else if (t.terminal == RTOpenBracket) {
		return ParserRange(src, ind);
	} else if (t.terminal == RTAny) {
		return SingleLetterAutomaton(' ', 'ÿ');
	} else {
		fprintf(stderr, "Error: Unexpected token: %c\n", t.symbol);
		return DefaultAutomaton();
	}
}
Automaton ParserRange(RegexToken * src, size_t * ind) {
	RegexToken t = src[*ind];	

	if (t.terminal == RTOpenBracket) {
		size_t first_ind = *ind;
		*ind ++;
		bool negation = ParserNegation(src, ind);
		if (src[*ind] >= ' ' || src[*ind] <= 'ÿ') {
			*ind ++;
			RangeType rt = ParserUp(src, ind);
			Automaton result;
			if (rt == TrueRange) {
				result = SingleLetterAutomaton(src[*ind-3], src[*ind-1]);
			} else {
				result = SingleLetterAutomaton(src[first_ind]);
				for (size_t i = first_ind + 1; i < *ind; i++) {
					to_add = SingleLetterAutomaton(src[i].symbol);
					result = AutomatonUnion(result, to_add);
				}
			}

			if (src[*ind].terminal != ']') {
				fprintf(stderr, "Expected ']', found '%c'\n", src[*ind].symbol);
			}

			return result;
		}
	} else {
		fprintf(stderr, "Unexpected character : '%c'\n", t.symbol);
		return DefaultAutomaton();
	}
}
RangeType ParserUpper(RegexToken * src, size_t * ind) {
	RegexToken t = src[*ind];

	if (t.is_letter) {
		*ind ++;
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
	RegexToken t = src[*ind];

	if (t.is_letter) {
		*ind ++;
		ParserWord(src, ind);
	} else if (t.terminal == RTCloseBracket) {
		fprintf(stderr, "Unexpected character : '%c'\n", t.symbol);		
	}
}
bool ParserNegation(RegexToken * src, size_t * ind) {
	RegexToken t = src[*ind];

	if (t.terminal == RTNegation) {
		*ind ++;
		fprintf(stderr, "Range negation not yet implemented !\n", t.symbol);		
		return true;
	} else if (t.terminal.is_letter) {
		return false;
	} else {
		fprintf(stderr, "Unexpected character : '%c'\n", t.symbol);		
	}
}