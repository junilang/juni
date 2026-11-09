#ifndef Type_PTRTAG
	#define Type_PTRTAG PTRTAG
#endif

typedef STRUCTDECL(Type);

enum {
	FLAG_DEF(Type_Clone)
};



typedef struct {
	#if Alc_PTRTAG
		AlcRes (*copy)(Ptr this, Alc alc, u8 flags, Type *out_type);
	#else
		AlcRes (*copy)(Ptr this, IARG(Alc, alc), )
	#endif
	AlcRes (*destroy)(Ptr this, IARG(Alc, alc));
} IType;

typedef struct {
	Ptr a; Ptr b;
} Object;
