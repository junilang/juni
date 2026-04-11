typedef uint8_t ubyte;
typedef int8_t ibyte;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

static_assert(sizeof(uintptr_t) == sizeof(size_t));
static_assert(alignof(uintptr_t) == alignof(size_t));

typedef uintptr_t usize;
typedef ptrdiff_t isize;
typedef uint16_t ualign;

typedef void *Ptr;
typedef const char *Str;

#define USIZE_MAX UINTPTR_MAX
