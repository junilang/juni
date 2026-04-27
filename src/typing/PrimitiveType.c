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

IPrintable_GENERATE(PrimitiveType, EPrimitiveType)
IType_GENERATE_ENUM(PrimitiveType, EPrimitiveType)

#define PrimitiveType(N) (PrimitiveType_upcast(PrimitiveType_##N))
