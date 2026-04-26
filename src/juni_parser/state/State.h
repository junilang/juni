#ifndef ParserState_PTRTAG
	#define ParserState_PTRTAG PTRTAG
#endif

#define ParserState_XS \
	X(NONE, ParserState_None) \
	X(TYPE, ParserState_Type)

typedef struct {
	Ptr value;
} ParserState;

typedef enum : u8 {
	#define X(E, N) ParserStateClass_##E,
		ParserState_XS
	#undef X
} ParserStateClass;

#if ParserState_PTRTAG

	#define ParserState_HEADER

	ParserState ParserState_upcast(Ptr this, ParserStateClass class) {
		return (ParserState){ptrtag(this, class)};
	}

	ParserState ParserState_init(Ptr this, ParserStateClass class) {
		return ParserState_upcast(this, class);
	}

	ParserStateClass ParserState_class(ParserState this) {
		return (ParserStateClass)ptrread(this.value);
	}

	Ptr ParserState_this(ParserState this) {
		return ptrstrip(this.value);
	}

#else

	#define ParserState_HEADER ParserStateClass IParserState_class;

	ParserState ParserState_init(Ptr this, ParserStateClass class) {
		*(ParserStateClass*)this = class;
		return (ParserState){this};
	}

	ParserState ParserState_upcast(Ptr this, ParserStateClass class) {
		return (ParserState){this};
	}

	ParserStateClass ParserState_class(ParserState this) {
		return *(ParserStateClass*)this.value;
	}

	Ptr ParserState_this(ParserState this) {
		return this.value;
	}
#endif

typedef struct {
	SourcePos pos;
	Trie symdb;
} ParserStateIO;

void ParserState_dispatch(
	ParserState this, ParserContext *ctx, ParserBranch *branch, ParserStateIO *io
);
