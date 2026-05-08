int ZZentry(SilverTestContext *ctx) {
	FPRINT(stdout, "type: ", Type_repr(PrimitiveType(A)),"\n");
	FPRINT(stdout, "type: ", Type_repr(PrimitiveType(B)),"\n");
	FPRINT(stdout, Type_equal(PrimitiveType(A), PrimitiveType(A)),"\n");
	return 0;
}
