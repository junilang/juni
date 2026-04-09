#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cu/_include.h"
#include "cu/_include.c"

#include "juni_core/_include.h"
#include "juni_core/_include.c"

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
