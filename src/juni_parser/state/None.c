void ParserState_None_dispatch(
	Ptr this, ParserContext *ctx, ParserBranch *branch, ParserStateIO *io
) {
	const SourceRef src = branch->src;
	SourcePos pos = io->pos;

	next_symbol:;
	Symbol sym = Parser_getsymbol(ctx, io->symdb, src, &pos);

	switch (Symbol_class(sym)) {
		case SymbolClass_WHITESPACE:
			goto next_symbol;
		case SymbolClass_TYPE:
			goto Btype;

		default:
	}

	if (0) Btype: {
		const Symbol_Type *symdata = Symbol_this(sym);
		ParserState_Type next;

		ParserState_Type_init(&next, Type_const(symdata->type));

		ParserStateIO next_io = {
			.pos = pos,
			.symdb = io->symdb
		};

		ParserState_Type_dispatch(&next, ctx, branch, &next_io);
	}
}
