Trie TrieSegment_set(
	Trie vthis,
	const u8 *segment, TrieSize segment_size,
	Ptr value, Allocator alc
) {
	const TrieFlag isoccupied = Trie_flags(vthis) & FLAG(TrieFlag, OCCUPIED);
	const TrieFlag isconst = Trie_flags(vthis) & FLAG(TrieFlag, CONST);
	TrieSegment *const this = Trie_data(vthis);

	if (segment_size == 0) {
		if (isconst) {
			const TrieSize this_size = this->size;
			TrieSegment *data = Allocator_new(alc, ZZTrieSegment_allocsize(this_size));
			data->next = Trie_const(this->next);
			data->size = this_size;
			data->value = value;
			memcpy(data->bytes, this->bytes, this_size);
			return Trie_upcast(this, FLAG(TrieFlag, OCCUPIED));
		} else {
			this->value = value;
			return Trie_upcast(this, FLAG(TrieFlag, OCCUPIED));
		}
	}

	if (Trie_isnull(this->next)) {
		// validate this is a stub
		if (this->size == 0) UNREACHABLE;
		if (!isoccupied) UNREACHABLE;

		Trie stub = Trie_createstub(value, alc);

		if (isconst) {
			TrieSegment *data = Allocator_new(alc, ZZTrieSegment_allocsize(segment_size));
			data->size = segment_size;
			data->value = this->value;
			data->next = stub;
			return Trie_upcast(data, FLAG(TrieFlag, OCCUPIED));
		} else {
			TrieSegment *data = Allocator_resize(alc, this, ZZTrieSegment_allocsize(segment_size));
			data->size = segment_size;
			data->next = stub;
			memcpy(data->bytes, segment, segment_size);
			return Trie_upcast(data, FLAG(TrieFlag, OCCUPIED));
		}
	}

	const TrieSize this_size = this->size;

	TrieSize idx = 0;
	if (segment_size < this_size) {
		for (; idx < segment_size; idx++) {
			if (this->bytes[idx] != segment[idx]) goto found_split;
		}

		// entirety of string fits into this segment, need to split
		// in the middle and create two segments - no branch

		TrieSize right_size = this_size - idx;
		TrieSegment *right = Allocator_new(alc, ZZTrieSegment_allocsize(right_size));
		right->size = right_size;
		right->value = value;
		memcpy(right->bytes, &this->bytes[idx], right_size);

		Trie right_trie = Trie_upcast(right, FLAG(TrieFlag, OCCUPIED));

		if (isconst) {
			right->next = Trie_const(this->next);
			TrieSegment *left = Allocator_new(alc, ZZTrieSegment_allocsize(idx));
			left->size = idx;
			left->value = this->value;
			left->next = right_trie;
			memcpy(left->bytes, this->bytes, idx);
			return Trie_upcast(left, isoccupied);
		} else {
			right->next = this->next;
			this->size = idx;
			this->next = right_trie;
			return Trie_upcast(this, isoccupied);
		}
	} else {
		for (; idx < this_size; idx++) {
			if (this->bytes[idx] != segment[idx]) goto found_split;
		}

		// segment fits inside of new string, modify next node

		if (isconst) {
			TrieSegment *data = Allocator_new(alc, ZZTrieSegment_allocsize(this_size));
			data->next = Trie_set(Trie_const(this->next),
				segment + idx, segment_size - idx, value, alc
			);
			data->size = this_size;
			data->value = this->value;
			memcpy(data->bytes, this->bytes, this_size);
			return Trie_upcast(data, isoccupied);
		} else {
			this->next = Trie_set(this->next,
				segment + idx, segment_size - idx, value, alc
			);
			return Trie_upcast(this, isoccupied);
		}
	}

	found_split:;

	const u8 a = this->bytes[idx];
	const u8 b = segment[idx];

	const TrieSize idx1 = idx + 1;

	Trie stub = Trie_create(segment + idx1, segment_size - idx1, value, alc);

	if (idx == 0) {
		// if the first character didn't match, there is no need to create
		// a left side segment, and this segment can be reused for the right side

		const TrieSize rest_size = this_size - 1;
		Trie rest;

		if (isconst) {
			TrieSegment *data = Allocator_new(alc, ZZTrieSegment_allocsize(rest_size));
			data->size = rest_size;
			data->next = Trie_const(this->next);
			memcpy(data->bytes, this->bytes + 1, rest_size);
			rest = Trie_upcast(data, FLAG(TrieFlag));
		} else {
			this->size = rest_size;
			memmove(this->bytes, this->bytes + 1, rest_size);
			rest = Trie_upcast(this, FLAG(TrieFlag));
		}

		TrieBranch *data = Allocator_new(alc, ZZTrieBranch_allocsize(2));
		memset(data->map, 0, sizeof(data->map));
		data->value = this->value;
		data->map[a / 64] |= (u64)1 << (a % 64);
		data->map[b / 64] |= (u64)1 << (b % 64);

		if (a < b) {
			data->next[0] = rest;
			data->next[1] = stub;
		} else {
			data->next[0] = stub;
			data->next[1] = rest;
		}

		return Trie_upcast(data, FLAG(TrieFlag, BRANCH) | isoccupied);
	}

	Trie rest;
	if (this_size == idx1) { // if last character didn't match
		if (isconst)
			rest = Trie_const(this->next);
		else
			rest = this->next;
	} else {
		const TrieSize rest_size = this_size - idx1;

		TrieSegment *data = Allocator_new(alc, ZZTrieSegment_allocsize(rest_size));

		if (isconst)
			data->next = Trie_const(this->next);
		else
			data->next = this->next;

		data->size = rest_size;
		memcpy(data->bytes, this->bytes + idx1, rest_size);
		rest = Trie_upcast(data, FLAG(TrieFlag));
	}

	Trie branch;
	{
		TrieBranch *data = Allocator_new(alc, ZZTrieBranch_allocsize(2));

		memset(data->map, 0, sizeof(data->map));
		data->map[a / 64] |= (u64)1 << (a % 64);
		data->map[b / 64] |= (u64)1 << (b % 64);

		if (a < b) {
			data->next[0] = rest;
			data->next[1] = stub;
		} else {
			data->next[0] = stub;
			data->next[1] = rest;
		}

		branch = Trie_upcast(data, FLAG(TrieFlag, BRANCH));
	}

	if (isconst) {
		TrieSegment *data = Allocator_new(alc, ZZTrieSegment_allocsize(idx));
		data->size = idx;
		data->next = branch;
		data->value = this->value;
		memcpy(data->bytes, this->bytes, idx);
		return Trie_upcast(data, isoccupied);
	} else {
		this->next = branch;
		this->size = idx;
		return Trie_upcast(this, isoccupied);
	}
}

Trie TrieSegment_unset(
	Trie vthis,
	const u8 *segment, TrieSize segment_size,
	Allocator alc
) {
	// TODO: implement TrieSegment_unset
	PANIC("TrieSegment_unset: not implemented");
}

void TrieSegment_destroy(Trie vthis, Allocator alc) {
	if (Trie_isconst(vthis)) return;
	TrieSegment *this = Trie_data(vthis);

	if (!Trie_isnull(this->next)) {
		Trie_destroy(this->next, alc);
	}

	Allocator_delete(alc, this);
}

void TrieSegment_print(Trie vthis, TrieSize depth, OutStream os) {
	const bool isoccupied = Trie_isoccupied(vthis);
	TrieSegment *this = Trie_data(vthis);
	PRINT(os, "> ");

	if (isoccupied) PRINT(os, "(",(Ptr)this->value,") ");

	if (this->size) {
		const String s = {
			.data = this->bytes,
			.size = this->size
		};

		PRINT(os, s," ");
	}

	const Trie next = this->next;
	if (Trie_isnull(this->next)) return;

	Trie_print(next, depth, os);
}
