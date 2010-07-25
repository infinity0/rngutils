/**
** Generate random data from the practical variations of usleep(3).
** (C) 2010 Ximin Luo <infinity0@gmx.com>
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <argp.h>

#include "macros.h"

/* Command line parsing */

typedef struct options_s {
	char *args[1]; char *sleep_i; int verbose;
} options;

const char *argp_program_version = "rng-usleep 0.2";
const char *argp_program_bug_address = "<infinity0@gmx.com>";
static char argp_arg[] = "[LENGTH]";
static char argp_doc[] =
	"Generate random data from the practical time variations of usleep(3). "
	"If LENGTH is omitted, will generate until terminated. ";
static struct argp_option optinfo[] = {
	{ "verbose", 'v', 0, 0, "Provide extra information." },
	{ "interval", 'i', "TIME", 0,
	  "Time in microseconds to sleep for (default 40). On top of this, there "
	  "is processing overhead, typically in the order of a few dozen us. "
	},
	{ 0 }
};

static error_t parse_opt (int key, char *arg, struct argp_state *state) {
	options *opts = state->input;

	switch (key) {
	case ARGP_KEY_ARG:
		if (state->arg_num >= 1) { argp_usage(state); }
		opts->args[state->arg_num] = arg;
		break;
	case ARGP_KEY_END:
		if (state->arg_num < 0) { argp_usage(state); }
		break;
	case 'i':
		opts->sleep_i = arg;
		break;
	case 'v':
		opts->verbose = 1;
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

static struct argp argp = { optinfo, parse_opt, argp_arg, argp_doc };


/* Algorithms - random number generation */

long ts_nanos(void) {
	struct timespec ts;
	SUCCESS(clock_gettime(CLOCK_REALTIME, &ts));
	return ts.tv_nsec;
}

int count_bits_set(long v) {
	int c;
	for (c = 0; v; c++) v &= v - 1;
	return c;
}

int random_sleep_bit(int millis) {

	long start = ts_nanos();
	usleep(millis);

	long xor = start ^ ts_nanos();
	int set = count_bits_set(xor);
	//fprintf(stderr, "%064lx = %d = %d\n", xor, set&1, set);
	return set&1;
}

void generate_random(int length, int sleep_i, FILE* out) {

	unsigned char byte, offset = 0;
	unsigned long written = 0, lastflush = 0;

	for (;;) {
		byte <<= 1;
		if (random_sleep_bit(sleep_i)) { byte |= 1; }

		if (++offset == 8) {
			offset = 0;
			SUCCESS_E(fputc(byte, out), EOF);

			if (++lastflush == POOL_RADIUS) {
				lastflush = 0;
				fflush(out);
			}

			if (length > 0 && ++written == length) {
				fflush(out);
				break;
			}
		}
	}

}

int main(int argc, char *argv[]) {

	options opts = { { NULL }, NULL, 0 };
	argp_parse(&argp, argc, argv, 0, 0, &opts);

	int length = 0, sleep_i = 40;
	TO_INT(opts.args[0], length, "LENGTH");
	TO_INT(opts.sleep_i, sleep_i, "TIME");

	char *len_str, *pad_str;
	len_str = (length)? opts.args[0]: "";
	pad_str = (length)? " ": "";

	fprintf(stderr, "generating %s%srandom bytes @ %d+c us/bit\n",
	  len_str, pad_str, sleep_i);

	/*struct timespec ts;
	SUCCESS(clock_getres(CLOCK_REALTIME, &ts));
	fprintf(stderr, "clock resolution: %lds %ldns\n",
	  (long)ts.tv_sec, ts.tv_nsec);*/

	generate_random(length, sleep_i, stdout);
	return EXIT_SUCCESS;

}

