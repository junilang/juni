#define PANIC__LINE_(line) #line
#define PANIC__LINE(line) PANIC__LINE_(line)


#define PANIC_HEADER "PANIC: "__FILE__":"PANIC__LINE(__LINE__)": "

#define PANIC(str) { \
	fputs(PANIC_HEADER, stderr); \
	fputs(str, stderr); \
	fputc('\n', stderr); \
	fflush(stderr); \
	abort(); \
}

#define PANICF(...) { \
	fputs(PANIC_HEADER, stderr); \
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
