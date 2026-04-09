typedef struct {
	int x;
} testing_TestContext;

typedef struct {
	testing_TestContext testctx;
	int total;
	int failed;
} testing_Context;

void testing_run(
	testing_Context *ctx,
	const char *name,
	int (*entry)(testing_TestContext*)
) {

	fprintf(stdout, "> %s\n", name);

	ctx->total++;
	int code = entry(&ctx->testctx);

	if (code) {
		ctx->failed++;
		fprintf(stdout, "> %s failed: %d \n", name, code);
	}
}

#define TEST_SUITE_BEGIN void testing_runtests(testing_Context *ctx) {
#define TEST_RUN(test) testing_run(ctx, #test, &test##_entry);
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

	if (ctx.failed) {
		fprintf(stdout, "\n> %d tests failed\n", ctx.failed);
	}

	return 0;
}
