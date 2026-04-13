int ZZentry(testing_TestContext *ctx) {
	Trie t;
	t = Trie_create(USTR("hello"), (Ptr)0, StdAlloc);
	t = Trie_set(t, USTR("hi"), (Ptr)1, StdAlloc);
	t = Trie_set(t, USTR("henlo"), (Ptr)2, StdAlloc);

	Trie_print(t, 0, FileOutStream_upcast(stdout));
	FPRINT(stdout, "\n");

	Trie_destroy(t, StdAlloc);

	return 0;
}
