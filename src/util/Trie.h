// whether to do micoroptimizations for conserving memory, e.g. resizing
// segment allocations when it is unnecessary
#ifndef Trie_CONSERVATIVE
	#define Trie_CONSERVATIVE true
#endif

#ifndef Trie_SAFE
	#define Trie_SAFE BUILD_SAFE
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
constexpr ualign Trie_align = TrieFlag_mask + 1;

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

Trie Trie_upcast(Ptr data, TrieFlag flags) {
	return (Trie){ .raw_value = (usize)data | flags };
}

TrieFlag Trie_flags(Trie this) {
	return (TrieFlag)(this.raw_value & TrieFlag_mask);
}

Trie Trie_orflags(Trie this, TrieFlag flags) {
	return (Trie){ .raw_value = this.raw_value | flags };
}

bool Trie_equal(Trie this, Trie other) {
	return ((this.raw_value ^ other.raw_value) & FLAG_CLEAR(TrieFlag_Const)) == 0;
}

typedef struct {
	Ptr value;
	u64 map[4];
	Trie next[];
} TrieBranch;

static_assert(Trie_align >= _Alignof(TrieBranch));

usize TrieBranch_allocsize(uint size) {
	return __builtin_offsetof(TrieBranch, next) + (size * sizeof(Trie));
}

typedef u16 TrieSize;
typedef struct {
	Ptr value;
	Trie next;
	TrieSize size;
	u8 bytes[];
} TrieSegment;

static_assert(Trie_align >= _Alignof(TrieSegment));

usize TrieSegment_allocsize(TrieSize size) {
	return __builtin_offsetof(TrieSegment, bytes) + size;
}

Trie Trie_createstub(Alc alc, Ptr value) {
	auto ptr = Alc_invoke(alc, &(AlcReq) {
		.intent = AlcIntent_New,
		.size = TrieSegment_allocsize(0),
		.align = Trie_align
	}, nullptr, nullptr);

	auto res = AlcPtr_get(ptr);
	if (res) return Trie_seterr(res);

	auto data = (TrieSegment*)ptr;
	data->next = Trie_null;
	data->value = value;
	data->size = 0;

	return Trie_upcast(data, FLAG(TrieFlag_Occupied));
}

Trie TrieSegment_create(
	Alc alc,
	const u8 *segment, TrieSize segment_size,
	Ptr value
) {
	#if Trie_SAFE
		ASSERT(segment_size != 0);
	#endif

	auto ptr = Alc_invoke(alc, &(AlcReq) {
		.intent = AlcIntent_New,
		.size = TrieSegment_allocsize(segment_size),
		.align = Trie_align
	}, nullptr, nullptr);

	auto res = AlcPtr_get(ptr);
	if (res) return Trie_seterr(res);

	auto data = (TrieSegment*)ptr;

	auto next = Trie_createstub(alc, value);
	if (Trie_iserr(next)) return next;

	data->next = next;
	data->value = nullptr;
	data->size = segment_size;
	memcpy(data->bytes, segment, segment_size);
	return Trie_upcast(data, 0);
}

Trie Trie_create(
	Alc alc,
	const u8 *segment, TrieSize segment_size,
	Ptr value
) {
	if (segment_size == 0)
		return Trie_createstub(alc, value);

	return TrieSegment_create(alc, segment, segment_size, value);
}

Trie Trie_set_(
	Trie this,
	Alc alc,
	const u8 *segment,
	TrieSize segment_size,
	Ptr value
);

Trie Trie_unset_(
	Trie this,
	Alc alc,
	const u8 *segment,
	TrieSize segment_size
);

AlcRes Trie_destroy_(
	Trie this,
	Alc alc
);

OutStreamRes Trie_print_(
	Trie this,
	OutStream os,
	TrieSize depth
);

#include "TrieBranch.h"
#include "TrieSegment.h"

Trie Trie_set_(
	Trie this,
	Alc alc,
	const u8 *segment,
	TrieSize segment_size,
	Ptr value
) {
	#if Trie_SAFE
		ASSERT(!Trie_isnull(this));
	#endif

	if (Trie_isbranch(this)) {
		return TrieBranch_set(this, alc, segment, segment_size, value);
	} else {
		return TrieSegment_set(this, alc, segment, segment_size, value);
	}
}

Trie Trie_unset_(
	Trie this,
	Alc alc,
	const u8 *segment,
	TrieSize segment_size
) {
	#if Trie_SAFE
		ASSERT(!Trie_isnull(this));
	#endif


	if (Trie_isbranch(this))
		return TrieBranch_unset(this, alc, segment, segment_size);
	else
		return TrieSegment_unset(this, alc, segment, segment_size);
}

AlcRes Trie_destroy_(Trie this, Alc alc) {
	#if Trie_SAFE
		ASSERT(
			!Trie_isnull(this) && !Trie_isconst(this)
		);
	#endif

	if (Trie_isbranch(this))
		return TrieBranch_destroy(this, alc);
	else
		return TrieSegment_destroy(this, alc);
}

OutStreamRes Trie_print_(Trie this, OutStream os, TrieSize depth) {
	#if Trie_SAFE
		ASSERT(!Trie_isnull(this));
	#endif


	if (Trie_isbranch(this))
		return TrieBranch_print(this, os, depth);
	else
		return TrieSegment_print(this, os, depth);
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

	return Trie_set_(this, alc, segment, segment_size, value);
}


Trie Trie_unset(
	Trie this,
	Alc alc,
	const u8 *segment,
	TrieSize segment_size
) {
	if (Trie_isnull(this))
		return this;

	return Trie_unset_(this, alc, segment, segment_size);
}

AlcRes Trie_destroy(
	Trie this,
	Alc alc
) {
	if (Trie_isnull(this) || Trie_isconst(this))
		return AlcRes_Ok;

	return Trie_destroy_(this, alc);
}

OutStreamRes Trie_print(
	Trie this,
	OutStream os
) {
	if (Trie_isnull(this))
		return OutStream_write(os, STR("Trie_null\n"));

	auto res = Trie_print_(this, os, 1);
	if (res) return res;
	return OutStream_write(os, STR("\n"));
}
