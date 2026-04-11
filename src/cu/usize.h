#define SIZE_ALIGN(S, A)  ((S + (A - (S % A) * !!(S % A))))

int usize_sadd(usize a, usize b, usize *result) {
	if (a > (USIZE_MAX - b)) {
		return 1;
	}

	*result = a + b;
	return 0;
}

usize usize_padd(usize a, usize b) {
	usize result;
	if (usize_sadd(a, b, &result))
		PANIC("usize_padd: overflow");

	return result;
}

int usize_smul(usize a, usize b, usize *result) {
	if ((b != 0) && (a > (USIZE_MAX / b))) {
		return 1;
	}
	*result = a * b;
	return 0;
}

usize usize_pmul(usize a, usize b) {
	usize result;
	if (usize_smul(a, b, &result))
		PANIC("usize_pmul overflow");

	return result;
}

int usize_sflx(usize base, usize el, usize n, usize *result) {
	if (usize_smul(el, n, &el)) return 1;
	return usize_sadd(base, el, result);
}

usize usize_pflx(usize base, usize el, usize n) {
	usize result;
	if (usize_sflx(base, el, n, &result))
		PANIC("usize_pflx: overflow");

	return result;
}

int usize_salign(usize size, usize alignment, usize *result) {
	usize d = size % alignment;
	if (!d) {
		*result = size;
		return 0;
	}
	return usize_sadd(size, alignment, result);
}

usize usize_palign(usize size, usize alignment) {
	usize result;
	if (usize_salign(size, alignment, &result))
		PANIC("usize_palign: overflow");

	return result;
}

// downcasting

int usize_scast8(usize size, u8 *result) {
	if (size > UINT8_MAX) return 1;
	*result = (u8)size;
	return 0;
}

u8 usize_pcast8(usize size) {
	if (size > UINT8_MAX)
		PANIC("usize_pcast8: overflow");
	return (u8)size;
}

int usize_scast16(usize size, u16 *result) {
	if (size > UINT16_MAX) return 1;
	*result = (u16)size;
	return 0;
}

u16 usize_pcast16(usize size) {
	if (size > UINT16_MAX)
		PANIC("usize_pcast16: overflow");
	return (u16)size;
}

int usize_scast32(usize size, u32 *result) {
	if (size > UINT32_MAX) return 1;
	*result = (u32)size;
	return 0;
}

u32 usize_pcast32(usize size) {
	if (size > UINT32_MAX)
		PANIC("usize_pcast32: overflow");
	return (u32)size;
}

int usize_scast64(usize size, u64 *result) {
	if (size > UINT64_MAX) return 1;
	*result = (u64)size;
	return 0;
}

u64 usize_pcast64(usize size) {
	if (size > UINT64_MAX)
		PANIC("usize_pcast64: overflow");
	return (u64)size;
}
