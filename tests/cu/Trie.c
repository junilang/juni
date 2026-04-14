int ZZentry(testing_TestContext *ctx) {
	Trie t;
	t = Trie_create(USTR("hello"), (Ptr)0, Malloc);
	t = Trie_set(t, USTR("hi"), (Ptr)1, Malloc);
	t = Trie_set(t, USTR("henlo"), (Ptr)2, Malloc);

	Trie_print(t, 0, FileOutStream_upcast(stdout));
	FPRINT(stdout, "\n");

	Trie_destroy(t, Malloc);

	return 0;
}
