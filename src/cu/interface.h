#define INTERFACE_REGISTRY(I, Id, Slots) \
	typedef Id I##__registry_Id; \
	I I##__registry[I##_KNOWN + Slots] = {0}; \
	usize I##__registry_index = I##_KNOWN; \
	void I##__register_known(Id id, const I *iface) { \
		if (id >= I##_KNOWN) \
			PANIC(#I"__register_known: id > known"); \
		I##__registry[(usize)id] = *iface; \
	} \
	Id I##_register(const I *iface) { \
		Id id = (Id)I##__registry_index; \
		if (id >= (I##_KNOWN + Slots)) { \
			PANIC(#I"__register: id overflow") \
		} \
		I##__registry_index++; \
		I##__registry[id] = *iface; \
		return id; \
	}

#define INTERFACE_REGISTER(I, N) \
	I##_registry_Id I##_##N##_ID; \
	void __attribute__((constructor(150))) I##_##N##__ctor() { \
		I##_##N##__ID= I##__register(&I##_##N); \
	}

#define INTERFACE_REGISTER_KNOWN(I, N) \
	void __attribute__((constructor(140))) I##_##N##__ctor() { \
		I##__register_known(I##_##N##_ID, &I##_##N); \
	}

// ON WINDOWS: pass interfaces as two pointers instead of 16byte struct
// 	on native abi boundaries (function pointers) because the windows abi
// 	is unable to pass split structs across multiple registers, and passes
// 	a pointer instead

#ifndef IFACESPLIT
	#ifndef __unix__ // windows
		#define IFACESPLIT true
	#else
		#define IFACESPLIT false
	#endif
#endif

#if IFACESPLIT

	#define IMACRO__(M, B, C, N) M##_##B(C, N)
	#define IMACRO_(M, B, C, N) IMACRO__(M, B, C, N)
	#define IMACRO(M, C, N) IMACRO_(M, C##_PTRTAG, C, N)

	#define IARG_true(C, N) C N
	#define IARG_false(C, N) typeof(((C*)0)->this) N##_this, typeof(((C*)0)->iface) N##_iface
	#define IARG(C, N) IMACRO(IARG, C, N)

	#define IFWD_true(C, N) N
	#define IFWD_false(C, N) N##_this, N##_iface
	#define IFWD(C, N) IMACRO(IFWD, C, N)

	#define IPASS_true(C, N) N
	#define IPASS_false(C, N) N.this, N.iface
	#define IPASS(C, N) IMACRO(IPASS, C, N)

	#define IWRAP_true(C, N) N
	#define IWRAP_false(C, N) (C){.this=N##_this,.iface=N##_iface}
	#define IWRAP(C, N) IMACRO(IWRAP, C, N)

#else

	#define IARG(C, N) C N
	#define IFWD(C, N) N
	#define IPASS(C, N) N
	#define IWRAP(C, N) N

#endif
