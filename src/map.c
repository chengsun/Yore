/* map.c */

#include "include/all.h"

#include "include/map.h"
#include "include/monst.h"
#include "include/thing.h"
#include "include/generate.h"
#include "include/graphics.h"

#define MAP_MOVEABLE 3

/* remember -- ONLY ONE MONSTER PER SQUARE */
struct Monster *get_square_monst (uint32_t yloc, uint32_t xloc, int level)
{
	int n = to_buffer(yloc, xloc);
	LOOP_THING(n, i)
	{
		struct Thing *th = THING(n, i);
		if (th->type == THING_MONS)
			return (th->thing);
	}
	/* no monster */
	return NULL;
}

uint32_t get_square_attr (uint32_t yloc, uint32_t xloc, int level)
{
	uint32_t mvbl = 1;

	if (yloc >= MAP_HEIGHT || xloc >= MAP_WIDTH)
		return -1;

	int n = to_buffer(yloc, xloc);
	LOOP_THING(n, i)
	{
		struct Thing *th = THING(n, i);
		if (th->type == THING_MONS)
		{
			mvbl = 2;		/* attack */
			break;
		}
		if (th->type == THING_DGN)
		{
			if ((((struct map_item_struct *)(th->thing))->attr & 1) == 0)
			{
				mvbl = 0;	/* unmoveable */
			}
		}
	}
	return mvbl;
}

uint32_t can_move_to (uint32_t attr)
{
	if (attr == (uint32_t) - 1)
		return (uint32_t) - 1;
	return (attr & MAP_MOVEABLE);
}

#define MAPITEM(nm,ch,at,cl) {nm,ch,at}

struct map_item_struct map_items[] = {
	MAPITEM("lit space", ACS_BULLET, M_TSPT, 0),
	MAPITEM("wall", 255, M_OPQ, 0),
	MAPITEM("downstair", '>', M_TSPT, 0),
	MAPITEM("upstair", '<', M_TSPT, 0),
	MAPITEM("tree", '+', M_OPQ, 0),
	MAPITEM("flower", '*', M_TSPT, 0),
	MAPITEM("corridor", '#', M_TSPT, 0),
	MAPITEM("_end_", '\0', M_OPQ, 0)
};

int GETMAPITEMID(char i)
{
	int n;

	for (n = 0; map_items[n].ch != '\0'; ++n)
	{
		if (map_items[n].ch == i)
			return n;
	}
	return n;
}
