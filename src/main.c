#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbit.h>
#include <stdatomic.h>

#include "util/_include.h"
#include "util/_include.c"

#include "typing/_include.h"
#include "typing/_include.c"

#include "parser/_include.h"
#include "parser/_include.c"

#if BUILD_TESTING
	#include "test.c"

	int main(int argc, char **argv) {
		return testing_entry(argc, argv);
	}

#else
	int main(int argc, char **argv) {
		FPRINT(stdout, "Hello Juni <3\n");
		return 0;
	}


#endif
