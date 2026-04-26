int ZZentry(testing_TestContext *ctx) {
	Trie t = Trie_NULL;

	t = Trie_set(t, USTR("Héllo"), (Ptr)1, Malloc);
	t = Trie_set(t, USTR("Hé"), (Ptr)1, Malloc);
	t = Trie_set(t, USTR("Hello"), (Ptr)2, Malloc);

	Trie_print(t, FileOutStream_upcast(stdout));

	Trie_destroy(t, Malloc);

	return 0;
}
