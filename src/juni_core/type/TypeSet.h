typedef u32 TypeSet_Size;

typedef struct {
	Type type;
	uhash hash;
} TypeSetElement;

typedef u32 TypeSetSize;

typedef struct {
	Allocator alc;
	Allocator type_alc;
	TypeSetElement *elements;
	TypeSetSize size;
	TypeSetSize capacity;
} TypeSet;

typedef enum : u8 {
	TypeSetFlag_BIT_CONST,
	TypeSetFlag_BIT_MOVE,
	TypeSetFlag_BIT_DUPLICATE,
} TypeSetFlag;

void TypeSet_add(TypeSet *this, Type *types, TypeSetSize size, TypeSetFlag flags);

void TypeSet_destroy(TypeSet *this) {
	Allocator_delete(this->alc, this->elements);
}

void TypeSet_init(TypeSet *this, Allocator alc, Allocator type_alc) {
	this->elements = nullptr;
	this->capacity = 0;
	this->size = 0;
	this->alc = alc;
	this->type_alc = type_alc;
}
