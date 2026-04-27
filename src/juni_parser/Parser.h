typedef enum : u8 {
	ParserResult_OK,
	ParserResult_ERROR
} ParserResult;

typedef struct {
	OutStream log;
	Allocator state_alc;
	Allocator tmp_alc;
} ParserContext;

typedef struct {
	SourcePos pos;
	SourceRef src;
	Trie symdb;
} ParserSubrIO;

typedef ParserResult (*ParserSubr)(ParserContext *ctx, ParserSubrIO *io);

ParserResult Parser_entry(ParserContext *ctx, ParserSubrIO *io);
