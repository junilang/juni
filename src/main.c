#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>



#include "cu/__module__.h"
#include "cu/__module__.c"



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
