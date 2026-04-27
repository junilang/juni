typedef struct {
	TypeSetSize size;
	Type elements[];
} SetType;

IType_DECLARE(SetType, SetType*)

usize ZZSetType_allocsize(TypeSetSize size) {
	#if BUILD_SAFE
		return usize_pflx(offsetof(SetType, elements), sizeof(Type), size);
	#else
		return offsetof(SetType, elements) + sizeof(Type) * size;
	#endif
}

SetType *ZZSetType_create(TypeSet *set) {
	auto const size = set->size;
	SetType *this = Allocator_new(set->type_alc, ZZSetType_allocsize(size));
	this->size = size;

	for (TypeSetSize i = 0; i < size; i++) {
		this->elements[i] = set->elements[i].type;
	}

	return this;
}

void ZZSetType_printinner(SetType *this, OutStream os) {
	#if BUILD_SAFE
		if (this->size == 0) return;
	#endif

	Type *it = this->elements;
	Type *end = it + (this->size - 1);
	for (; it < end; it++) {
		PRINT(os, Type_repr(*it),", ");
	}

	PRINT(os, Type_repr(*it));
}

Type SetType_create(TypeSet *set) {
	#if BUILD_SAFE
		if (set->size == 0) return NullType;
	#endif

	return SetType_upcast(ZZSetType_create(set));
}

void SetType_print(SetType *this, OutStream os) {
	PRINT(os, "{");
	ZZSetType_printinner(this, os);
	PRINT(os, "}");
}

void SetType_destroy(SetType *this, Allocator alc) {
	Type *it = this->elements;
	Type *end = it + this->size;

	for (; it < end; it++) {
		Type_destroy(*it, alc);
	}

	Allocator_delete(alc, this);
}

uhash SetType_hash(SetType *this, uhash base) {
	Type *it = this->elements;
	Type *end = it + this->size;
	for (; it < end; it++) {
		base = Type_hash(*it, base);
	}

	return base;
}

SetType *ZZSetType_copy(SetType *this, TypeCopyFlag flags, Allocator alc) {
	auto const size = this->size;

	SetType *copy = Allocator_new(alc, ZZSetType_allocsize(size));
	copy->size = size;

	for (usize i = 0; i < size; i++) {
		copy->elements[i] = Type_copy(this->elements[i], flags, alc);
	}

	return copy;
}

Type SetType_copy(SetType *this, TypeCopyFlag flags, Allocator alc) {
	return SetType_upcast(ZZSetType_copy(this, flags, alc));
}

bool SetType_equal(SetType *this, SetType *other) {
	const usize size = this->size;
	if (size != other->size) return false;

	for (usize i = 0; i < size; i++) {
		if (!Type_equal(this->elements[i], other->elements[i])) return false;
	}

	return true;
}

IPrintable_GENERATE(SetType, SetType*)
IType_GENERATE_KNOWN(SetType, SetType*)
