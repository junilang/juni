#define XS \
	X(A, "TypeA") \
	X(B, "TypeB")

typedef enum : u8 {
	#define X(N, _) PrimitiveType_##N,
		XS
	#undef X
} EPrimitiveType;

const String PrimitiveType__StrMap[] = {
	#define X(N, P) [PrimitiveType_##N] = STRING(P),
		XS
	#undef X
};

#undef XS

IType_DECLARE(PrimitiveType, EPrimitiveType)

void PrimitiveType_print(EPrimitiveType this, OutStream os) {
	PRINT(os, PrimitiveType__StrMap[this]);
}

void PrimitiveType_destroy(EPrimitiveType this, Allocator alc) {}

Type PrimitiveType_copy(EPrimitiveType this, TypeCopyFlag flags, Allocator alc) {
	return Type_const(PrimitiveType_upcast(this));
}

uhash PrimitiveType_hash(EPrimitiveType this, uhash base) {
	return HASH(this, base);
}

bool PrimitiveType_equal(EPrimitiveType this, EPrimitiveType other) {
	return this == other;
}

IPrintable_GENERATE(PrimitiveType, EPrimitiveType)
IType_GENERATE(PrimitiveType, EPrimitiveType)

#define PrimitiveType(N) (Type_const(PrimitiveType_upcast(PrimitiveType_##N)))
