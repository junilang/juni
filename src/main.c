#include <stdio.h>

#if BUILD_TESTING
	#include "test.c"
#endif

int main(int argc, char **argv) {
	#if BUILD_TESTING
		return testing_entry(argc, argv);
	#else
		printf("Hello world!\n");
	#endif
}
