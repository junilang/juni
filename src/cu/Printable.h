#ifndef Printable_PTRTAG
	#define Printable_PTRTAG PTRTAG
#endif

typedef struct {
	void (*print)(Ptr this, IARG(OutStream, os));
} IPrintable;

#if Printable_PTRTAG
	typedef struct {
		Ptr value;
	} Printable;

	enum {
		IPrintable_KNOWN
	};

	INTERFACE_REGISTRY(IPrintable, utag, 64)

	Ptr Printable_this(Printable this) { return ptrstrip(this.value); }
	const IPrintable *Printable_iface(Printable this) {
		return &IPrintable__registry[ptrread(this.value)];
	}

#else
	typedef struct {
		Ptr this;
		const IPrintable *iface;
	} Printable;

	Ptr Printable_this(Printable this) { return this.this; }
	const IPrintable *Printable_iface(Printable this) { return this.iface; }

#endif

void Printable_print(Printable this, OutStream os) {
	Printable_iface(this)->print(Printable_this(this), IPASS(OutStream, os));
}

#include "Printable_meta.h"
