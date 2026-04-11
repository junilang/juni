#define UNIQUEPTR(name) struct {char name##_UNIQUEPTR__;} *name
#define UNIQUENUM(T, name) enum : T {name##_UNIQUENUM__} name
#define STRUCTDECL(name) struct name name

#define USTR(str) (const ubyte*)str, (sizeof(str) - 1)
