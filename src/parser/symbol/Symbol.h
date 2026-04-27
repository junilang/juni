#ifndef Symbol_PTRTAG
	#define Symbol_PTRTAG PTRTAG
#endif

#define XS \
	X(NULL) \
	X(WHITESPACE) \
	X(TYPE) \
	X(TOKEN) \
	X(FUNCTION)

typedef enum : u8 {
	#define X(N) SymbolClass_##N,
		XS
	#undef X
} SymbolClass;

String SymbolClass_Repr[] = {
	#define X(N) [SymbolClass_##N] = STRING(#N),
		XS
	#undef X
};

typedef STRUCTDECL(SymbolHeader);

typedef struct {
	Ptr value;
} Symbol;

#if Symbol_PTRTAG
	typedef utag SymbolMetadata;

	struct SymbolHeader {
		Symbol next;
	};

	SymbolMetadata Symbol_metadata(Symbol this) {
		return ptrread(this.value);
	}

	Ptr Symbol_this(Symbol this) {
		return ptrstrip(this.value);
	}

	Symbol Symbol_init(SymbolHeader *header, SymbolClass class) {
		return (Symbol){ptrtag(header, (SymbolMetadata)class)};
	}

	#define Symbol_NULL LITERAL(Symbol, ptrtag(nullptr, SymbolClass_NULL))
	//#define Symbol_EOF LITERAL(Symbol, ptrtag(nullptr, SymbolClass_EOF))

#else
	typedef u64 SymbolMetadata;

	struct SymbolHeader {
		SymbolMetadata metadata;
		Symbol next;
	};

	SymbolMetadata Symbol_metadata(Symbol this) {
		return ((SymbolHeader*)this.value)->metadata;
	}

	Ptr Symbol_this(Symbol this) {
		return this.value;
	}

	Symbol Symbol_init(SymbolHeader *header, SymbolClass class) {
		header->metadata = class;
		return (Symbol){header};
	}

	#define Symbol_NULL LITERAL(Symbol, (Ptr)&ZZSymbol_NULL)
	//#define Symbol_EOF LITERAL(Symbol, (Ptr)&ZZSymbol_EOF)

	const SymbolHeader ZZSymbol_NULL = {
		.metadata = SymbolClass_NULL, .next = Symbol_NULL
	};

	/*
	const SymbolHeader ZZSymbol_EOF = {
		.metadata = SymbolClass_EOF, .next = Symbol_NULL
	};
	*/


#endif

Symbol Symbol_next(Symbol this) {
	#if Symbol_PTRTAG && BUILD_SAFE
		if (!Symbol_this(this)) {
			PANIC("attempt to dereference sentinel symbol");
		}
	#endif

	return ((SymbolHeader*)Symbol_this(this))->next;
}

SymbolClass Symbol_class(Symbol this) {
	return (SymbolClass)Symbol_metadata(this);
}

bool Symbol_isnull(Symbol this) {
	#if BUILD_SAFE
		return Symbol_class(this) == SymbolClass_NULL;
	#else
		return this.value == Symbol_NULL.value;
	#endif
}
