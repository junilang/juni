int ZZentry(testing_TestContext *ctx) {
	Vec vec; Vec_einit(&vec);


	for (int i = 0; i < 16; i++) {
		*(int*)Vec_append(&vec, sizeof(int), StdAlloc) = i;
	}

	int *it = Vec_begin(&vec);
	int *end = Vec_end(&vec);

	for (; it < end; it++) {
		FPRINT(stdout, *it,"\n");
	}

	return 0;
}
