#ifndef Type_PTRTAG
	#define Type_PTRTAG PTRTAG
#endif

typedef STRUCTDECL(Type);

typedef u8 TypeCopyFlag; enum {
	FLAG_DEF(TypeCopyFlag_Clone)
};

typedef struct {
	Printable (*repr)(Ptr this);
	Type (*copy)(Ptr this, IARG(Alc, alc), TypeCopyFlag flags);
	AlcRes (*destroy)(Ptr this, IARG(Alc, alc));
	uhash (*hash)(Ptr this, uhash base);
	bool (*equal)(Ptr this, Ptr other);
} IType;

typedef enum : u8 {
	TypeSc_Enum,
	TypeSc_Const,
	TypeSc_Copy,
	TypeSc_Move
} TypeSc;


constexpr u8 TypeSc_bits = 2;
constexpr usize TypeSc_mask = 0b11ull;

#if Type_PTRTAG
	struct Type {
		union {
			usize raw_value;
			Ptr value;
		};
	};

	enum {
		IType_NullType_ID,
		IType_SetType_ID,
		IType_UnionType_ID,
		IType_KNOWN
	};

	INTERFACE_REGISTRY(IType, utag, 12)

	typedef utag TypeId;

	TypeSc Type_sc(Type this) {
		return (TypeSc)(ptrread(this.value) & TypeSc_mask);
	}

	Ptr Type_this(Type this) { return ptrstrip(this.value); }

	TypeId Type_id(Type this) {
		return ptrread(this.value) >> TypeSc_bits;
	}

	const IType *Type_iface(Type this) {
		return &IType__registry[Type_id(this)];
	}

	Type Type_ZZsetsc(Type this, TypeSc sc) {
		return (Type) {
			.value = ptrtag(ptrstrip(this.value), (utag)((ptrread(this.value) & (~TypeSc_mask)) | sc))
		};
	}

#else
	// ensure that bottom 2 bits of interface pointer
	// are usable for storage class tag
	static_assert(alignof(IType) > TypeSc_mask);

	typedef struct {
		Ptr value;
	} ITypeRef;

	struct Type {
		Ptr this;
		ITypeRef iface;
	};

	typedef usize TypeId;

	TypeSc Type_sc(Type this) {
		return (TypeSc)((usize)this.iface.value & TypeSc__MASK);
	}

	const IType *Type_iface(Type this) {
		return (const IType*)((usize)this.iface.value & (~TypeSc__MASK));
	}

	TypeId Type_id(Type this) {
		return (TypeId)Type_iface(this);
	}

	Ptr Type_this(Type this) { return this.this; }

	Type ZZType_setsc(Type this, TypeSc sc) {
		return (Type) {
			.this = this.this,
			.iface.value = (Ptr)(((usize)this.iface.value & (~TypeSc__MASK)) | (usize)sc)
		};
	}

#endif

Type Type_const(Type this) {
	switch (Type_sc(this)) {
		case TypeSc_Const:
		case TypeSc_Enum:
			return this;
		case TypeSc_Copy:
		case TypeSc_Move:
			return Type_ZZsetsc(this, TypeSc_Const);
	}

	UNREACHABLE;
}

bool Type_isconst(Type this) {
	switch (Type_sc(this)) {
		case TypeSc_Const:
		case TypeSc_Enum:
			return true;
		case TypeSc_Copy:
		case TypeSc_Move:
			return false;
	}

	UNREACHABLE;
}

Type Type_move(Type this) {
	switch (Type_sc(this)) {
		case TypeSc_Const:
		case TypeSc_Enum:
		case TypeSc_Move:
			return this;
		case TypeSc_Copy:
			return Type_ZZsetsc(this, TypeSc_Move);
	}

	UNREACHABLE;
}

bool Type_ismove(Type this) {
	switch (Type_sc(this)) {
		case TypeSc_Move:
			return true;
		case TypeSc_Const:
		case TypeSc_Enum:
		case TypeSc_Copy:
			return false;

	}

	UNREACHABLE;
}

Type Type_copy(Type this, Alc alc, TypeCopyFlag flags) {
	const TypeSc sc = Type_sc(this);
	if (sc == TypeSc_Enum) return this;

	if (flags & FLAG(TypeCopyFlag_Clone))
		goto do_copy;

	switch (sc) {
		case TypeSc_Const:
			return this;
		case TypeSc_Move:
			return Type_ZZsetsc(this, TypeSc_Copy);
		case TypeSc_Copy:
			goto do_copy;

		case TypeSc_Enum:
	}

	UNREACHABLE;

	do_copy:;
	return Type_iface(this)->copy(Type_this(this), IPASS(Allocator, alc), flags);
}

void Type_destroy(Type this, Allocator alc) {
	switch (Type_sc(this)) {
		case TypeSc_CONST:
		case TypeSc_ENUM:
			return;
		case TypeSc_MOVE:
		case TypeSc_COPY:
			Type_iface(this)->destroy(Type_this(this), IPASS(Allocator, alc));
			return;
	}

	UNREACHABLE;
}

Printable Type_repr(Type this) {
	return Type_iface(this)->repr(Type_this(this));
}

uhash Type_hash(Type this, uhash base) {
	const TypeId id = Type_id(this);
	if (Type_sc(this) == TypeSc_ENUM)
		return HASH(id, HASH(Type_this(this), base));

	return HASH(id, Type_iface(this)->hash(Type_this(this), base));
}

bool Type_equal(Type this, Type other) {
	if (Type_id(this) != Type_id(other)) return false;

	if (
		(Type_sc(this) == TypeSc_ENUM)

		#if BUILD_SAFE
			&& (Type_sc(other) == TypeSc_ENUM)
		#endif
	)
		return Type_this(this) == Type_this(other);

	return Type_iface(this)->equal(Type_this(this), Type_this(other));
}

#include "Type_meta.h"
