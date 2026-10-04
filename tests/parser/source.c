int ZZentry(SilverTestContext *ctx) {
	StaticAlc_LOCAL(alc, 4096);

	String path = String_INIT("tests/parser/source.txt");

	FsFile file; {
		auto res = FsFile_open(path, FsFileMode_Read, &file);
		if (res) {
			PRINTP(Stdout, "FsFile_open: ",FsRes_repr[res],"\n");
			return 1;
		}
	}

	FsMapping mapping; {
		auto res = FsFile_map(file, FsMappingMode_Read, &mapping);
		if (res) {
			PRINTP(Stdout, "FsFile_map: ",FsRes_repr[res],"\n");
			return 1;
		}
	}

	Source source; {
		auto res = Source_create(
			alc,
			path.data, (u16)path.size,
			FsMapping_data(&mapping),
			FsMapping_size(&mapping),
			&source
		);

		if (res) {
			PRINTP(Stdout, "Source_create: ",AlcRes_repr[res],"\n");
			return 1;
		}
	}

	{
		auto res = FsMapping_unmap(&mapping);
		if (res) {
			PRINTP(Stdout, "FsMapping_unmap: ",FsRes_repr[res],"\n");
			return 1;
		}
	}

	{
		auto res = FsFile_close(file);
		if (res) {
			PRINTP(Stdout, "FsFile_close: ",FsRes_repr[res],"\n");
			return 1;
		}
	}

	auto data = Source_at(source, 0);
	auto name = Source_name(source);
	PRINTP(Stdout, "(",name,") file_content: ",data);

	{
		auto res = Source_release(source);
		if (res) {
			PRINTP(Stdout, "Source_release ",AlcRes_repr[res],"\n");
			return 1;
		}
	}

	return 0;
}
