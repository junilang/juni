int ZZentry(testing_TestContext *ctx) {
	u32 *arr = Allocator_new(Malloc, sizeof(u32) * 12);

	for (usize i = 0; i < 12; i++) {
		arr[i] = (u32)i;
		FPRINT(stdout, "[",i,"]: ",arr[i],"\n");
	}

	Allocator_delete(Malloc, arr);

	return 0;
}
