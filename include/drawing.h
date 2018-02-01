#ifndef VISION_H_INCLUDED
#define VISION_H_INCLUDED

#include <stdbool.h>
#include <stdint.h>

struct BresState
{
	int cy, cx;
	int ty, tx;
	int dy, dx;
	int sy, sx;
	int err;
	int done;
};

bool bres_draw (int, int, int, int, int, uint8_t *, uint8_t *, int (*)(int, int));
void bres_init (struct BresState *st, int fy, int fx, int ty, int tx);
void bres_iter (struct BresState *st);

#endif /* VISON_H_INCLUDED */

