#if defined(__x86_64__)
	#ifndef PTRTAG
		#define PTRTAG true
	#endif
	#define PTRTAG_BITS 16
	#define PTRTAG_ADDR_BITS 48
	typedef u16 utag;
#else
	#define PTRTAG false
#endif

#ifndef PTRTAG_SAFE
	#define PTRTAG_SAFE BUILD_SAFE
#endif

#if PTRTAG
	#define PTRTAG_MAX ((1ULL << PTRTAG_BITS) - 1)

	#if PTRTAG_LAM
		#error "PTRTAG_LAM unimplemented"
	#endif

	#if PTRTAG_CANONICAL
		#error "PTRTAG_CANONICAL unimplemented"
	#endif

	#define PTRTAG_LSB_MASK ((1ULL << PTRTAG_BITS) - 1)
	#define PTRTAG_ADDR_MASK ((1ULL << PTRTAG_ADDR_BITS) - 1)
	#define PTRTAG_MSB_MASK (~PTRTAG_ADDR_MASK)

	Ptr ptrtag_msb(const Ptr ptr, utag tag) {
		#if PTRTAG_SAFE
			if ((usize)ptr & PTRTAG_MSB_MASK)
				PANIC("ptrtag_msb: pointer already tagged");

			GCC_DIAG_PUSH
				GCC_DIAG_IGNORE(WTYPELIMITS)
				if (tag > PTRTAG_MAX)
					PANIC("ptrtag_msb: tag overflow");
			GCC_DIAG_POP

		#endif

		return (Ptr)(
			(usize)ptr | ((usize)tag << PTRTAG_ADDR_BITS)
		);
	}

	Ptr ptrstrip_msb(const Ptr ptr) {
		return (Ptr)(
			(usize)ptr & PTRTAG_ADDR_MASK
		);
	}

	utag ptrread_msb(const Ptr ptr) {
		return (utag)((usize)ptr >> PTRTAG_ADDR_BITS);
	}

	Ptr ptrtag_lsb(const Ptr ptr, utag tag) {
		#if PTRTAG_SAFE
			if ((usize)ptr & (PTRTAG_MSB_MASK))
				PANIC("ptrtag_lsb: pointer already tagged");

			GCC_DIAG_PUSH
				GCC_DIAG_IGNORE(WTYPELIMITS)
				if (tag > PTRTAG_MAX)
					PANIC("ptrtag_msb: tag overflow");
			GCC_DIAG_POP
		#endif

		return (Ptr)(
			((usize)ptr << PTRTAG_BITS) | (usize)tag
		);
	}

	Ptr ptrstrip_lsb(const Ptr ptr) {
		return (Ptr)((usize)ptr >> PTRTAG_BITS);
	}

	utag ptrread_lsb(const Ptr ptr) {
		return (utag)((usize)ptr & PTRTAG_LSB_MASK);
	}

	#if PTRTAG_MODE_LSB
		#define PTRTAG_DEFAULT(N) N##_lsb
	#else
		#define PTRTAG_DEFAULT(N) N##_msb
	#endif

	Ptr ptrtag(const Ptr ptr, utag tag) { return PTRTAG_DEFAULT(ptrtag)(ptr, tag); }
	Ptr ptrstrip(const Ptr ptr) { return PTRTAG_DEFAULT(ptrstrip)(ptr); }
	utag ptrread(const Ptr ptr) { return PTRTAG_DEFAULT(ptrread)(ptr); }

	#undef PTRTAG_DEFAULT

#endif
