#include <silver.h>

#include "util/_include.h"

#if BUILD_TESTING
	#include <silver_test.h>

	#define TEST_SUITE_BEGIN void juni_testsuite(SilverTestState *state) {
	#define TEST_RUN(test, name) SilverTest_run(state, String_from(name), &test##_entry);
	#define TEST_SUITE_END }

	#ifdef BUILD_TESTING_INCLUDE
		#include BUILD_TESTING_INCLUDE
	#else
		TEST_SUITE_BEGIN TEST_SUITE_END
	#endif

	int main(int argc, Str *argv, Str *envp) {
		return SilverTest_entry(argc, argv, &juni_testsuite);
	}

#else
	int main(int argc, Str *argv, Str *envp) {
		PRINTBP(Stdout, "Hello Juni <3\n");
		return 0;
	}

#endif
