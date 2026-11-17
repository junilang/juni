
#if LIBSILVER_HAS_CRC64
	typedef u64 TypeHash;
	TypeHash constexpr TypeHash_base = crc64_base;

	#define Type_HASHBUF(base, buf, buf_size) crc64(base, buf, buf_size)
	#define Type_HASH(base, value) HASH(crc64, base, value)
#else
	typedef u64 TypeHash;
	TypeHash constexpr TypeHash_base = fnv1a_base;

	#define Type_HASHBUF(base, buf, buf_size) fnv1a(base, buf, buf_size)
	#define Type_HASH(base, value) HASH(fnv1a, base, value)

#endif
