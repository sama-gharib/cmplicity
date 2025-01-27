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
	RTOpenBrackt,
	RTUnion,
	RTRangeTo,
	RTNegation,
	RTEmpty,
	RTLetter,
	RTEnd

} RegexTerminal;

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

// All of the following constitute the
// Regex LL(1) recursive parser
Automaton ParserUnion(char *, size_t *);
OperationOrder ParserUnionRight(char *, size_t *);
Automaton ParserConcat(char *, size_t *);
OperationOrder ParserConcatRight(char *, size_t *);
Automaton ParserRepeater(char *, size_t *);
OperationOrder ParserRepeaterRight(char *, size_t *);
OperationType ParserRepeaterSym(char *, size_t *);
Automaton ParserUnit(char *, size_t *);
Automaton ParserRange(char *, size_t *);
RangeType ParserUpper(char *, size_t *);
void ParserWord(char *, size_t *);
bool ParserNegation(char *, size_t *);

#endif