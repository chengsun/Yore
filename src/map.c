/* map.c */

#include "include/all.h"

#include "include/map.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/generate.h"
#include "include/graphics.h"
#include "include/dlevel.h"

#define MAP_MOVEABLE 3

Graph map_graph = NULL;

uint32_t get_sqattr (struct DLevel *lvl, int yloc, int xloc)
{
	uint32_t mvbl = 1;

	if (yloc < 0 || yloc >= map_graph->h ||
	    xloc < 0 || xloc >= map_graph->w)
		return -1;

	int n = map_buffer (yloc, xloc);
	if (lvl->monsIDs[n])
		return 2;
	Vector *things = lvl->things;
	int i;
	for (i = 0; i < things[n]->len; ++ i)
	{
		struct Thing *th = THING(things, n, i);
		if (th->type == THING_DGN)
		{
			if ((th->thing.mis.attr & 1) == 0)
			{
				mvbl = 0; /* unmoveable */
			}
		}
	}
	return mvbl;
}

int can_amove (int attr)
{
	if (attr == ~0)
		return attr;
	return (attr & MAP_MOVEABLE);
}

#define MAPITEM(nm,gl,at) {(nm), (gl), (at)}

struct map_item_struct map_items[] = {
	MAPITEM("lit space", ACS_BIGDOT,                        M_TSPT),
	MAPITEM("grass",     ACS_DOT|COL_BG(0,5,0)|COL_TXT_DEF, M_TSPT),
	MAPITEM("grass",     ACS_DOT|COL_BG(1,6,0)|COL_TXT_DEF, M_TSPT),
	MAPITEM("wall",      ACS_WALL,                          M_OPQ ),
	MAPITEM("downstair", '>',                               M_TSPT),
	MAPITEM("upstair",   '<',                               M_TSPT),
	MAPITEM("tree",      5  |COL_TXT(2,1,0)|COL_BG(0,0,0),  M_TSPT),
	MAPITEM("flower",    '*'|COL_TXT(0,8,15),               M_TSPT),
	MAPITEM("flower",    '*'|COL_TXT(12,12,3),              M_TSPT),
	MAPITEM("corridor",  '#',                               M_TSPT),
	MAPITEM("door",      '+'|COL_TXT(9,9,0),                M_OPQ ),
	MAPITEM("open door", 254|COL_TXT(9,9,0),                M_TSPT),
	MAPITEM("slime",     0xB2|COL_TXT(3,9,0),               M_TSPT),
	MAPITEM("_end_",     '\0',                              M_OPQ )
};

