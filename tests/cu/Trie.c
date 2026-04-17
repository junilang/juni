void ZZprint_trie(Trie t) {
	Trie_print(t, 0, FileOutStream_upcast(stdout));
	FPRINT(stdout, "\n");
}

int ZZentry(testing_TestContext *ctx) {
	Trie t;
	t = Trie_create(USTR("hello"), (Ptr)0, Malloc);
	ZZprint_trie(t);
	t = Trie_set(t, USTR("hi"), (Ptr)1, Malloc);
	ZZprint_trie(t);
	t = Trie_set(t, USTR("henlo"), (Ptr)2, Malloc);
	ZZprint_trie(t);
	t = Trie_unset(t, USTR("hello"), Malloc);
	ZZprint_trie(t);

	Trie_destroy(t, Malloc);

	return 0;
}
