ParserResult Parser_entry(ParserContext *ctx, ParserSubrIO *io) {
	SourcePos pos = io->pos;
	Trie symdb = io->symdb;

	ParserBranch branch = {
		.src = io->src
	};

	ParserStateIO state_io = {
		.pos = pos,
		.symdb = symdb
	};

	ParserState state = ParserState_None;

	ParserState_dispatch(state, ctx, &branch, &state_io);

	return ParserResult_OK;
}
