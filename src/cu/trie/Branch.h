uint ZZTrieBranch_size(
	const TrieBranch *this
) {
	return (
		stdc_count_ones_ull(this->map[0]) +
		stdc_count_ones_ull(this->map[1]) +
		stdc_count_ones_ull(this->map[2]) +
		stdc_count_ones_ull(this->map[3])
	);
}

uint ZZTrieBranch_index(
	const TrieBranch *this,
	u8 map_idx, u64 map_bit
) {
	switch (map_idx) {
		case 0:
			return (
				stdc_count_ones_ull(this->map[0] & (map_bit - 1))
			);
		case 1:
			return (
				stdc_count_ones_ull(this->map[0]) +
				stdc_count_ones_ull(this->map[1] & (map_bit - 1))
			);
		case 2:
			return (
				stdc_count_ones_ull(this->map[0]) +
				stdc_count_ones_ull(this->map[1]) +
				stdc_count_ones_ull(this->map[2] & (map_bit - 1))
			);
		case 3:
			return (
				stdc_count_ones_ull(this->map[0]) +
				stdc_count_ones_ull(this->map[1]) +
				stdc_count_ones_ull(this->map[2]) +
				stdc_count_ones_ull(this->map[3] & (map_bit - 1))
			);
	}

	UNREACHABLE;
}

Trie TrieBranch_set(
	Trie vthis,
	const u8 *segment, TrieSize segment_size,
	Ptr value, Allocator alc
) {
	const TrieFlag isoccupied = Trie_flags(vthis) & FLAG(TrieFlag, OCCUPIED);
	const TrieFlag isconst = Trie_flags(vthis) & FLAG(TrieFlag, CONST);
	TrieBranch *const this = Trie_data(vthis);

	if (segment_size == 0) {
		if (isconst) {
			const uint this_size = ZZTrieBranch_size(this);
			TrieBranch *data = Allocator_new(alc, ZZTrieBranch_allocsize(this_size));
			data->value = value;
			memcpy(data->map, this->map, sizeof(this->map));
			for (uint i = 0; i < this_size; i++) {
				data->next[i] = Trie_const(this->next[i]);
			}
			return Trie_upcast(data, FLAG(TrieFlag, BRANCH, OCCUPIED));
		} else {
			this->value = value;
			return Trie_upcast(this, FLAG(TrieFlag, BRANCH, OCCUPIED));
		}
	}

	const u8 chr = segment[0];
	const u8 map_idx = chr / 64;
	const u64 map_bit = (u64)1 << (chr % 64);

	if (this->map[map_idx] & map_bit) {
		const uint idx = ZZTrieBranch_index(this, map_idx, map_bit);

		if (isconst) {
			const uint this_size = ZZTrieBranch_size(this);
			TrieBranch *data = Allocator_new(alc, ZZTrieBranch_allocsize(this_size));
			data->value = this->value;
			memcpy(data->map, this->map, sizeof(this->map));

			for (uint i = 0; i < this_size; i++) {
				data->next[i] = Trie_const(this->next[i]);
			}

			data->next[idx] = Trie_set(
				data->next[idx],
				segment + 1, segment_size - 1,
				value, alc
			);
			return Trie_upcast(data, FLAG(TrieFlag, BRANCH) | isoccupied);
		} else {
			this->next[idx] = Trie_set(
				this->next[idx],
				segment + 1, segment_size - 1,
				value, alc
			);
			return Trie_upcast(this, FLAG(TrieFlag, BRANCH) | isoccupied);
		}
	}

	const Trie stub = Trie_create(segment + 1, segment_size - 1, value, alc);

	if (isconst) {
		const uint new_size = ZZTrieBranch_size(this) + 1;
		TrieBranch *data = Allocator_new(alc, ZZTrieBranch_allocsize(new_size));
		data->value = this->value;
		memcpy(data->map, this->map, sizeof(this->map));
		data->map[map_idx] |= map_bit;

		const uint idx = ZZTrieBranch_index(data, map_idx, map_bit);

		uint i = 0;
		for (; i < idx; i++) {
			data->next[i] = Trie_const(this->next[i]);
		}

		data->next[i] = stub;
		i++;

		for (; i < new_size; i++) {
			data->next[i] = Trie_const(this->next[i - 1]);
		}

		return Trie_upcast(data, FLAG(TrieFlag, BRANCH) | isoccupied);
	} else {
		const uint this_size = ZZTrieBranch_size(this);
		TrieBranch *data = Allocator_resize(alc, this, ZZTrieBranch_allocsize(this_size + 1));

		data->map[map_idx] |= map_bit;
		const uint idx = ZZTrieBranch_index(data, map_idx, map_bit);

		memmove(&data->next[idx + 1], &data->next[idx], (this_size - idx) * sizeof(Trie));
		data->next[idx] = stub;

		return Trie_upcast(data, FLAG(TrieFlag, BRANCH) | isoccupied);
	}
}

Trie TrieBranch_unset(
	Trie vthis,
	const u8 *segment, TrieSize segment_size,
	Allocator alc
) {
	// TODO: implement TrieBranch_unset
	PANIC("TrieBranch_unset: not implemented");
}

void TrieBranch_destroy(Trie vthis, Allocator alc) {
	if (Trie_isconst(vthis)) return;
	TrieBranch *this = Trie_data(vthis);

	uint size = ZZTrieBranch_size(this);

	for (uint i = 0; i < size; i++) {
		Trie_destroy(this->next[i], alc);
	}

	Allocator_delete(alc, this);
}

void TrieBranch_print(Trie vthis, TrieSize depth, OutStream os) {
	const bool isoccupied = Trie_isoccupied(vthis);
	const TrieBranch *this = Trie_data(vthis);

	if (isoccupied) {
		PRINT(os, "-| (",(Ptr)this->value,")");
	} else {
		PRINT(os, "-|");
	}

	uint child_idx = 0;
	for (uint i = 0; i < 4; i++) {
		u64 map = this->map[i];
		while (map) {
			uint idx = stdc_trailing_zeros_ull(map);
			u8 c = (u8)(i * 64 + idx);
			String indent = {.data=g_whitespace,.size=depth * 3};
			PRINT(os, "\n",indent);

			if (c < 32 || c > 127) {
				PRINT(os, "|- \\",(ubyte)c," ");
			} else {
				PRINT(os, "|- ",(char)c," ");
			}

			Trie_print(this->next[child_idx], depth + 1, os);
			child_idx++;
			map &= map - 1;
		}
	}
}
