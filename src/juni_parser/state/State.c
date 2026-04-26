void ParserState_dispatch(
	ParserState this, ParserContext *ctx, ParserBranch *branch, ParserStateIO *io
) {
	switch (ParserState_class(this)) {
		#define X(E, N) \
			case ParserStateClass_##E: \
				return N##_dispatch(ParserState_this(this), ctx, branch, io);

			ParserState_XS

		#undef X
	}

	UNREACHABLE;
}
