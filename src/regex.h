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


Automate CompileRegex(char *);


#endif