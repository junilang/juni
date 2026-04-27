void ParserState_dispatch(
	ParserState this, ParserContext *ctx, ParserBranch *branch, ParserStateIO *io
) {
	PDBGL("-> ",ParserStateClass_NameRepr[ParserState_class(this)]," dispatch");
	PDBG_PUSH;

	switch (ParserState_class(this)) {
		#define X(E, N) \
			case ParserStateClass_##E: \
				N##_dispatch(ParserState_this(this), ctx, branch, io); \
				break;

			ParserState_XS

		#undef X

		default: UNREACHABLE;
	}

	PDBG_POP;
}
