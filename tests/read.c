int ZZentry(SilverTestContext *ctx) {
	iword res = linux_openat2(AT_FDCWD, "tests/text", &(struct open_how) {
		.flags = O_RDONLY, .mode = 0
	}, sizeof(struct open_how));

	ASSERT(res >= 0, "failed to open file");

	auto fd = (linux_fd)res;

	struct statx info;

	res = linux_statx(fd, nullptr, AT_EMPTY_PATH, STATX_SIZE, &info);
	PRINTP(Stdout, res,"\n");

	ASSERT(res >= 0, "failed to stat file");

	res = linux_mmap(nullptr, info.stx_size, PROT_READ, MAP_PRIVATE, fd, 0);

	PRINTP(Stdout, res,"\n");

	ASSERT(!linux_mmap_iserror(res));

	auto file = (const ubyte*)res;

	OutStreamRes_UNWRAP(OutStream_write(Stdout, file, info.stx_size));

	res = linux_munmap((Ptr)file, info.stx_size);

	ASSERT(res >= 0);

	return 0;
}
