typedef ParserStateIO ParserInlineIO;
typedef void (*ParserInline)(
	ParserState state, ParserContext *ctx, ParserBranch *branch, ParserInlineIO *io
);
