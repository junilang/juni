#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbit.h>
#include <stdatomic.h>

#include <silver/_include.h>

#include "typing/_include.h"
#include "typing/_include.c"

#include "parser/_include.h"
#include "parser/_include.c"

#if BUILD_TESTING
	#define TEST_SUITE_BEGIN void libsilver_testsuite(SilverTestState *state) {
	#define TEST_RUN(test, name) SilverTest_run(state, STRING(name), &test##_entry);
	#define TEST_SUITE_END }

	#ifdef BUILD_TESTING_INCLUDE
		#include BUILD_TESTING_INCLUDE
	#else
		TEST_SUITE_BEGIN TEST_SUITE_END
	#endif

	int main(int argc, char **argv) {
		return SilverTest_entry(argc, argv, &libsilver_testsuite);
	}

#else
	int main(int argc, char **argv) {
		FPRINT(stdout, "Hello Juni <3\n");
		return 0;
	}


#endif
