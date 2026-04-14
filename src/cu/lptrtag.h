#ifndef LPTRTAG_SAFE
	#define LPTRTAG_SAFE PTRTAG
#endif

Ptr lptrtag(const Ptr ptr, uint width, usize value) {
	#if LPTRTAG_SAFE
		const usize mask = (1ULL << width) - 1;

		if (value > mask)
			PANIC("lptrtag: tag overflow");

		if ((usize)ptr & mask)
			PANIC("lptrtag: pointer misaligned or already tagged");
	#endif

	return (Ptr)((usize)ptr | value);
}

Ptr lptrstrip(const Ptr ptr, uint width) {
	return (Ptr)((usize)ptr & (UINTPTR_MAX << width));
}

usize lptrread(const Ptr ptr, uint width) {
	return (usize)ptr & (((usize)1 << width) - 1);
}
