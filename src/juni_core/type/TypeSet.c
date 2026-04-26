#ifndef TypeSet_ENABLE_BINARY_SEARCH
	#define TypeSet_ENABLE_BINARY_SEARCH false
#endif

void ZZTypeSet_reserve(
	TypeSet *this, TypeSetSize count
) {
	const TypeSetSize newcap = this->size + count;
	if (newcap <= this->capacity) return;

	this->elements = Allocator_resize(this->alc,
		this->elements, sizeof(TypeSetElement) * newcap
	);

	this->capacity = newcap;
}


void TypeSet_add(
	TypeSet *this, Type *it, TypeSetSize size, TypeSetFlag flags
) {
	const Type *end = it + size;
	for (; it < end; it++) {
		Type type = *it;

		if (NullType_is(type)) continue;

		if (SetType_is(type)) {
			SetType *data = Type_this(type);
			TypeSetFlag sub_flags = flags &
				((TypeSetFlag)(Type_isconst(type) * FLAG_NOT(TypeSetFlag, MOVE)))
			;

			TypeSet_add(this, data->elements, data->size, sub_flags);
			continue;
		}

		{
			auto const hash = Type_hash(type, HASH_BASE);
			auto const this_size = this->size;

			TypeSetSize i = 0;

			#if TypeSet_ENABLE_BINARY_SEARCH
				// TODO
				#error "TypeSet binary search not implemented"
			#else

				for (; i < this_size; i++) {
					auto const el_hash = this->elements[i].hash;
					if (hash < el_hash) goto insert_before;

					if (hash == el_hash) {
						if (Type_equal(type, this->elements[i].type)) goto skip_equal;
						goto insert_before;
					}

				}

			#endif

			if (0) insert_before: {
				ZZTypeSet_reserve(this, (TypeSetSize)(end - it));
				memmove(
					&this->elements[i + 1], &this->elements[i],
					sizeof(TypeSetElement) * (this_size - i)
				);
			} else {
				ZZTypeSet_reserve(this, (TypeSetSize)(end - it));
			}

			this->size++;
			this->elements[i].hash = hash;

			if (flags & FLAG(TypeSetFlag, CONST)) {
				this->elements[i].type = Type_const(type);
			} else if (flags & FLAG(TypeSetFlag, MOVE)) {
				this->elements[i].type = type;
				*it = NullType;
			} else if (flags & FLAG(TypeSetFlag, DUPLICATE)) {
				this->elements[i].type = Type_copy(
					type, FLAG(TypeCopyFlag, DUPLICATE), this->type_alc
				);
			} else {
				this->elements[i].type = Type_copy(
					type, FLAG(TypeCopyFlag), this->type_alc
				);
			}
		}

		skip_equal:;
	}
}
