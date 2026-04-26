#if ParserState_PTRTAG
	#define ParserState_None ParserState_upcast(nullptr, ParserStateClass_NONE)
#else
	const ParserStateClass ZZParserState_None = ParserStateClass_NONE;
	#define ParserState_None ParserState_upcast(&ZZParserState_None, ParserStateClass_NONE)
#endif
