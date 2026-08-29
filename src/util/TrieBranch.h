uint TrieBranch_ZZsize(
	const TrieBranch *this
) {
	return (uint)(
		__builtin_popcountll(this->map[0]) +
		__builtin_popcountll(this->map[1]) +
		__builtin_popcountll(this->map[2]) +
		__builtin_popcountll(this->map[3])
	);
}

uint TrieBranch_ZZindex(
	const TrieBranch *this,
	u8 map_idx, u64 map_bit
) {
	switch (map_idx) {
		case 0:
			return (uint)(
				__builtin_popcountll(this->map[0] & (map_bit - 1))
			);
		case 1:
			return (uint)(
				__builtin_popcountll(this->map[0]) +
				__builtin_popcountll(this->map[1] & (map_bit - 1))
			);
		case 2:
			return (uint)(
				__builtin_popcountll(this->map[0]) +
				__builtin_popcountll(this->map[1]) +
				__builtin_popcountll(this->map[2] & (map_bit - 1))
			);
		case 3:
			return (uint)(
				__builtin_popcountll(this->map[0]) +
				__builtin_popcountll(this->map[1]) +
				__builtin_popcountll(this->map[2]) +
				__builtin_popcountll(this->map[3] & (map_bit - 1))
			);
	}

	UNREACHABLE;
}

u8 TrieBranch_ZZfirst(u64 map[4]) {
	if (map[0]) return (u8)__builtin_ctzll(map[0]) + (64 * 0);
	if (map[1]) return (u8)__builtin_ctzll(map[1]) + (64 * 1);
	if (map[2]) return (u8)__builtin_ctzll(map[2]) + (64 * 2);
	if (map[3]) return (u8)__builtin_ctzll(map[3]) + (64 * 3);
	UNREACHABLE;
}


// TODO potentially store branch size in pointer tag
Trie TrieBranch_ZZset(
	Trie vthis, Alc alc,
	const u8 *segment, TrieSize segment_size,
	Ptr value
) {
	const TrieFlag isoccupied = Trie_flags(vthis) & FLAG(TrieFlag_Occupied);
	const TrieFlag isconst = Trie_flags(vthis) & FLAG(TrieFlag_Const);
	TrieBranch *const this = Trie_data(vthis);

	if (segment_size == 0) {
		#if Trie_CONSERVATIVE
			if (value == this->value)
				return Trie_upcast(this, FLAG(TrieFlag_Branch) | isoccupied | isconst);
		#endif

		if (isconst) {
			const uint this_size = TrieBranch_ZZsize(this);

			AlcReq req = {
				.intent = AlcIntent_New,
				.size = TrieBranch_ZZallocsize(this_size),
				.align = _Alignof(TrieBranch)
			};

			auto ptr = Alc_invoke(alc, &req, nullptr, nullptr);
			auto res = AlcPtr_get(ptr);
			if (res) return Trie_seterr(res);

			auto data = (TrieBranch*)ptr;
			data->value = value;
			memcpy(data->map, this->map, sizeof(this->map));
			for (uint i = 0; i < this_size; i++) {
				data->next[i] = Trie_const(this->next[i]);
			}
			return Trie_upcast(data, FLAGS(TrieFlag, Branch, Occupied));
		} else {
			this->value = value;
			return Trie_upcast(this, FLAGS(TrieFlag, Branch, Occupied));
		}
	}

	const u8 chr = segment[0];
	const u8 map_idx = chr / 64;
	const u64 map_bit = (u64)1 << (chr % 64);

	if (this->map[map_idx] & map_bit) {
		const uint idx = TrieBranch_ZZindex(this, map_idx, map_bit);

		if (isconst) {
			#if Trie_CONSERVATIVE

				Trie next = Trie_ZZset(Trie_const(this->next[idx]), alc,
					segment + 1, segment_size - 1, value
				);

				if (Trie_ZZequal(next, this->next[idx]))
					return Trie_upcast(this, FLAGS(TrieFlag, Branch, Const) | isoccupied);

			#endif

			const uint this_size = TrieBranch_ZZsize(this);

			AlcReq req = {
				.intent = AlcIntent_New,
				.size = TrieBranch_ZZallocsize(this_size),
				.align = _Alignof(TrieBranch)
			};

			auto ptr = Alc_invoke(alc, &req, nullptr, nullptr);
			auto res = AlcPtr_get(ptr);
			if (res) return Trie_seterr(res);

			auto data = (TrieBranch*)ptr;
			data->value = this->value;
			memcpy(data->map, this->map, sizeof(this->map));

			uint i = 0;
			for (; i < idx; i++) {
				data->next[i] = Trie_const(this->next[i]);
			}

			#if !Trie_CONSERVATIVE
				Trie next = Trie_ZZset(Trie_const(this->next[idx]), alc
					segment + 1, segment_size - 1, value
				);
			#endif

			data->next[i] = next;
			i++;

			for (; i < this_size; i++) {
				data->next[i] = Trie_const(this->next[i]);
			}

			return Trie_upcast(data, FLAG(TrieFlag_Branch) | isoccupied);
		} else {
			this->next[idx] = Trie_ZZset(
				this->next[idx], alc,
				segment + 1, segment_size - 1,
				value
			);
			return Trie_upcast(this, FLAG(TrieFlag_Branch) | isoccupied);
		}

	} else if (isconst) {
		const uint this_size = TrieBranch_ZZsize(this);

		AlcReq req = {
			.intent = AlcIntent_New,
			.size = TrieBranch_ZZallocsize(this_size),
			.align = _Alignof(TrieBranch)
		};

		auto ptr = Alc_invoke(alc, &req, nullptr, nullptr);
		auto res = AlcPtr_get(ptr);
		if (res) return Trie_seterr(res);

		auto data = (TrieBranch*)ptr;
		data->value = this->value;
		memcpy(data->map, this->map, sizeof(this->map));
		data->map[map_idx] |= map_bit;

		const uint idx = TrieBranch_ZZindex(data, map_idx, map_bit);

		uint i = 0;
		for (; i < idx; i++) {
			data->next[i] = Trie_const(this->next[i]);
		}

		data->next[i] = Trie_create(alc, segment + 1, segment_size - 1, value);

		for (; i < this_size; i++) {
			data->next[i + 1] = Trie_const(this->next[i]);
		}

		return Trie_upcast(data, FLAG(TrieFlag_Branch) | isoccupied);

	} else {
		const uint this_size = TrieBranch_ZZsize(this);

		AlcReq req = {
			.intent = AlcIntent_Resize,
			.size = TrieBranch_ZZallocsize(this_size),
			.align = _Alignof(TrieBranch),
		};

		auto ptr = Alc_invoke(alc, &req, nullptr, this);
		auto res = AlcPtr_get(ptr);
		if (res) return Trie_seterr(res);

		auto data = (TrieBranch*)ptr;

		data->map[map_idx] |= map_bit;
		const uint idx = TrieBranch_ZZindex(data, map_idx, map_bit);

		memmove(&data->next[idx + 1], &data->next[idx], (this_size - idx) * sizeof(Trie));
		data->next[idx] = Trie_create(alc, segment + 1, segment_size - 1, value);

		return Trie_upcast(data, FLAG(TrieFlag_Branch) | isoccupied);
	}
}

Trie TrieBranch_ZZunset(
	Trie vthis, Alc alc,
	const u8 *segment, TrieSize segment_size
) {
	const TrieFlag isoccupied = Trie_ZZflags(vthis) & FLAG(TrieFlag_Occupied);
	const TrieFlag isconst = Trie_ZZflags(vthis) & FLAG(TrieFlag_Occupied);
	TrieBranch *const this = Trie_data(vthis);

	// if segment size is 0 we need to unset this trie node
	if (segment_size == 0) {
		if (isconst) { // if const, copy the branch struct
			const uint this_size = TrieBranch_ZZsize(this);

			auto ptr = Alc_invoke(alc, &(AlcReq) {
				.intent = AlcIntent_New,
				.size = TrieBranch_ZZallocsize(this_size),
				.align = TrieBranch_align
			}, nullptr, nullptr);

			auto res = AlcPtr_get(ptr);
			if (res) return Trie_seterr(res);

			auto data = (TrieBranch*)ptr;
			memcpy(data->map, this->map, sizeof(this->map));
			for (uint i = 0; i < this_size; i++) {
				data->next[i] = Trie_const(this->next[i]);
			}
			return Trie_ZZupcast(data, FLAG(TrieFlag_Branch));

		} else { // if not const, just remove the occupied flag
			return Trie_ZZupcast(this, FLAG(TrieFlag_Branch));
		}
	}

	const u8 chr = segment[0];
	const u8 map_idx = chr / 64;
	const u64 map_bit = (u64)1 << (chr % 64);

	if (!(this->map[map_idx] & map_bit)) {
		// if the byte is not in this branch, return with original flags
		// intentionally not using "vthis" to free a register / avoid memory load
		return Trie_ZZupcast(this, FLAG(TrieFlag_Branch) | isoccupied | isconst);
	}

	const uint idx = TrieBranch_ZZindex(this, map_idx, map_bit);
	uint this_size = TrieBranch_ZZsize(this);

	if (isconst) {
		auto old_next = this->next[idx];

		auto next = Trie_ZZunset(Trie_const(old_next), alc,
			segment + 1, segment_size - 1
		);

		if (Trie_iserr(next)) return next;

		#if Trie_CONSERVATIVE
			if (Trie_ZZequal(next, this->next[idx])) // no allocation was made
				return Trie_ZZupcast(this, FLAGS(TrieFlag, Branch, Const) | isoccupied);
		#endif

		if (Trie_isnull(next)) {
			this_size--;
			// implying the previous size was 1 (not allowed for branches)
			if (this_size == 0) UNREACHABLE;
			if (this_size == 1) {
				u64 map[4]; memcpy(map, this->map, sizeof(this->map));
				map[map_idx] &= ~map_bit;
				u8 chr = TrieBranch_ZZfirst(map);

				auto ptr = Alc_invoke(alc, &(AlcReq) {
					.intent = AlcIntent_New,
					.size = TrieSegment_ZZallocsize(1),
					.align = TrieSegment_align
				}, nullptr, nullptr);

				auto res = AlcPtr_get(ptr);
				if (res) return Trie_seterr(res);

				auto data = (TrieSegment*)ptr;
				data->value = this->value;
				data->size = 1;
				data->bytes[0] = chr;

				// if index of the unset element was 0 the remaining one must be 1
				data->next = Trie_const(this->next[idx == 0 ? 1 : 0]);

				return Trie_ZZupcast(data, isoccupied);
			}

			AlcReq req = {
				.intent = AlcIntent_New,
				.size = TrieBranch_ZZallocsize(this_size),
				.align = _Alignof(TrieSegment)
			};

			auto ptr = Alc_invoke(alc, &req, nullptr, nullptr);
			auto res = AlcPtr_get(ptr);
			if (res) return Trie_seterr(res);

			auto data = (TrieBranch*)ptr;
			memcpy(data->map, this->map, sizeof(this->map));
			data->map[map_idx] &= ~map_bit;
			data->value = this->value;

			uint i = 0;
			for (; i < idx; i++) {
				data->next[i] = Trie_const(this->next[i]);
			}

			for (; i < this_size; i++) {
				data->next[i] = Trie_const(this->next[i + 1]);
			}

			return Trie_ZZupcast(data, FLAG(TrieFlag_Branch) | isoccupied);
		} else {
			AlcReq req = {
				.intent = AlcIntent_New,
				.size = TrieBranch_ZZallocsize(this_size),
				.align = _Alignof(TrieSegment)
			};

			auto ptr = Alc_invoke(alc, &req, nullptr, nullptr);
			auto res = AlcPtr_get(ptr);
			if (res) return Trie_seterr(res);

			auto data = (TrieBranch*)ptr;
			memcpy(data->map, this->map, sizeof(this->map));
			data->value = this->value;

			uint i = 0;
			for (; i < idx; i++) {
				data->next[i] = Trie_const(this->next[i]);
			}

			data->next[i] = next;
			i++;

			for (; i < this_size; i++) {
				data->next[i] = Trie_const(this->next[i]);
			}

			return Trie_upcast(data, FLAG(TrieFlag_Branch) | isoccupied);
		}

	} else {
		Trie next = Trie_ZZunset(this->next[idx], alc,
			segment + 1, segment_size - 1
		);

		if (Trie_iserr(next)) return next;

		if (Trie_isnull(next)) {
			this_size--;
			if (this_size == 0) UNREACHABLE;
			if (this_size == 1) {
				uint64_t map[4]; memcpy(map, this->map, sizeof(this->map));
				map[map_idx] &= ~map_bit;

				ubyte chr = TrieBranch_ZZfirst(map);
				Ptr value = this->value;

				Trie next = this->next[idx == 0 ? 1 : 0];

				#if Trie_CONSERVATIVE
					AlcReq req = {
						.intent = AlcIntent_Resize,
						.size = TrieSegment_ZZallocsize(1),
						.align = _Alignof(TrieSegment)
					};

					auto ptr = Alc_invoke(alc, &req, nullptr, this);
					auto res = AlcPtr_get(ptr);
					if (res) return Trie_seterr(res);

					auto data = (TrieSegment*)ptr;
				#else
					auto data = (TrieSegment*)this;
				#endif

				data->value = value;
				data->size = 1;
				data->bytes[0] = chr;

				// if index of the unset element was 0 the remaining one must be 1 and vice versa
				data->next = next;

				return Trie_upcast(data, isoccupied);
			}

			this->map[map_idx] &= ~map_bit;
			memmove(&this->next[idx], &this->next[idx + 1], (this_size - idx) * sizeof(Trie));
			return Trie_upcast(this, FLAG(TrieFlag_Branch) | isoccupied);
		} else {
			this->next[idx] = next;
			return Trie_upcast(this, FLAG(TrieFlag_Branch) | isoccupied);
		}
	}
}

AlcRes TrieBranch_ZZdestroy(Trie vthis, Alc alc) {
	if (Trie_isconst(vthis)) UNREACHABLE;

	TrieBranch *this = Trie_data(vthis);
	uint size = TrieBranch_ZZsize(this);

	for (uint i = 0; i < size; i++) {
		auto res = Trie_ZZdestroy(this->next[i], alc);
		if (res) return res;
	}

	return Alc_delete(alc, this);
}

OutStreamRes TrieBranch_ZZprint(Trie vthis, TrieSize depth, OutStream os) {
	const bool isoccupied = Trie_isoccupied(vthis);
	const TrieBranch *this = Trie_data(vthis);

	auto res = OutStreamRes_Ok;

	if (isoccupied) {
		PRINT(&res, os, "-| (",(Ptr)this->value,")");
	} else {
		PRINT(&res, os, "-|");
	}

	if (res) return res;

	uint child_idx = 0;
	for (uint i = 0; i < 4; i++) {
		u64 map = this->map[i];
		while (map) {
			uint idx = (uint)__builtin_ctzll(map);
			u8 c = (u8)(i * 64 + idx);
			String indent = { .data = chars_spc, .size = depth * 3 };
			PRINT(&res, os, "\n",indent);
			if (res) return res;

			//PRINT(&res, os, "|- '",VString_upcast(&c, 1, FLAG(VStringFlag, KEEPUTF)),"' ")
			if (res) return res;

			Trie_ZZprint(this->next[child_idx], os, depth + 1);
			child_idx++;
			map &= map - 1;
		}
	}
}
