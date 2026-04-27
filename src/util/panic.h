#define PANIC__LINE_(line) #line
#define PANIC__LINE(line) PANIC__LINE_(line)

#define PANIC_HEADER \
	fputs("PANIC: "__FILE__":"PANIC__LINE(__LINE__)" in ", stderr); \
	fputs(__func__, stderr); \
	fputs(": ", stderr);

#define PANIC(str) { \
	PANIC_HEADER \
	fputs(str, stderr); \
	fputc('\n', stderr); \
	fflush(stderr); \
	abort(); \
}

#define PANICF(...) { \
	PANIC_HEADER \
	FPRINT(stderr, __VA_ARGS__); \
	fputc('\n', stderr); \
	fflush(stderr); \
	abort(); \
}

#if BUILD_DEBUG
	#define UNREACHABLE PANIC("unreachable")
#else
	#define UNREACHABLE unreachable()
#endif
