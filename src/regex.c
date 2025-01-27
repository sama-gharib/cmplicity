#include "regex.h"

Automaton CompileRegex(char * src) {

	RegexToken * tokens = TokenizeRegex(src);

	size_t cursor = 0;
	Automaton result = ParserUnion(tokens, &cursor);

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
Automaton ParserUnion(RegexToken * src, size_t * ind) {
	char c = src[*ind];


}
OperationOrder ParserUnionRight(RegexToken *, size_t *) {

}
Automaton ParserConcat(RegexToken * src, size_t * ind) {

}
OperationOrder ParserConcatRight(RegexToken * src, size_t * ind) {

}
Automaton ParserRepeater(RegexToken * src, size_t * ind) {

}
OperationOrder ParserRepeaterRight(RegexToken * src, size_t * ind) {

}
OperationType ParserRepeaterSym(RegexToken * src, size_t * ind) {

}
Automaton ParserUnit(RegexToken * src, size_t * ind) {

}
Automaton ParserRange(RegexToken * src, size_t * ind) {

}
RangeType ParserUpper(RegexToken * src, size_t * ind) {

}
void ParserWord(RegexToken * src, size_t * ind) {

}
bool ParserNegation(RegexToken * src, size_t * ind) {

}