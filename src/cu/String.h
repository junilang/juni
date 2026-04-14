typedef struct {
	const ubyte *data;
	usize size;
} String;

typedef struct {
	const ubyte *begin;
	const ubyte *end;
} StringSpan;

#define USTR(str) (const ubyte*)(str), __builtin_strlen(str)
#define STRING(str) ((String){.data=(const ubyte*)(str), .size=__builtin_strlen(str)})
