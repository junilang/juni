#ifndef Vec32_SAFE
	#define Vec32_SAFE BUILD_SAFE
#endif

typedef struct {
	ubyte *data;
	u32 size;
	u32 capacity;
} Vec32;

void Vec32_einit(Vec32 *this) {
	this->data = nullptr;
	this->capacity = 0;
	this->size = 0;
}

void Vec32_init(Vec32 *this, u32 capacity, Allocator alc) {
	this->size = 0;
	this->capacity = capacity;

	#if Vec32_SAFE
		if (!capacity) PANIC("Vec32_init: capacity = 0");
	#endif

	this->data = Allocator_new(alc, capacity);
}

void Vec32_destroy(Vec32 *this, Allocator alc) {
	Allocator_delete(alc, this->data);
	#if Vec32_SAFE
		Vec32_einit(this);
	#endif
}

void ZZVec32_expand(Vec32 *this, u32 target, Allocator alc) {
	u32 capacity = this->capacity;
	if (!capacity) capacity = 16;

	while (capacity < target) {
		u32 next = capacity + (capacity / 2);
		#if Vec32_SAFE
			if (next < capacity)
				PANIC("ZZVec32_expand: overflow")
		#endif
		capacity = next;
	}

	this->data = Allocator_resize(alc, this->data, capacity);
	this->capacity = capacity;
}

Ptr Vec32_append(Vec32 *this, u32 size, Allocator alc) {
	u32 new_size = this->size + size;
	#if Vec32_SAFE
		if (new_size < this->size) PANIC("Vec32_append: overflow");
	#endif

	if (new_size > this->capacity) {
		ZZVec32_expand(this, new_size, alc);
	}

	Ptr result = this->data + this->size;
	this->size = new_size;
	return result;
}

void Vec32_drop(Vec32 *this, u32 size) {
	#if Vec32_SAFE
		if (size > this->size) {
			this->size = 0;
			return;
		}
	#endif
	this->size -= size;
}

void Vec32_clear(Vec32 *this) {
	this->size = 0;
}

Ptr Vec32_get(Vec32 *this, u32 index) {
	return this->data + index;
}

Ptr Vec32_begin(Vec32 *this) {
	return this->data;
}

Ptr Vec32_end(Vec32 *this) {
	return this->data + this->size;
}

u32 Vec32_size(Vec32 *this, u32 elem_size) {
	return this->size / elem_size;
}
