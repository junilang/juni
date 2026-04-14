#ifndef Vec_SAFE
	#define Vec_SAFE BUILD_SAFE
#endif

typedef struct {
	ubyte *data;
	usize capacity;
	usize size;
} Vec;

void Vec_einit(Vec *this) {
	this->data = nullptr;
	this->capacity = 0;
	this->size = 0;
}

void Vec_init(Vec *this, usize capacity, Allocator alc) {
	this->size = 0;
	this->capacity = capacity;

	#if Vec_SAFE
		if (!capacity) PANIC("Vec_init: capacity = 0")
	#endif

	this->data = Allocator_new(alc, capacity);
}

void Vec_destroy(Vec *this, Allocator alc) {
	Allocator_delete(alc, this->data);
	#if Vec_SAFE
		Vec_einit(this);
	#endif
}

void ZZVec_expand(Vec *this, usize target, Allocator alc) {
	usize capacity = this->capacity;
	if (!capacity) capacity = 16;

	while (capacity < target) {
		usize next = capacity + (capacity / 2);
		#if Vec_SAFE
			if (next < capacity)
				PANIC("ZZVec_expand: overflow");
		#endif
		capacity = next;
	}

	this->data = Allocator_resize(alc, this->data, capacity);
	this->capacity = capacity;
}

Ptr Vec_append(Vec *this, usize size, Allocator alc) {
	#if Vec_SAFE
		usize new_size = usize_padd(this->size, size);
	#else
		usize new_size = this->size + size;
	#endif

	if (new_size > this->capacity) {
		ZZVec_expand(this, new_size, alc);
	}

	Ptr result = this->data + this->size;
	this->size = new_size;
	return result;
}

void Vec_drop(Vec *this, usize size) {
	#if Vec_SAFE
		if (size > this->size) {
			this->size = 0;
			return;
		}
	#endif
	this->size -= size;
}

void Vec_clear(Vec *this) {
	this->size = 0;
}

Ptr Vec_get(Vec *this, usize index) {
	return this->data + index;
}

Ptr Vec_begin(Vec *this) {
	return this->data;
}

Ptr Vec_end(Vec *this) {
	return this->data + this->size;
}

usize Vec_size(Vec *this, usize elem_size) {
	return this->size / elem_size;
}
