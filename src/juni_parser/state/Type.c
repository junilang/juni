typedef struct {
	ParserState_HEADER
	Type type;
} ParserState_Type;

ParserState ParserState_Type_init(ParserState_Type *this, Type type) {
	PDBG("ParserState_Type_init: ",Type_repr(type));

	this->type = type;
	return ParserState_init(this, ParserStateClass_TYPE);
}

void ParserState_Type_dispatch(
	ParserState_Type *this, ParserContext *ctx, ParserBranch *branch, ParserStateIO *io
) {

}
