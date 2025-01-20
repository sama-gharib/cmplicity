#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include <stdlib.h>

#define BUFFER_SIZE 32

typedef struct {
	void * data;
	size_t size;

} Element;

typedef struct {
	Element * content;
	size_t length;
	size_t allocated;

} Vector;

Vector CreateVector();
void DestroyVector(Vector*);

void PushVector(Vector*, Element);

#endif