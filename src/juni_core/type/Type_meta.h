#define IType_GENERATE_INTERFACE(N) \
	const IType IType_##N = { \
		.repr = &IType_##N##_repr, \
		.copy = &IType_##N##_copy, \
		.destroy = &IType_##N##_destroy, \
		.hash = &IType_##N##_hash, \
		.equal = &IType_##N##_equal, \
	};

#define IType_GENERATE_INTERFACE_ENUM(N) \
	const IType IType_##N = { \
		.repr = &IType_##N##_repr \
	};

#define IType_GENERATE_METHODS_ENUM(N, E)	\
	extern Printable IType_##N##_repr(Ptr this) { \
		return N##_repr((E)(usize)this); \
	}


#define IType_GENERATE_METHODS(N, E) \
	IType_GENERATE_METHODS_ENUM(N, E) \
	extern Type IType_##N##_copy(Ptr this, TypeCopyFlag flags, IARG(Allocator, alc)) { \
		return N##_copy((E)(usize)this, flags, IWRAP(Allocator, alc)); \
	} \
	extern void IType_##N##_destroy(Ptr this, IARG(Allocator, alc)) { \
		N##_destroy((E)(usize)this, IWRAP(Allocator, alc)); \
	} \
	extern uhash IType_##N##_hash(Ptr this, uhash base) { \
		return N##_hash((E)(usize)this, base); \
	} \
	extern bool IType_##N##_equal(Ptr this, Ptr other) { \
		return N##_equal((E)(usize)this, (E)(usize)other); \
	}

#define IType_DECLARE(N, E) \
	Type N##_upcast(E this); \
	bool N##_is(Type this);

#if Type_PTRTAG
	#define IType_GETID(N) (IType_##N##_ID)

	#define IType_GENERATE_UPCAST(N, E) \
		Type N##_upcast(E this) { \
			return (Type){ptrtag((Ptr)(usize)this, (utag)(IType_##N##_ID << TypeSc__BITS))}; \
		}

	#define IType_GENERATE_UPCAST_ENUM(N, E) \
		Type N##_upcast(E this) { \
			return (Type){ptrtag((Ptr)(usize)this, (utag)((IType_##N##_ID << TypeSc__BITS) | TypeSc_ENUM))}; \
		}

	#define IType_REGISTER(N) INTERFACE_REGISTER(IType, N)
	#define IType_REGISTER_KNOWN(N) INTERFACE_REGISTER_KNOWN(IType, N)

#else
	#define IType_GETID(N) ((TypeId)&IType_##N)

	#define IType_GENERATE_UPCAST(N, E) \
		Type N##_upcast(E this) { \
			return (Type){.this=(Ptr)(usize)this,.iface={(Ptr)&IType_##N}}; \
		}

	#define IType_GENERATE_UPCAST_ENUM(N, E) \
		Type N##_upcast(E this) { \
			return (Type){ \
				.this=(Ptr)(usize)this, \
				.iface={(Ptr)((usize)&IType_##N | TypeSc_ENUM)} \
			}; \
		}

	#define IType_REGISTER(N)
	#define IType_REGISTER_KNOWN(N)

#endif

#define IType_GENERATE_IS(N) \
	bool N##_is(Type this) { \
		return Type_id(this) == IType_GETID(N); \
	}

#define IType_GENERATE_(N, E, REGISTER) \
	IType_GENERATE_METHODS(N, E) \
	IType_GENERATE_INTERFACE(N) \
	REGISTER(N) \
	IType_GENERATE_UPCAST(N, E) \
	IType_GENERATE_IS(N)

#define IType_GENERATE_KNOWN(N, E) \
	IType_GENERATE_(N, E, IType_REGISTER_KNOWN)

#define IType_GENERATE(N, E) \
	IType_GENERATE_(N, E, IType_REGISTER)

#define IType_GENERATE_ENUM_(N, E, REGISTER) \
	IType_GENERATE_METHODS_ENUM(N, E) \
	IType_GENERATE_INTERFACE_ENUM(N) \
	REGISTER(N) \
	IType_GENERATE_UPCAST_ENUM(N, E) \
	IType_GENERATE_IS(N)

#define IType_GENERATE_KNOWN_ENUM(N, E) \
	IType_GENERATE_ENUM_(N, E, IType_REGISTER_KNOWN)

#define IType_GENERATE_ENUM(N, E) \
	IType_GENERATE_ENUM_(N, E, IType_REGISTER)
