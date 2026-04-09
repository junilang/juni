Ptr StdAlloc_new(Ptr this, usize size) {
	return malloc(size);
}

Ptr StdAlloc_resize(Ptr this, Ptr buf, usize size) {
	return realloc(buf, size);
}

void StdAlloc_delete(Ptr this, Ptr buf) {
	free(buf);
}

IAllocator_GENERATE(StdAlloc)
IAllocator_REGISTER_KNOWN(StdAlloc)

#define StdAlloc StdAlloc_upcast(nullptr)
