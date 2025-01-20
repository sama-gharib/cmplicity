#include "automaton.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
	printf("%s\n", "Hello World !");

	State q0 = CreateState(true);
	State q1 = CreateState(true);
	AddTransition(&q0, RangeTransition('0', '9', &q0));
	AddTransition(&q0, ClassicTransition('.', &q1));
	AddTransition(&q1, RangeTransition('0', '9', &q1));

	printf("%d %d %d %d\n", 
		Match(&q0, "9.6682", 0),
		Match(&q0, "12.56.1", 0),
		Match(&q0, "6844", 0),
		Match(&q0, "abab", 0)
	);

	printf("%s\n", "Done !");

	return EXIT_SUCCESS;
}