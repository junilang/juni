typedef SetType UnionType;

IType_DECLARE(UnionType, UnionType*)

Type UnionType_create(TypeSet *set) {
	#if BUILD_SAFE
		if (set->size == 0) return NullType;
	#endif

	return UnionType_upcast(ZZSetType_create(set));
}

void UnionType_print(UnionType *this, OutStream os) {
	PRINT(os, "[");
	ZZSetType_printinner(this, os);
	PRINT(os, "]");
}

Type UnionType_copy(SetType *this, TypeCopyFlag flags, Allocator alc) {
	return UnionType_upcast(ZZSetType_copy(this, flags, alc));
}

#define UnionType_hash SetType_hash
#define UnionType_destroy SetType_destroy
#define UnionType_equal SetType_equal

IPrintable_GENERATE(UnionType, UnionType*)
IType_GENERATE_KNOWN(UnionType, UnionType*)
