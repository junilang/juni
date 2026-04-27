Ptr Malloc_new(Ptr this, usize size) {
	#if Allocator_SAFE
		if (!size) PANIC("Malloc_new: size = 0");
	#endif

	Ptr mem = malloc(size);

	#if Allocator_SAFE
		if (!mem) PANIC("Malloc_new: malloc failed");
	#endif

	return mem;
}

Ptr Malloc_resize(Ptr this, Ptr buf, usize size) {
	#if Allocator_SAFE
		if (!size) PANIC("Malloc_new: size = 0");
	#endif

	Ptr mem = realloc(buf, size);

	#if Allocator_SAFE
		if (!mem) PANIC("Malloc_new: realloc failed");
	#endif

	return mem;
}

void Malloc_delete(Ptr this, Ptr buf) {
	#if Allocator_SAFE
		if (!buf) PANIC("buf = null");
	#endif

	free(buf);
}

IAllocator_GENERATE_KNOWN(Malloc)

#define Malloc Malloc_upcast(nullptr)
