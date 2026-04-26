#define IOutStream_GENERATE_INTERFACE(N) \
	const IOutStream IOutStream_##N = { \
		.write = &IOutStream_##N##_write, \
		.flush = &IOutStream_##N##_flush \
	};

#define IOutStream_GENERATE_METHODS(N) \
	extern void IOutStream_##N##_write(Ptr this, const ubyte *buffer, usize buffer_size) { \
		N##_write(this, buffer, buffer_size); \
	} \
	extern void IOutStream_##N##_flush(Ptr this) { \
		N##_flush(this); \
	}

#if OutStream_PTRTAG
	#define IOutStream_GENERATE_UPCAST(N) \
		OutStream N##_upcast(Ptr this) { \
			return (OutStream){ptrtag(this, IOutStream_##N##_ID)}; \
		}

	#define IOutStream_REGISTER(N) INTERFACE_REGISTER(IOutStream, N)
	#define IOutStream_REGISTER_KNOWN(N) INTERFACE_REGISTER_KNOWN(IOutStream, N)

#else
	#define IOutStream_GENERATE_UPCAST(N) \
		OutStream N##_upcast(Ptr this) { \
			return (OutStream){.this=this,.iface=&IOutStream_##N}; \
		}

	#define IOutStream_REGISTER(N)
	#define IOutStream_REGISTER_KNOWN(N)

#endif

#define IOutStream_GENERATE_(N, REGISTER) \
	IOutStream_GENERATE_METHODS(N) \
	IOutStream_GENERATE_INTERFACE(N) \
	IOutStream_GENERATE_UPCAST(N) \
	REGISTER(N)

#define IOutStream_GENERATE(N) \
	IOutStream_GENERATE_(N, IOutStream_REGISTER)

#define IOutStream_GENERATE_KNOWN(N) \
	IOutStream_GENERATE_(N, IOutStream_REGISTER_KNOWN)
