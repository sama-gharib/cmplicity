#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vector.h"

Vector CreateVector() {
	return (Vector) {
		malloc(sizeof(Element) * BUFFER_SIZE),
		0,
		BUFFER_SIZE
	};
}
void DestroyVector(Vector * v) {
	for (size_t i = 0; i < v->length; i++) {
		free(v->content[i].data);
	}

	free(v->content);
	v->content = NULL;
	v->length = 0;
	v->allocated = 0;
}

void PushVector(Vector * v, Element e) {
	if (v->length == v->allocated) {
		v->allocated += BUFFER_SIZE;
		v->content = realloc(v->content, sizeof(Element) * v->allocated);
	}

	printf("========= TOP BUN ==========\n");
	v->content[v->length] = (Element) {NULL, 0};
	printf("MYSTERY: %p\n", v->content + v->length);
	printf("ALLOCATED %llu from %p\n", v->allocated * sizeof(Element), v->content);
	printf("LENGTH: %llu, ALLOCATED: %llu\n", v->length * sizeof(Element), v->allocated * sizeof(Element));
	printf("======== BOTTOM BUN =========\n");

	v->content[v->length] = (Element) {
		malloc(e.size),
		e.size
	};

	memcpy(v->content[v->length].data, e.data, e.size);

	v->length ++;
}