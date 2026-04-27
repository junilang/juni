#ifndef Allocator_PTRTAG
	#define Allocator_PTRTAG PTRTAG
#endif

#ifndef Allocator_SAFE
	#define Allocator_SAFE BUILD_SAFE
#endif

typedef struct {
	Ptr (*new)(Ptr this, usize size);
	Ptr (*resize)(Ptr this, Ptr buf, usize size);
	void (*delete)(Ptr this, Ptr buf);
} IAllocator;

#if Allocator_PTRTAG
	typedef struct {
		Ptr value;
	} Allocator;

	enum {
		IAllocator_Malloc_ID,
		IAllocator_KNOWN
	};

	INTERFACE_REGISTRY(IAllocator, utag, 8)

	Ptr Allocator_this(Allocator this) { return ptrstrip(this.value); }
	const IAllocator *Allocator_iface(Allocator this) {
		return &IAllocator__registry[ptrread(this.value)];
	}

	bool Allocator_equal(Allocator this, Allocator other) {
		return this.value == other.value;
	}

#else
	typedef struct {
		Ptr this;
		const IAllocator *iface;
	} Allocator;

	Ptr Allocator_this(Allocator this) { return this.this; }
	const IAllocator *Allocator_iface(Allocator this) { return this.iface; }

	bool Allocator_equal(Allocator this, Allocator other) {
		return this.this == other.this && this.iface == other.iface;
	}

#endif

Ptr Allocator_new(Allocator this, usize size) {
	return Allocator_iface(this)->new(Allocator_this(this), size);
}

Ptr Allocator_znew(Allocator this, usize size) {
	Ptr buf = Allocator_new(this, size);
	memset(buf, 0, size);
	return buf;
}

Ptr Allocator_resize(Allocator this, Ptr buf, usize size) {
	return Allocator_iface(this)->resize(Allocator_this(this), buf, size);
}

void Allocator_delete(Allocator this, Ptr buf) {
	return Allocator_iface(this)->delete(Allocator_this(this), buf);
}

#include "Allocator_meta.h"
