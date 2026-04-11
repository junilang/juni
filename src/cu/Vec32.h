#ifndef Vec32_SAFE
	#define Vec32_SAFE BUILD_SAFE
#endif

typedef struct {
	const ubyte *data;
	u32 size;
	u32 capacity;
} Vec32;
