int ZZentry(SilverTestContext *ctx) {
	Str path = "tests/parser/example.juni";

	FILE *file = fopen(path, "r");
	if (!file) PANIC("failed to open file");

	SourceRef s = Source_fromfile(STRING(path), file, Malloc);

	fclose(file);

	const Source *sd = Source_data(s);
	FPRINT(stdout, sd->data_size,"\n");

	Source_release(s);

	return 0;
}
