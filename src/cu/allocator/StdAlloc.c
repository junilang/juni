Ptr StdAlloc_new(Ptr this, usize size) {
	Ptr mem = malloc(size);

	#if Allocator_SAFE
		if (!mem) PANIC("StdAlloc_new: malloc failed");
	#endif

	return mem;
}

Ptr StdAlloc_resize(Ptr this, Ptr buf, usize size) {
	Ptr mem = realloc(buf, size);

	#if Allocator_SAFE
		if (!mem) PANIC("StdAlloc_new: malloc failed");
	#endif

	return mem;
}

void StdAlloc_delete(Ptr this, Ptr buf) {
	free(buf);
}

IAllocator_GENERATE(StdAlloc)
IAllocator_REGISTER_KNOWN(StdAlloc)

#define StdAlloc StdAlloc_upcast(nullptr)
