TEST(ZZ) {
	Type t = PrimitiveType_asType(PrimitiveType_Int);
	PRINTA(ctx->log, Type_asFmt(t), " ", Type_hash(t, TypeHash_base), "\n");
	return 0;
}
