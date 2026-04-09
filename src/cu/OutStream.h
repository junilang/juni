#if PTRTAG && !defined(OutStream_PTRTAG)
	#define OutStream_PTRTAG true
#else
	#define OutStream_PTRTAG false
#endif

typedef struct {
	void (*write)(Ptr this, const ubyte *buffer, usize buffer_size);
	void (*flush)(Ptr this);
} IOutStream;


#if OutStream_PTRTAG
	typedef struct {
		Ptr value;
	} OutStream;

	enum {
		IOutStream_FileOutStream_ID,
		IOutStream_KNOWN
	};

	INTERFACE_REGISTRY(IOutStream, utag, 4)

	Ptr OutStream_this(OutStream this) { return ptrstrip(this.value); }
	const IOutStream *OutStream_iface(OutStream this) {
		return &IOutStream__registry[ptrread(this.value)];
	}

#else

	typedef struct {
		const IOutStream *iface;
		Ptr this;
	} OutStream;

	Ptr OutStream_this(OutStream this) { return this.this; }
	const IOutStream *OutStream_iface(OutStream this) { return this.iface; }

#endif

void OutStream_write(OutStream this, const ubyte *buffer, usize buffer_size) {
	OutStream_iface(this)->write(OutStream_this(this), buffer, buffer_size);
}

void OutStream_flush(OutStream this) {
	OutStream_iface(this)->flush(OutStream_this(this));
}

#include "OutStream_meta.h"
