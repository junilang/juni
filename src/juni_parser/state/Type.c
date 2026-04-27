typedef struct {
	ParserState_HEADER
	Type type;
} ParserState_Type;

ParserState ParserState_Type_init(ParserState_Type *this, Type type) {
	PDBGL("ParserState_Type_init: ",Type_repr(type));

	this->type = type;
	return ParserState_init(this, ParserStateClass_TYPE);
}

void ParserState_Type_dispatch(
	ParserState_Type *this, ParserContext *ctx, ParserBranch *branch, ParserStateIO *io
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

		TypeSet typeset; TypeSet_init(&typeset, ctx->tmp_alc, ctx->state_alc);
		TypeSet_add(&typeset, (Type[]){
			this->type, Type_const(symdata->type)
		}, 2, FLAG(TypeSetFlag));

		ParserState_Type_init(&next, SetType_create(&typeset));

		TypeSet_destroy(&typeset);

		ParserStateIO next_io = {
			.pos = pos,
			.symdb = io->symdb
		};

		ParserState_Type_dispatch(&next, ctx, branch, &next_io);
	}
}
