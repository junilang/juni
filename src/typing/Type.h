#ifndef Type_PTRTAG
	#define Type_PTRTAG PTRTAG
#endif

#ifndef Type_SAFE
	#define Type_SAFE BUILD_SAFE
#endif

typedef STRUCTDECL(Type);

typedef enum : u8 {
	TypeSc_Enum,
	TypeSc_Const,
	TypeSc_Copy,
	TypeSc_Clone,
	TypeSc_Move,
} TypeSc;

constexpr auto TypeSc_width = 3;
constexpr usize TypeSc_mask = FIELD_MASK_LOWER(usize, TypeSc_width);

#if Type_PTRTAG
	constexpr usize TypePtr_maxenum = ptrtag_addr_mask >> TypeSc_width;
#else
	constexpr usize TypePtr_maxenum = usize_max >> TypeSc_width;
#endif

typedef UNIQUEPTR(TypePtr);

TypeSc TypePtr_sc(TypePtr this) {
	return (TypeSc)((usize)this & TypeSc_mask);
}

Ptr TypePtr_ptr(TypePtr this) {
	return (Ptr)((usize)this & (~TypeSc_mask));
}

usize TypePtr_enum(TypePtr this) {
	return (usize)this >> TypeSc_width;
}

TypePtr TypePtr_fromptr(Ptr this, TypeSc sc) {
	#if Type_SAFE
		ASSERT(!((usize)this & TypeSc_mask));
	#endif

	return (TypePtr)((usize)this | sc);
}

TypePtr TypePtr_fromenum(usize this) {
	#if Type_SAFE
		ASSERT(this <= TypePtr_maxenum);
	#endif

	return (TypePtr)(this << TypeSc_width);
}

typedef u64 TypeHash;

typedef struct {
	#if Printable_PTRTAG
		Printable (*repr)(TypePtr this);
	#else
		void (*repr)(TypePtr this, Printable *out_printable);
	#endif
	AlcRes (*copy)(TypePtr this, IARG(Alc, alc), Type *out_copy);
	AlcRes (*destroy)(TypePtr this, IARG(Alc, alc));
	TypeHash (*hash)(TypePtr this, TypeHash base);
} IType;

#if Type_PTRTAG
	struct Type {
		union {
			Ptr value;
			usize raw_value;
		};
	};

	enum {
		IType_NullType_ID,
		IType_SetType_ID,
		IType_UnionType_ID,
		IType_reserved
	};

	typedef utag TypeId;

	IFACEDB(IType, TypeId, 32, IType_reserved);

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
	// TODO implement
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
	}

	UNREACHABLE(Type_SAFE);
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
	}

	UNREACHABLE(Type_SAFE);
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
	}

	UNREACHABLE(Type_SAFE);
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
	}

	UNREACHABLE(Type_SAFE);
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
	}

	UNREACHABLE(Type_SAFE);
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
	}

	UNREACHABLE(Type_SAFE);
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
	}

	UNREACHABLE(Type_SAFE);
}

Printable Type_repr(Type this) {
	#if Printable_PTRTAG
		return Type_iface(this)->repr(Type_this(this));
	#else
		Printable result;
		Type_iface(this)->repr(Type_this(this), &result);
		return result;
	#endif
}

TypeHash Type_hash(Type this, TypeHash base) {
	return Type_iface(this)->hash(Type_this(this), base);
}
