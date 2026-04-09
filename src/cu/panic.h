#define PANIC__LINE_(line) #line
#define PANIC__LINE(line) PANIC__LINE_(line)

#define PANIC(str) { \
	fputs("PANIC "__FILE__":"PANIC__LINE(__LINE__)": "str"\n", stderr); \
	fflush(stderr); \
	abort(); \
}


#if BUILD_DEBUG
	#define UNREACHABLE PANIC("unreachable broken")
#else
	#define UNREACHABLE unreachable()
#endif
