int ZZentry(testing_TestContext *ctx) {
	static const char *screpr[] = {
		[TypeSc_CONST] = "const",
		[TypeSc_MOVE] = "move",
		[TypeSc_COPY] = "copy"
	};

	Type T = {0};

	FPRINT(stdout, screpr[Type_sc(Type_const(T))],"\n");

	return 0;
}
