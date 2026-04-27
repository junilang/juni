IType_DECLARE(NullType, Ptr)

void NullType_print(Ptr this, OutStream os) {
	OutStream_write(os, USTR("NullType"));
}

bool NullType_equal(Ptr this, Ptr other) {
	return true;
}

Type NullType_copy(Ptr this, TypeCopyFlag flags, Allocator alc) {
	return NullType_upcast(nullptr);
}

void NullType_destroy(Ptr this, Allocator alc) {}

uhash NullType_hash(Ptr this, uhash base) {
	return base;
}

IPrintable_GENERATE(NullType, Ptr)
IType_GENERATE_KNOWN(NullType, Ptr)

#define NullType NullType_upcast(nullptr)
