#ifndef Type_PTRTAG
	#define Type_PTRTAG PTRTAG
#endif

typedef STRUCTDECL(Type);

typedef enum : u8 {
	TypeCopyFlag_BIT_DUPLICATE
} TypeCopyFlag;

typedef struct {
	Printable (*repr)(Ptr this);
	Type (*copy)(Ptr this, TypeCopyFlag flags, IARG(Allocator, alc));
	void (*destroy)(Ptr this, IARG(Allocator, alc));
	uhash (*hash)(Ptr this, uhash base);
	bool (*equal)(Ptr this, Ptr other);
} IType;

typedef enum : u8 {
	TypeSc_COPY,
	TypeSc_MOVE,
	TypeSc_CONST
} TypeSc;

#define TypeSc__BITS 2
#define TypeSc__MASK (0b11ULL)

#if Type_PTRTAG
	struct Type {
		Ptr value;
	};

	enum {
		IType_KNOWN
	};

	INTERFACE_REGISTRY(IType, utag, 12)

	typedef utag TypeId;

	TypeSc Type_sc(Type this) {
		return (TypeSc)(ptrread(this.value) & TypeSc__MASK);
	}

	Ptr Type_this(Type this) { return ptrstrip(this.value); }

	TypeId Type_id(Type this) {
		return ptrread(this.value) >> TypeSc__BITS;
	}

	const IType *Type_iface(Type this) {
		return &IType__registry[Type_id(this)];
	}

	Type ZZType_setsc(Type this, TypeSc sc) {
		return (Type) {
			(Ptr)(((usize)this.value & (~TypeSc__MASK)) | (usize)sc)
		};
	}

#else
	// ensure that bottom 2 bits of interface pointer
	// are usable for storage class tag
	static_assert(alignof(IType) > TypeSc__BITS);

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
		case TypeSc_CONST:
			return this;
		case TypeSc_COPY:
		case TypeSc_MOVE:
			return ZZType_setsc(this, TypeSc_CONST);
	}

	UNREACHABLE;
}

Type Type_move(Type this) {
	switch (Type_sc(this)) {
		case TypeSc_CONST:
		case TypeSc_MOVE:
			return this;
		case TypeSc_COPY:
			return ZZType_setsc(this, TypeSc_MOVE);
	}

	UNREACHABLE;
}

Type Type_copy(Type this, TypeCopyFlag flags, Allocator alc) {
	if (flags & FLAG(TypeCopyFlag, DUPLICATE))
		goto do_copy;

	switch (Type_sc(this)) {
		case TypeSc_CONST:
			return this;
		case TypeSc_MOVE:
			return ZZType_setsc(this, TypeSc_COPY);
		case TypeSc_COPY:
			goto do_copy;
	}

	UNREACHABLE;

	do_copy:;
	return Type_iface(this)->copy(Type_this(this), flags, IPASS(Allocator, alc));
}

void Type_destroy(Type this, Allocator alc) {
	switch (Type_sc(this)) {
		case TypeSc_CONST:
			return;
		case TypeSc_MOVE:
		case TypeSc_COPY:
			Type_iface(this)->destroy(Type_this(this), IPASS(Allocator, alc));
	}

	UNREACHABLE;
}

Printable Type_repr(Type this) {
	return Type_iface(this)->repr(Type_this(this));
}

uhash Type_hash(Type this, uhash base) {
	return Type_iface(this)->hash(Type_this(this), base);
}

bool Type_equal(Type this, Type other) {
	if (Type_id(this) != Type_id(other)) return false;
	return Type_iface(this)->equal(Type_this(this), Type_this(other));
}

#include "Type_meta.h"
