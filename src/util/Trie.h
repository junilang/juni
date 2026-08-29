// whether to do micoroptimizations for conserving memory, e.g. resizing
// segment allocations when it is unnecessary
#ifndef Trie_CONSERVATIVE
	#define Trie_CONSERVATIVE true
#endif

typedef union {
	Ptr value;
	usize raw_value;
} Trie;

constexpr Trie Trie_null = { .raw_value=0 };

bool Trie_isnull(Trie this) {
	return !this.value;
}

// use AlcPtr as value to propagate allocation errors
Trie Trie_seterr(AlcRes err) {
	return (Trie){ .value = AlcPtr_set(err) };
}

bool Trie_iserr(Trie this) {
	return AlcPtr_check(this.value);
}

AlcRes Trie_geterr(Trie this) {
	return AlcPtr_get(this.value);
}

typedef u8 TrieFlag; enum {
	// determines trie node type, if 1 -> branch else segment
	FLAG_DEF(TrieFlag_Branch),
	FLAG_DEF(TrieFlag_Occupied),
	FLAG_DEF(TrieFlag_Const)
};

constexpr u8 TrieFlag_bits = 3;
constexpr usize TrieFlag_mask = 0b111ull;

bool Trie_isbranch(Trie this) {
	return this.raw_value & FLAG(TrieFlag_Branch);
}

bool Trie_isoccupied(Trie this) {
	return this.raw_value & FLAG(TrieFlag_Occupied);
}

bool Trie_isconst(Trie this) {
	return this.raw_value & FLAG(TrieFlag_Const);
}

Ptr Trie_data(Trie this) {
	return (Ptr)(this.raw_value & (~TrieFlag_mask));
}

Trie Trie_const(Trie this) {
	return (Trie){ .raw_value = this.raw_value | FLAG(TrieFlag_Const) };
}

Trie Trie_ZZupcast(Ptr data, TrieFlag flags) {
	return (Trie){ .raw_value = (usize)data | flags };
}

TrieFlag Trie_ZZflags(Trie this) {
	return (TrieFlag)(this.raw_value & TrieFlag_mask);
}

Trie Trie_ZZorflags(Trie this, TrieFlag flags) {
	return (Trie){ .raw_value = this.raw_value | flags };
}

bool Trie_ZZequal(Trie this, Trie other) {
	return ((this.raw_value ^ other.raw_value) & FLAG_NOT(TrieFlag_Const)) == 0;
}

typedef struct {
	Ptr value;
	u64 map[4];
	Trie next[];
} TrieBranch;

static_assert(_Alignof(TrieBranch) > TrieFlag_mask);

constexpr ualign TrieBranch_align = _Alignof(TrieBranch);

usize TrieBranch_ZZallocsize(uint size) {
	return __builtin_offsetof(TrieBranch, next) + (size * sizeof(Trie));
}

typedef u16 TrieSize;
typedef struct {
	Ptr value;
	Trie next;
	TrieSize size;
	u8 bytes[];
} TrieSegment;

static_assert(_Alignof(TrieSegment) > TrieFlag_mask);

constexpr ualign TrieSegment_align = _Alignof(TrieSegment);

usize TrieSegment_ZZallocsize(TrieSize size) {
	return __builtin_offsetof(TrieSegment, bytes) + size;
}

Trie Trie_ZZcreatestub(Alc alc, Ptr value) {
	auto ptr = Alc_invoke(alc, &(AlcReq) {
		.intent = AlcIntent_New,
		.size = TrieSegment_ZZallocsize(0),
		.align = TrieSegment_align
	}, nullptr, nullptr);

	auto res = AlcPtr_get(ptr);
	if (res) return Trie_seterr(res);

	auto data = (TrieSegment*)ptr;
	data->next = Trie_null;
	data->value = value;
	data->size = 0;

	return Trie_ZZupcast(data, FLAG(TrieFlag_Occupied));
}

Trie Trie_ZZcreatesegment(
	Alc alc,
	const u8 *segment, TrieSize segment_size,
	Ptr value
) {
	if (segment_size == 0) UNREACHABLE;

	auto ptr = Alc_invoke(alc, &(AlcReq) {
		.intent = AlcIntent_New,
		.size = TrieSegment_ZZallocsize(segment_size),
		.align = TrieSegment_align
	}, nullptr, nullptr);

	auto res = AlcPtr_get(ptr);
	if (res) return Trie_seterr(res);

	auto data = (TrieSegment*)ptr;

	auto next = Trie_ZZcreatestub(alc, value);
	if (Trie_iserr(next)) return next;

	data->next = next;
	data->value = nullptr;
	data->size = segment_size;
	memcpy(data->bytes, segment, segment_size);
	return Trie_ZZupcast(data, FLAGS(TrieFlag));
}


Trie Trie_ZZset(
	Trie this,
	Alc alc,
	const u8 *segment,
	TrieSize segment_size,
	Ptr value
);

Trie Trie_ZZunset(
	Trie this,
	Alc alc,
	const u8 *segment,
	TrieSize segment_size
);

AlcRes Trie_ZZdestroy(
	Trie this,
	Alc alc
);

OutStreamRes Trie_ZZprint(
	Trie this,
	OutStream os,
	TrieSize depth
);

#include "TrieBranch.h"
#include "TrieSegment.h"

Trie Trie_ZZset(
	Trie this,
	Alc alc,
	const u8 *segment,
	TrieSize segment_size,
	Ptr value
) {
	if (Trie_isnull(this)) UNREACHABLE;

	if (Trie_isbranch(this)) {
		return TrieBranch_ZZset(this, alc, segment, segment_size, value);
	} else {
		return TrieSegment_ZZset(this, alc, segment, segment_size, value);
	}
}

Trie Trie_ZZunset(
	Trie this,
	Alc alc,
	const u8 *segment,
	TrieSize segment_size
) {
	if (Trie_isnull(this)) UNREACHABLE;

	if (Trie_isbranch(this))
		return TrieBranch_ZZunset(this, alc, segment, segment_size);
	else
		return TrieSegment_ZZunset(this, alc, segment, segment_size);
}

AlcRes Trie_ZZdestroy(Trie this, Alc alc) {
	if (Trie_isnull(this) || Trie_isconst(this)) UNREACHABLE;

	if (Trie_isbranch(this))
		return TrieBranch_ZZdestroy(this, alc);
	else
		return TrieSegment_ZZdestroy(this, alc);
}

OutStreamRes Trie_ZZprint(Trie this, OutStream os, TrieSize depth) {
	if (Trie_isnull(this)) UNREACHABLE;

	if (Trie_isbranch(this))
		return TrieBranch_ZZprint(this, os, depth);
	else
		return TrieSegment_ZZprint(this, os, depth);
}

Trie Trie_create(
	Alc alc,
	const u8 *segment, TrieSize segment_size,
	Ptr value
) {
	if (segment_size == 0)
		return Trie_ZZcreatestub(alc, value);

	return Trie_ZZcreatesegment(alc, segment, segment_size, value);
}

Trie Trie_set(
	Trie this,
	Alc alc,
	const u8 *segment,
	TrieSize segment_size,
	Ptr value
) {
	if (Trie_isnull(this))
		return Trie_create(alc, segment, segment_size, value);

	return Trie_ZZset(this, alc, segment, segment_size, value);
}


Trie Trie_unset(
	Trie this,
	Alc alc,
	const u8 *segment,
	TrieSize segment_size
) {
	if (Trie_isnull(this))
		return this;

	return Trie_ZZunset(this, alc, segment, segment_size);
}

AlcRes Trie_destroy(
	Trie this,
	Alc alc
) {
	if (Trie_isnull(this) || Trie_isconst(this))
		return AlcRes_Ok;

	return Trie_ZZdestroy(this, alc);
}

OutStreamRes Trie_print(
	Trie this,
	OutStream os
) {
	if (Trie_isnull(this))
		return OutStream_write(os, STR("Trie_null\n"));

	auto res = Trie_ZZprint(this, os, 1);
	if (res) return res;
	return OutStream_write(os, STR("\n"));
}
