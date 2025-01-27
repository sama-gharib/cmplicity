#ifndef REGEX_H_INCLUDED
#define REGEX_H_INCLUDED

#include "automaton.h"

typedef enum {
	RNTUnion,
	RNTUnionRight,
	RNTConcat,
	RNTConcatRight,
	RNTRepeater,
	RNTRepeaterRight,
	RNTRepeaterSym,
	RNTUnit,
	RNTRange,
	RNTWord,
	RNTNegation

} RegexNonTerminal;

typedef enum {
	RTStar,
	RTPlus,
	RTOption,
	RTAny,
	RTOpen,
	RTClose,
	RTOpenBracket,
	RTCloseBracket,
	RTUnion,
	RTRangeTo,
	RTNegation,
	RTEmpty,
	RTLetter,
	RTEnd

} RegexTerminal;

typedef struct {
	bool is_letter;
	RegexTerminal terminal;
	char symbol;

} RegexToken;

typedef enum {
	OTStar,
	OTPlus,
	OTOption,
	OTConcatenation,
	OTUnion,
	OTNone

} OperationType;

typedef struct {
	OperationType type;
	Automaton on;

} OperationOrder;


typedef enum {
	TrueRange,
	Enumeration

} RangeType;

/*
    # LL(1) Extended Regex Grammar :
        RNTUnion: RNTConcat RNTUnionRight | RTEmpty
        RNTUnionRight: RTUnion RNTConcat RNTUnionRight | RTEmpty
        RNTConcat: RNTRepeater RNTConcatRight
        RNTConcatRight: RNTRepeater RNTConcatRight | RTEmpty
        RNTRepeater: RTUnit RNTRepeaterRight
        RNTRepeaterRight: RNTRepeaterSym RNTRepeaterRight | RTEmpty
        RNTRepeaterSym: RTStar | RTPlus | RTOption
        RNTUnit: RTLetter | RTOpen RNTUnion RTClose | RNTRange | RTAny
        RNTRange: RTOpenBracket RNTNegation RTLetter RNTUpper RTCloseBracket
        RNTUpper: RTRAngeTo RTLetter | RTLetter RNTWord
        RNTWord: RTLetter RNTWord | RTEmpty
        RNTNegation: RTNegation | RTEmpty
        
*/


Automaton CompileRegex(char *);

RegexToken * TokenizeRegex(char *);

// All of the following constitute the
// Regex LL(1) recursive parser
Automaton ParserUnion(RegexToken *, size_t *);
OperationOrder ParserUnionRight(RegexToken *, size_t *);
Automaton ParserConcat(RegexToken *, size_t *);
OperationOrder ParserConcatRight(RegexToken *, size_t *);
Automaton ParserRepeater(RegexToken *, size_t *);
OperationOrder ParserRepeaterRight(RegexToken *, size_t *);
OperationType ParserRepeaterSym(RegexToken *, size_t *);
Automaton ParserUnit(RegexToken *, size_t *);
Automaton ParserRange(RegexToken *, size_t *);
RangeType ParserUpper(RegexToken *, size_t *);
void ParserWord(RegexToken *, size_t *);
bool ParserNegation(RegexToken *, size_t *);

#endif