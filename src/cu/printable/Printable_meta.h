#define IPrintable_GENERATE_INTERFACE(N) \
	const IPrintable IPrintable_##N = { \
		.print = &IPrintable_##N_print \
	};

#define IPrintable_GENERATE_METHODS(N, E) \
	extern void IPrintable_##N##_print(Ptr this, IARG(OutStream, os)) { \
		N##_print((E)(usize)this, IWRAP(OutStream, os)); \
	}

#if Printable_PTRTAG
	#define IPrintable_GENERATE_UPCAST(N, E) \
		Printable N##_repr(E this) { \
			return (Printable){ptrtag((Ptr)(usize)this, IPrintable_##N##_ID)}; \
		}

	#define IPrintable_REGISTER(N) INTERFACE_REGISTER(IPrintable, N)
	#define IPrintable_REGISTER_KNOWN(N) INTERFACE_REGISTER(IPrintable, N)

#else
	#define IPrintable_GENERATE_UPCAST(N, E) \
		Printable N##_repr(E this) { \
			return (Printable){.this=(Ptr)(usize)this,.iface=&IPrintable_##N}; \
		}

	#define IPrintable_REGISTER(N)
	#define IPrintable_REGISTER_KNOWN(N)

#endif

#define IPrintable_GENERATE(N, E) \
	IPrintable_GENERATE_METHODS(N, E) \
	IPrintable_GENERATE_INTERFACE(N) \
	IPrintable_GENERATE_UPCAST(N, E)
