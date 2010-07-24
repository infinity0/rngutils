/**
** Increase the kernel entropy pool size using data from stdin.
**
** This program assumes that the incoming data has maximum entropy, so don't
** feed it biased or non-random data.
**
*/
/*
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
#include <fcntl.h>
#include <linux/random.h>

#include "macros.h"

static unsigned long written = 0;

int main () {

	struct {
		struct rand_pool_info info;
		char buffer[POOL_RADIUS];
	} output;

	output.info.entropy_count = POOL_RADIUS << 3;
	output.info.buf_size = POOL_RADIUS;

	while (fread(&(output.buffer), sizeof(char), POOL_RADIUS, stdin) == POOL_RADIUS) {
		//struct rand_pool_info *info = (struct rand_pool_info*)(&output);
		//fprintf(stderr, "%08x %08x\n", info->entropy_count, info->buf_size);
		//fprintf(stderr, "%08x %08x %08x %08x\n", info->buf[0], info->buf[1], info->buf[2], info->buf[3]);
		//fprintf(stderr, "...\n", info->buf[0], info->buf[1], info->buf[2], info->buf[3]);
		//fprintf(stderr, "%08x %08x %08x %08x\n", info->buf[60], info->buf[61], info->buf[62], info->buf[63]);
		//fprintf(stderr, "%08x %08x\n", info->buf[64], info->buf[65]);
		int rnd;
		SUCCESS(rnd = open(DEV_RANDOM, O_WRONLY));
		SUCCESS(ioctl(rnd, RNDADDENTROPY, &output));
		close(rnd);
		++written;
		//fprintf(stderr, "wrote %d bytes\n", POOL_RADIUS);
	}

	if (feof(stdin)) {
		fprintf(stderr, "wrote %ld x %d bytes\n", written, POOL_RADIUS);
		return 0;
	} else {
		perror("error reading from stdin");
		return 1;
	}
}

