#ifndef Type_PTRTAG
	#define Type_PTRTAG PTRTAG
#endif

#ifndef Type_SAFE
	#define Type_SAFE BUILD_SAFE
#endif

#include "TypePtr.h"
#include "TypeHash.h"

typedef STRUCTDECL(Type);
typedef struct {
	void (*asFmt)(TypePtr this, Fmt *out_fmt);
	AlcRes (*copy)(TypePtr this, IARG(Alc, alc), Type *out_copy);
	AlcRes (*destroy)(TypePtr this, IARG(Alc, alc));
	TypeHash (*hash)(TypePtr this, TypeHash base);
	bool (*equal)(TypePtr this, TypePtr other);
	bool (*lmatch)(TypePtr this, IARG(Type, other));
	bool (*rmatch)(TypePtr this, IARG(Type, other));
} IType;

#if Type_PTRTAG
	struct Type {
		union {
			Ptr value;
			usize raw_value;
		};
	};

	#ifndef IType_SLOTS
		#define IType_SLOTS 16
	#endif

	constexpr auto IType_slots = IType_SLOTS;

	enum {
		IType_NullType_ID,
		IType_SetType_ID,
		IType_UnionType_ID,
		IType_PrimitiveType_ID,
		IType_reserved
	};

	typedef utag TypeId;

	IFACEDB(IType, TypeId, IType_slots, IType_reserved);

	TypePtr Type_this(Type this) {
		return ptrstrip(this.value);
	}

	TypeId Type_id(Type this) {
		return ptrread(this.value);
	}

	const IType *Type_iface(Type this) {
		return IFACEDB_GET(IType, Type_id(this));
	}

	TypeSc Type_sc(Type vthis) {
		return TypePtr_sc(Type_this(vthis));
	}

	Type Type_setsc_(Type vthis, TypeSc sc) {
		auto const id = Type_id(vthis);
		auto const this = Type_this(vthis);

		return (Type) {
			.value = ptrtag(TypePtr_fromptr(TypePtr_ptr(this), sc), id)
		};
	}

#else
	struct Type {
		TypePtr this;
		const IType *iface;
	};

	typedef usize TypeId;

	TypePtr Type_this(Type this) {
		return this.this;
	}

	const IType *Type_iface(Type this) {
		return this.iface;
	}

	TypeId Type_id(Type this) {
		return (TypeId)Type_iface(this);
	}

	Type Type_setsc_(Type vthis, TypeSc sc) {
		auto const iface = Type_iface(vthis);
		auto const this = Type_this(vthis);

		return (Type) {
			.this = TypePtr_fromptr(TypePtr_ptr(this), sc),
			.iface = iface
		};
	}

#endif

Type Type_const(Type this) {
	switch (Type_sc(this)) {
		case TypeSc_Enum:
		case TypeSc_Const:
			return this;

		case TypeSc_Copy:
		case TypeSc_Clone:
		case TypeSc_Move:
			return Type_setsc_(this, TypeSc_Const);

		default:
			UNREACHABLE(Type_SAFE);
	}
}

bool Type_isconst(Type this) {
	switch (Type_sc(this)) {
		case TypeSc_Enum:
		case TypeSc_Const:
			return true;

		case TypeSc_Copy:
		case TypeSc_Clone:
		case TypeSc_Move:
			return false;

		default:
			UNREACHABLE(Type_SAFE);
	}
}

Type Type_move(Type this) {
	switch (Type_sc(this)) {
		case TypeSc_Enum:
		case TypeSc_Const:
		case TypeSc_Move:
			return this;

		case TypeSc_Copy:
		case TypeSc_Clone:
			return Type_setsc_(this, TypeSc_Move);

		default:
			UNREACHABLE(Type_SAFE);
	}
}

bool Type_ismove(Type this) {
	switch (Type_sc(this)) {
		case TypeSc_Enum:
		case TypeSc_Const:
		case TypeSc_Copy:
		case TypeSc_Clone:
			return false;

		case TypeSc_Move:
			return true;

		default:
			UNREACHABLE(Type_SAFE);
	}
}

Type Type_clone(Type this) {
	switch (Type_sc(this)) {
		case TypeSc_Enum:
		case TypeSc_Clone:
			return this;

		case TypeSc_Const:
		case TypeSc_Copy:
		case TypeSc_Move:
			return Type_setsc_(this, TypeSc_Clone);

		default:
			UNREACHABLE(Type_SAFE);
	}
}

bool Type_isclone(Type this) {
	switch (Type_sc(this)) {
		case TypeSc_Enum:
		case TypeSc_Const:
		case TypeSc_Copy:
		case TypeSc_Move:
			return false;

		case TypeSc_Clone:
			return true;

		default:
			UNREACHABLE(Type_SAFE);
	}
}


AlcRes Type_copy(Type this, Alc alc, Type *out_copy) {
	switch (Type_sc(this)) {
		case TypeSc_Enum:
		case TypeSc_Const:
		case TypeSc_Move:
			*out_copy = this;
			return AlcRes_Ok;

		case TypeSc_Copy:
		case TypeSc_Clone:
			return Type_iface(this)->copy(Type_this(this), IPASS(Alc, alc), out_copy);

		default:
			UNREACHABLE(Type_SAFE);
	}
}

AlcRes Type_destroy(Type this, Alc alc) {
	switch (Type_sc(this)) {
		case TypeSc_Enum:
		case TypeSc_Const:
			return AlcRes_Ok;
		case TypeSc_Copy:
		case TypeSc_Clone:
		case TypeSc_Move:
			return Type_iface(this)->destroy(Type_this(this), IPASS(Alc, alc));

		default:
			UNREACHABLE(Type_SAFE);
	}
}

Fmt Type_asFmt(Type this) {
	Fmt result;
	Type_iface(this)->asFmt(Type_this(this), &result);
	return result;
}

TypeHash Type_hash(Type this, TypeHash base) {
	auto const thisp = Type_this(this);

	switch (Type_sc(this)) {
		case TypeSc_Enum:
			auto const id = Type_id(this);
			auto const val = TypePtr_ptr(thisp);
			return Type_HASH(Type_HASH(TypeHash_base, id), (usize)val);

		case TypeSc_Const:
		case TypeSc_Copy:
		case TypeSc_Clone:
		case TypeSc_Move:
			return Type_iface(this)->hash(Type_this(this), base);

		default:
			UNREACHABLE(Type_SAFE);
	}
}

bool Type_equal(Type this, Type other) {
	if (Type_id(this) != Type_id(other))
		return false;

	auto const thisp = Type_this(this);
	auto const otherp = Type_this(other);

	if (TypePtr_ptr(thisp) == TypePtr_ptr(otherp))
		return true;

	return Type_iface(this)->equal(thisp, otherp);
}

bool Type_lmatch(Type this, Type other) {
	return Type_iface(this)->lmatch(Type_this(this), IPASS(Type, other));
}

bool Type_rmatch(Type this, Type other) {
	return Type_iface(this)->rmatch(Type_this(this), IPASS(Type, other));
}
