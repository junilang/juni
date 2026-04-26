int ZZentry(testing_TestContext *ctx) {
	TypeSet set; TypeSet_init(&set, Malloc, Malloc);

	Type types[] = {
		PrimitiveType(B),
		PrimitiveType(A),
		PrimitiveType(A),
		PrimitiveType(A),
		PrimitiveType(A),
		PrimitiveType(B),
		PrimitiveType(B),
		PrimitiveType(B),
		PrimitiveType(A),
	};

	TypeSet_add(&set, types, sizeof(types) / sizeof(Type), FLAG(TypeSetFlag));

	Type T = UnionType_create(&set);

	FPRINT(stdout, Type_repr(T),"\n");

	TypeSet_add(&set, &T, 1, FLAG(TypeSetFlag, MOVE));

	Type S = SetType_create(&set);

	TypeSet_destroy(&set);

	FPRINT(stdout, Type_repr(S),"\n");

	Type_destroy(T, Malloc);
	Type_destroy(S, Malloc);

	return 0;
}
