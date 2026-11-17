typedef enum : u8 {
	TypeSc_Enum,
	TypeSc_Const,
	TypeSc_Copy,
	TypeSc_Clone,
	TypeSc_Move,
} TypeSc;

constexpr auto TypeSc_width = 3;
constexpr usize TypeSc_mask = FIELD_MASK_LOWER(usize, TypeSc_width);
constexpr ualign Type_align = TypeSc_mask + 1;

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
