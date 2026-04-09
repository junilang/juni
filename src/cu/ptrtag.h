#if defined(__x86_64__)
	#ifndef PTRTAG
		#define PTRTAG true
	#endif
	#define PTRTAG_BITS 16
	typedef uint16_t utag;
	#define PTRTAG_MAX UINT16_MAX
#else
	#define PTRTAG false
#endif

#if PTRTAG
	#define PTRTAG_MASK ((1ULL << PTRTAG_BITS) - 1ULL)

	// TODO: add debug build checks for already tagged pointers
	Ptr ptrtag(const Ptr ptr, utag tag) {
		return (Ptr)(
			((usize)ptr << PTRTAG_BITS) | (usize)tag
		);
	}

	Ptr ptrstrip(const Ptr ptr) {
		return (Ptr)((usize)ptr >> PTRTAG_BITS);
	}

	utag ptrread(const Ptr ptr) {
		return (utag)((usize)ptr & PTRTAG_MASK);
	}

#endif
