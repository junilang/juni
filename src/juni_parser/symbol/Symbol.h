#ifndef Symbol_PTRTAG
	#define Symbol_PTRTAG PTRTAG
#endif

typedef enum : u8 {
	SymbolClass_NULL,
	SymbolClass_WHITESPACE,
	SymbolClass_TYPE,
	SymbolClass_TOKEN,
	SymbolClass_FUNCTION
} SymbolClass;

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

	const SymbolHeader ZZSymbol_NULL = { .metadata = SymbolClass_NULL, .next = Symbol_NULL };


#endif

Symbol Symbol_next(Symbol this) {
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
