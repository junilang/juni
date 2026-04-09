#define UNIQUEPTR(name) struct {char name##_UNIQUEPTR__;} *name
#define UNIQUENUM(T, name) enum : T {name##_UNIQUENUM__} name
#define STRUCTDECL(name) struct name name

#define FLAG_X(pfx, flag) (1ULL << pfx##_BIT_##flag)
#define FLAG_8(pfx, flag, ...) FLAG_X(pfx, flag)__VA_OPT__(;FLAG_MAX_DEPTH_REACHED)
#define FLAG_7(pfx, flag, ...) FLAG_X(pfx, flag)__VA_OPT__(|FLAG_8(pfx, __VA_ARGS__))
#define FLAG_6(pfx, flag, ...) FLAG_X(pfx, flag)__VA_OPT__(|FLAG_7(pfx, __VA_ARGS__))
#define FLAG_5(pfx, flag, ...) FLAG_X(pfx, flag)__VA_OPT__(|FLAG_6(pfx, __VA_ARGS__))
#define FLAG_4(pfx, flag, ...) FLAG_X(pfx, flag)__VA_OPT__(|FLAG_5(pfx, __VA_ARGS__))
#define FLAG_3(pfx, flag, ...) FLAG_X(pfx, flag)__VA_OPT__(|FLAG_4(pfx, __VA_ARGS__))
#define FLAG_2(pfx, flag, ...) FLAG_X(pfx, flag)__VA_OPT__(|FLAG_3(pfx, __VA_ARGS__))
#define FLAG_1(pfx, flag, ...) FLAG_X(pfx, flag)__VA_OPT__(|FLAG_2(pfx, __VA_ARGS__))
#define FLAG(pfx, flag, ...)   FLAG_X(pfx, flag)__VA_OPT__(|FLAG_1(pfx, __VA_ARGS__))

#define FLAG_NOT(pfx, ...) (~(FLAG(pfx, __VA_ARGS__)))

#define STR(s) ((const ubyte*)(s)), (sizeof(s) - 1)
