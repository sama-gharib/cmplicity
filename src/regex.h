#ifndef REGEX_H_INCLUDED
#define REGEX_H_INCLUDED

#include "automaton.h"

typedef enum {
	RNTUnion,
	RNTUnionRight,
	RNTConcat,
	RNTConcatRight,
	RNTStar,
	RNTStarRight,
	RNTUnit

} RegexNonTerminal;

typedef enum {
	RTUnion,
	RTConcatenation,
	RTStar,
	RTLetter,
	RTEmpty

} RegexTerminal;


/*
    # LL(1) Regex Grammar :
        RNTUnion: RNTConcat RNTUnionRight | RTEmpty
        RNTUnionRight: RTUnion RNTConcat | RTEmpty
        RNTConcat: RNTStar RNTConcatRight
        RNTConcatRight: RTConcatenation RNTStar | RTEmpty
        RNTStar: RNTUnit RNTStarRight
        RNTStarRight: RTStar | RTEmpty
        RNTUnit: '(' RNTUnion ')' | RTLetter
*/


Automate CompileRegex(char *);


#endif