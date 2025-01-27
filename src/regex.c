#include "regex.h"

Automaton CompileRegex(char * src) {
	fprintf(stderr, "CompileRegex not yet implemented\n");
}

// All of the following constitute the
// Regex LL(1) recursive parser
Automaton ParserUnion(char * src, size_t * ind) {
	
}
OperationOrder ParserUnionRight(char *, size_t *) {

}
Automaton ParserConcat(char * src, size_t * ind) {

}
OperationOrder ParserConcatRight(char * src, size_t * ind) {

}
Automaton ParserRepeater(char * src, size_t * ind) {

}
OperationOrder ParserRepeaterRight(char * src, size_t * ind) {

}
OperationType ParserRepeaterSym(char * src, size_t * ind) {

}
Automaton ParserUnit(char * src, size_t * ind) {

}
Automaton ParserRange(char * src, size_t * ind) {

}
RangeType ParserUpper(char * src, size_t * ind) {

}
void ParserWord(char * src, size_t * ind) {

}
bool ParserNegation(char * src, size_t * ind) {

}