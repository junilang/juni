int ZZentry(testing_TestContext *ctx) {
	OutStream os = FileOutStream_upcast(stdout);

	OutStream_write(os, USTR("OutStream test: Hello World!\n"));

	return 0;
}
