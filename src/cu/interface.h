#define INTERFACE_REGISTRY(I, Id, Slots) \
	typedef Id I##__registry_Id; \
	I I##__registry[I##_KNOWN + Slots] = {0}; \
	usize I##__registry_index = I##_KNOWN; \
	void IOutStream__register_known(Id id, const I *iface) { \
		if (id >= I##_KNOWN) \
			PANIC(#I"__register_known: id > known"); \
		I##__registry[(usize)id] = *iface; \
	} \
	Id IOutStream__register(const I *iface) { \
		Id id = (Id)I##__registry_index; \
		if (id >= (I##_KNOWN + Slots)) { \
			PANIC(#I"__register: id overflow") \
		} \
		I##__registry_index++; \
		I##__registry[id] = *iface; \
		return id; \
	}

#define INTERFACE_REGISTER(I, N) \
	I##_registry_Id I##_##N##_ID; \
	void __attribute__((constructor(150))) I##_##N##__register() { \
		I##_##N##__id = I##__register(&I##_##N); \
	}

#define INTERFACE_REGISTER_KNOWN(I, N) \
	void __attribute__((constructor(140))) I##_##N##__register() { \
		I##__register_known(I##_##N##_ID, &I##_##N); \
	}
