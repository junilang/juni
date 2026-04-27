int ZZentry(testing_TestContext *ctx) {
	PDBG_INIT(FileOutStream_upcast(stderr));

	SymbolHeader sym_wsp = { .next = Symbol_NULL };
	SymbolHeader sym_tkn = { .next = Symbol_NULL };

	Ptr sym_wsp_val = Symbol_init(&sym_wsp, SymbolClass_WHITESPACE).value;
	Ptr sym_tkn_val = Symbol_init(&sym_tkn, SymbolClass_TOKEN).value;

	Trie db = Trie_NULL;

	#define X(C, V) db = Trie_set(db, (ubyte[]){C}, 1, V, Malloc)
		X(9, sym_wsp_val);
		X(10, sym_wsp_val);
		X(13, sym_wsp_val);
		X(32, sym_wsp_val);
		X('(', sym_tkn_val);
		X(')', sym_tkn_val);
		X('{', sym_tkn_val);
		X('}', sym_tkn_val);
	#undef X

	Symbol_Type sym_type_a = {.type = PrimitiveType(A)};
	Symbol_Type sym_type_b = {.type = PrimitiveType(B)};

	db = Trie_set(db, USTR("A"), Symbol_init(&sym_type_a.head, SymbolClass_TYPE).value, Malloc);
	db = Trie_set(db, USTR("B"), Symbol_init(&sym_type_b.head, SymbolClass_TYPE).value, Malloc);

	SourceRef src = Source_openfile("tests/juni_parser/example.juni", Malloc);

	ParserSubrIO subr_io = {
		.pos = SourcePos_NULL,
		.src = src,
		.symdb = db
	};

	ParserContext parser_ctx = {
		.state_alc = Malloc,
		.tmp_alc = Malloc
	};

	[[maybe_unused]]
	ParserResult result = Parser_entry(&parser_ctx, &subr_io);

	Trie_destroy(db, Malloc);
	Source_release(src);

	return 0;

}
