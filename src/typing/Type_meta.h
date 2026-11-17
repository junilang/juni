AlcRes IType_destroy_stub_(TypePtr this, IARG(Alc, alc)) {
	PANIC("should not be called");
}

AlcRes IType_copy_stub_(TypePtr this, IARG(Alc, alc), Type *out_copy) {
	PANIC("should not be called");
}

TypeHash IType_hash_stub_(TypePtr this, TypeHash base) {
	PANIC("should not be called");
}

bool IType_equal_stub_(TypePtr this, TypePtr other) {
	PANIC("should not be called");
}

#define IType_GENERATE_COMMON_METHODS(N) \
	void IType_##N##_asFmt(TypePtr this, Fmt *out_fmt) { \
		*out_fmt = N##_asTypeFmt(this); \
	} \
	bool IType_##N##_lmatch(TypePtr this, IARG(Type, other)) { \
		return N##_lmatch(this, IPASS(Type, other)); \
	} \
	bool IType_##N##_rmatch(TypePtr this, IARG(Type, other)) { \
		return N##_lmatch(this, IPASS(Type, other)); \
	}

#define IType_GENERATE_METHODS(N) \
	IType_GENERATE_COMMON_METHODS(N) \
	AlcRes IType_##N##_destroy(TypePtr this, IARG(Alc, alc)) { \
		return N##_destroy(this, IWRAP(Alc, alc)); \
	} \
	AlcRes IType_##N##_copy(TypePtr this, IARG(Alc, alc), Type *out_copy) { \
		return N##_copy(this, IWRAP(Alc, alc), out_copy); \
	} \
	TypeHash IType_##N##_hash(TypePtr this, TypeHash base) { \
		return N##_hash(this, base); \
	} \
	bool IType_##N##_equal(TypePtr this, TypePtr other) { \
		return N##_equal(this, other); \
	}

#define IType_GENERATE_ENUM_METHODS(N) \
	IType_GENERATE_COMMON_METHODS(N)

#define IType_GENERATE_INTERFACE(N) \
	const IType IType_##N = { \
		.asFmt = &IType_##N##_asFmt, \
		.copy = &IType_##N##_copy, \
		.destroy = &IType_##N##_destroy, \
		.hash = &IType_##N##_hash, \
		.equal = &IType_##N##_equal, \
		.lmatch = &IType_##N##_lmatch, \
		.rmatch = &IType_##N##_rmatch, \
	};

#define IType_GENERATE_ENUM_INTERFACE(N) \
	const IType IType_##N = { \
		.asFmt = &IType_##N##_asFmt, \
		.copy = &IType_copy_stub_, \
		.destroy = &IType_destroy_stub_, \
		.hash = &IType_hash_stub_, \
		.equal = &IType_equal_stub_, \
		.lmatch = &IType_##N##_lmatch, \
		.rmatch = &IType_##N##_rmatch, \
	};

#define IType_DECLARE(N, T) \
	Type N##_asType(T this);

#if Type_PTRTAG
	#define IType_REGISTER(N) IFACEDB_REGISTER(IType, N)
	#define IType_REGISTER_KNOWN(N) IFACEDB_SET(IType, N)

	#define IType_GENERATE_UPCAST(N, P) \
		Type N##_asType(P this) { \
			return (Type){.value=ptrtag(TypePtr_fromptr(this, TypeSc_Copy), IType_##N##_ID)}; \
		}

	#define IType_GENERATE_ENUM_UPCAST(N, E) \
		Type N##_asType(E this) { \
			return (Type){.value=ptrtag(TypePtr_fromenum(this), IType_##N##_ID)}; \
		}

#else
	#define IType_REGISTER(N)
	#define IType_REGISTER_KNOWN(N)

	#define IType_GENERATE_UPCAST(N, P) \
		Type N##_asType(P this) { \
			return (Type){.this=TypePtr_fromptr(this, TypeSc_Copy),.iface=&IType_##N}; \
		}

	#define IType_GENERATE_ENUM_UPCAST(N, E) \
		Type N##_asType(E this) { \
			return (Type){.value=TypePtr_fromenum(this),.iface=&IType_##N}; \
		}
#endif

#define IType_GENERATE_(N, P, REGISTER) \
	IType_GENERATE_METHODS(N) \
	IType_GENERATE_INTERFACE(N) \
	IType_GENERATE_UPCAST(N, P) \
	REGISTER(N)

#define IType_GENERATE_ENUM_(N, E, REGISTER) \
	IType_GENERATE_ENUM_METHODS(N) \
	IType_GENERATE_ENUM_INTERFACE(N) \
	IType_GENERATE_ENUM_UPCAST(N, E) \
	REGISTER(N)

#define IType_GENERATE(N, P) \
	IType_GENERATE_(N, P, IType_REGISTER)

#define IType_GENERATE_KNOWN(N, P) \
	IType_GENERATE_(N, P, IType_REGISTER_KNOWN)

#define IType_GENERATE_ENUM(N, E) \
	IType_GENERATE_ENUM_(N, E, IType_REGISTER)

#define IType_GENERATE_ENUM_KNOWN(N, E) \
	IType_GENERATE_ENUM_(N, E, IType_REGISTER_KNOWN)
