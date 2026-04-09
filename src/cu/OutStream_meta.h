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
		OutStream N##_upcast(void *this) { \
			return (OutStream){ptrtag(this, IOutStream_##N##_ID)}; \
		}

#else
	#define IOutStream_GENERATE_UPCAST(N) \
		OutStream N##_upcast(void *this) { \
			return (OutStream){.this=this,.iface=&IOutStream_##N}; \
		}
#endif

#define IOutStream_GENERATE(N) \
	IOutStream_GENERATE_METHODS(N) \
	IOutStream_GENERATE_INTERFACE(N) \
	IOutStream_GENERATE_REST(N) \
	INTERFACE_REGISTER(IOutStream, N)

#define IOutStream_GENERATE_KNOWN(N) \
	IOutStream_GENERATE_METHODS(N) \
	IOutStream_GENERATE_INTERFACE(N) \
	IOutStream_GENERATE_UPCAST(N) \
	INTERFACE_REGISTER_KNOWN(IOutStream, N)
