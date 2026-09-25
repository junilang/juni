int ZZentry(SilverTestContext *ctx) {
	ubyte mem[4096];
	StaticAlc *alc_ = StaticAlc_init(mem, sizeof(mem));
	Alc alc = StaticAlc_upcast(alc_);

	Trie trie = Trie_null;
	trie = Trie_set(trie, alc, STR("hello world"), (Ptr)1);
	AlcRes_UNWRAP(Trie_geterr(trie));
	trie = Trie_set(trie, alc, STR("hello guys"), (Ptr)2);
	AlcRes_UNWRAP(Trie_geterr(trie));
	trie = Trie_set(trie, alc, STR("henlo world"), (Ptr)3);
	AlcRes_UNWRAP(Trie_geterr(trie));

	OutStreamRes_UNWRAP(Trie_print(trie, Stdout));

	return 0;
}
