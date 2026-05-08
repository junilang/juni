#define XS \
	X(NULL) \
	X(ANY)

	typedef enum {
		#define X(N) ParserValueClass_##N,
			XS
		#undef X
	} ParserValueClass;
	
	typedef enum {
		ParserValueFlag_BIT_
	} ParserValueFlag;

	const String ParserValueClass_Repr[] = {
		#define X(N) [ParserValueClass_##N] = STRING(#N),
			XS
		#undef X
	};

	typedef u16 ParserValueMetadata;
	
	
#undef XS
