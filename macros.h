#include <stdio.h>
#include <stdlib.h>

#define SUCCESS_E(X, E) if ((X) == (E)) {\
	perror(#X);\
	exit(EXIT_FAILURE);\
}
#define SUCCESS(X) SUCCESS_E((X), -1)

#define EXIT_SYNTAX 2

#define TO_INT(SRC, DST, NAME) if ((SRC) != NULL && sscanf((SRC), "%d", &(DST)) != 1) {\
	fprintf(stderr, NAME " must be an integer: %s\n", (SRC));\
	return EXIT_SYNTAX;\
}

/* This should be set to half the maximum pool size, in bytes */
#define POOL_RADIUS 256

#define DEV_RANDOM "/dev/random"

