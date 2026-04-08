typedef struct {
	int x;
} testing_TestContext;

typedef struct {
	testing_TestContext testctx;
	int total;
	int failed;
} testing_Context;

void testing_run(testing_Context *ctx, int (*test_fn)(testing_TestContext*)) {
	ctx->total++;
	if (test_fn(&ctx->testctx)) {
		ctx->failed++;
	}
}

#define TEST_SUITE_BEGIN void testing_runtests(testing_Context *ctx) {
#define TEST_RUN(test) testing_run(ctx, test##_entry);
#define TEST_SUITE_END }

#ifdef BUILD_TESTING_INCLUDE
	#include BUILD_TESTING_INCLUDE
#else
	void testing_runtests(testing_Context *ctx);
#endif

#undef TEST_SUITE_BEGIN
#undef TEST_RUN
#undef TEST_SUITE_END

int testing_entry(int argc, char **argv) {
	testing_Context ctx = {
		.total = 0,
		.failed = 0
	};

	testing_runtests(&ctx);

	return 0;
}
