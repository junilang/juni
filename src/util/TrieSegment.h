#define XALLOCATE(dst, size_) { \
	AlcReq req = { \
		.intent = AlcIntent_New, \
		.size = (size_), \
		.align = Trie_align \
	}; \
	auto ptr = Alc_invoke(alc, &req, nullptr, nullptr); \
	auto res = AlcPtr_get(ptr); \
	if (res) return Trie_seterr(res); \
	dst = (Ptr)ptr; \
}

Trie TrieSegment_set(
	Trie vthis, Alc alc,
	const u8 *segment, TrieSize segment_size,
	Ptr value
) {
	const TrieFlag isoccupied = Trie_flags(vthis) & FLAG(TrieFlag_Occupied);
	const TrieFlag isconst = Trie_flags(vthis) & FLAG(TrieFlag_Const);
	TrieSegment *const this = Trie_data(vthis);

	if (segment_size == 0) {
		if (value == this->value)
			return Trie_upcast(this, isoccupied | isconst);

		TrieSegment *data;
		if (isconst) {
			const TrieSize this_size = this->size;

			XALLOCATE(data, TrieSegment_allocsize(this_size));
			data->next = Trie_const(this->next);
			data->size = this_size;
			memcpy(data->bytes, this->bytes, this_size);

		} else {
			data = this;
		}

		data->value = value;
		return Trie_upcast(data, FLAG(TrieFlag_Occupied));
	}

	const TrieSize this_size = this->size;

	if (Trie_isnull(this->next)) {
		// validate this is a stub
		#if Trie_SAFE
			ASSERT(this->size != 0 && isoccupied);
		#endif

		TrieSegment *data;
		if (isconst) {
			XALLOCATE(data, TrieSegment_allocsize(segment_size));
			data->value = this->value;

		} else {
			AlcReq req = {
				.intent = AlcIntent_Resize,
				.size = TrieSegment_allocsize(segment_size),
				.align = Trie_align,
			};

			auto ptr = Alc_invoke(alc, &req, nullptr, this);
			auto res = AlcPtr_get(ptr);
			if (res) return Trie_seterr(res);

			data = (Ptr)ptr;
		}

		data->size = segment_size;
		memcpy(data->bytes, segment, segment_size);

		Trie next = Trie_createstub(alc, value);
		if (Trie_iserr(next)) return next;

		data->next = next;

		return Trie_upcast(data, FLAG(TrieFlag_Occupied));
	}

	TrieSize idx = 0;
	if (segment_size < this_size) {
		for (; idx < segment_size; idx++) {
			if (this->bytes[idx] != segment[idx]) goto found_split;
		}

		// entirety of string fits into this segment, need to split
		// in the middle and create two segments - no branch

		TrieSegment *left;
		Trie right_next;

		if (isconst) {
			right_next = Trie_const(this->next);
			XALLOCATE(left, TrieSegment_allocsize(idx));
			left->value = this->value;
			memcpy(left->bytes, this->bytes, idx);
		} else {
			right_next = this->next;
			#if Trie_CONSERVATIVE
				AlcReq req = {
					.intent = AlcIntent_Resize,
					.size = TrieSegment_allocsize(idx),
					.align = Trie_align,
				};

				auto ptr = Alc_invoke(alc, &req, nullptr, this);
				auto res = AlcPtr_get(ptr);
				if (res) return Trie_seterr(res);

				left = (Ptr)ptr;
			#else
				left = this;
			#endif
		}

		left->size = idx;

		const TrieSize right_size = this_size - idx;
		TrieSegment *right; XALLOCATE(right, TrieSegment_allocsize(right_size));

		right->size = right_size;
		right->value = value;
		right->next = right_next;
		memcpy(right->bytes, &this->bytes[idx], right_size);

		left->next = Trie_upcast(right, FLAG(TrieFlag_Occupied));

		return Trie_upcast(left, isoccupied);

	} else {
		for (; idx < this_size; idx++) {
			if (this->bytes[idx] != segment[idx]) goto found_split;
		}

		// segment fits inside of new string, modify next node

		if (isconst) {

			Trie next = Trie_set_(Trie_const(this->next), alc,
				segment + idx, segment_size - idx, value
			);
			if (Trie_iserr(next)) return next;

			if (Trie_equal(next, this->next))
				return Trie_upcast(this, FLAG(TrieFlag_Const) | isoccupied);

			TrieSegment *data; XALLOCATE(data, TrieSegment_allocsize(this_size));
			data->next = next;
			data->size = this_size;
			data->value = this->value;
			memcpy(data->bytes, this->bytes, this_size);
			return Trie_upcast(data, isoccupied);
		} else {
			auto next = Trie_set_(this->next, alc,
				segment + idx, segment_size - idx, value
			);
			if (Trie_iserr(next)) return next;

			return Trie_upcast(this, isoccupied);
		}
	}

	found_split:;

	const u8 a = this->bytes[idx];
	const u8 b = segment[idx];

	if (idx == 0) {
		// if the first character didn't match, there is no need to create
		// a left side segment, and this segment can be reused for the right side

		TrieBranch *data; XALLOCATE(data, TrieBranch_allocsize(2));
		memset(data->map, 0, sizeof(data->map));
		data->value = this->value;
		data->map[a / 64] |= (u64)1 << (a % 64);
		data->map[b / 64] |= (u64)1 << (b % 64);

		const TrieSize rest_size = this_size - 1;
		TrieSegment *rest;

		if (isconst) {
			XALLOCATE(rest, TrieSegment_allocsize(rest_size));
			rest->size = rest_size;
			rest->next = Trie_const(this->next);
			memcpy(rest->bytes, this->bytes + 1, rest_size);
		} else {
			this->size = rest_size;
			memmove(this->bytes, this->bytes + 1, rest_size);

			#if Trie_CONSERVATIVE
				AlcReq req = {
					.intent = AlcIntent_Resize,
					.size = TrieSegment_allocsize(rest_size),
					.align = Trie_align
				};

				auto ptr = Alc_invoke(alc, &req, nullptr, this);
				auto res = AlcPtr_get(ptr);
				if (res) return Trie_seterr(res);

				rest = (Ptr)ptr;
			#else
				rest = this;
			#endif
		}

		Trie stub = Trie_create(alc, segment + 1, segment_size - 1, value);
		if (Trie_iserr(stub)) return stub;

		if (a < b) {
			data->next[0] = Trie_upcast(rest, 0);
			data->next[1] = stub;
		} else {
			data->next[0] = stub;
			data->next[1] = Trie_upcast(rest, 0);
		}

		return Trie_upcast(data, FLAG(TrieFlag_Branch) | isoccupied);

	} else {
		const TrieSize idx1 = idx + 1;
		const TrieSize rest_size = this_size - idx1;

		Trie rest = Trie_orflags(this->next, isconst);
		if (rest_size != 0) {
			TrieSegment *data; XALLOCATE(data, TrieSegment_allocsize(rest_size));
			data->size = rest_size;
			data->next = rest;
			memcpy(data->bytes, this->bytes + idx1, rest_size);
			rest = Trie_upcast(data, 0);
		}

		TrieSegment *left;
		if (isconst) {
			XALLOCATE(left, TrieSegment_allocsize(idx));
			left->size = idx;
			left->value = this->value;
			memcpy(left->bytes, this->bytes, idx);
		} else {
			this->size = idx;

			#if Trie_CONSERVATIVE
				AlcReq req = {
					.intent = AlcIntent_Resize,
					.size = TrieSegment_allocsize(idx),
					.align = Trie_align,
				};

				auto ptr = Alc_invoke(alc, &req, nullptr, this);
				auto res = AlcPtr_get(ptr);
				if (res) return Trie_seterr(res);

				left = (Ptr)ptr;
			#else
				left = this;
			#endif
		}

		TrieBranch *branch; XALLOCATE(branch, TrieBranch_allocsize(2));
		memset(branch->map, 0, sizeof(branch->map));
		branch->map[a / 64] |= (u64)1 << (a % 64);
		branch->map[b / 64] |= (u64)1 << (b % 64);

		Trie stub = Trie_create(alc, segment + idx1, segment_size - idx1, value);
		if (Trie_iserr(stub)) return stub;

		if (a < b) {
			branch->next[0] = rest;
			branch->next[1] = stub;
		} else {
			branch->next[0] = stub;
			branch->next[1] = rest;
		}

		left->next = Trie_upcast(branch, FLAG(TrieFlag_Branch));

		return Trie_upcast(left, isoccupied);
	}
}

Trie TrieSegment_unset(
	Trie vthis, Alc alc,
	const u8 *segment, TrieSize segment_size
) {
	const TrieFlag isoccupied = Trie_flags(vthis) & FLAG(TrieFlag_Occupied);
	const TrieFlag isconst = Trie_flags(vthis) & FLAG(TrieFlag_Const);
	TrieSegment *const this = Trie_data(vthis);

	if (segment_size == 0) {
		if (Trie_isnull(this->next)) {
			if (!isconst) {
				auto res = Alc_delete(alc, this);
				if (res) return Trie_seterr(res);
			}

			return Trie_null;
		} else {
			return Trie_upcast(this, isconst);
		}

	}

	const TrieSize this_size = this->size;

	if (
		Trie_isnull(this->next) ||
		(segment_size < this_size) ||
		(memcmp(segment, this->bytes, this_size) != 0)
	) {
		return Trie_upcast(this, isoccupied | isconst);
	}

	if (isconst) {
		Trie next = Trie_unset_(Trie_const(this->next), alc,
			segment + this_size, segment_size - this_size
		);
		if (Trie_iserr(next)) return next;

		if (Trie_equal(next, this->next))
			return Trie_upcast(this, FLAG(TrieFlag_Const) | isoccupied);

		if (Trie_isnull(next)) {
			if (isoccupied)
				return Trie_createstub(alc, this->value);
			else
				return Trie_null;

		} else {
			TrieSegment *data; XALLOCATE(data, TrieSegment_allocsize(this_size));
			data->value = this->value;
			data->size = this_size;
			data->next = next;
			memcpy(data->bytes, this->bytes, this_size);

			return Trie_upcast(data, isoccupied);
		}
	} else {
		Trie next = Trie_unset_(this->next, alc,
			segment + this_size, segment_size - this_size
		);
		if (Trie_iserr(next)) return next;

		if (Trie_equal(next, this->next))
			return Trie_upcast(this, isoccupied);

		if (Trie_isnull(next)) {
			if (isoccupied) {
				this->size = 0;
				#if Trie_CONSERVATIVE
					AlcReq req = {
						.intent = AlcIntent_Resize,
						.size = TrieSegment_allocsize(0),
						.align = Trie_align
					};

					auto ptr = Alc_invoke(alc, &req, nullptr, this);
					auto res = AlcPtr_get(ptr);
					if (res) return Trie_seterr(res);

					return Trie_upcast((Ptr)ptr, 0);
				#else
					return Trie_upcast(this, 0);
				#endif

			} else {
				auto res = Alc_delete(alc, this);
				if (res) return Trie_seterr(res);
				return Trie_null;
			}

		} else {
			this->next = next;
			return Trie_upcast(this, isoccupied);
		}

	}
}

AlcRes TrieSegment_destroy(Trie vthis, Alc alc) {
	if (Trie_isconst(vthis)) return AlcRes_Ok;
	TrieSegment *this = Trie_data(vthis);

	if (!Trie_isnull(this->next)) {
		auto res = Trie_destroy_(this->next, alc);
		if (res) return res;
	}

	return Alc_delete(alc, this);
}

OutStreamRes TrieSegment_print(Trie vthis, OutStream os, TrieSize depth) {
	OutStreamRes res;
	const bool isoccupied = Trie_isoccupied(vthis);
	TrieSegment *this = Trie_data(vthis);
	PRINT(&res, os, "> ");
	if (res) return res;

	if (isoccupied) {
		PRINT(&res, os, "(",(Ptr)this->value,") ");
		if (res) return res;
	}

	if (this->size) {
		PRINT(&res, os,
			"\"",
			((String){.data=this->bytes, .size=this->size}),
			"\""
		);
		if (res) return res;
	}

	const Trie next = this->next;
	if (Trie_isnull(this->next)) return OutStreamRes_Ok;

	return Trie_print_(next, os, depth);
}
