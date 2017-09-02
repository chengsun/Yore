#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "include/all.h"
#include "include/graphics.h"

#define M_OPQ  0
#define M_TSPT 1

#define ACS_WALL 255

#define PANE_H 10

typedef uint32_t SqAttr;

enum
{
	DGN_GROUND = 0,
	DGN_GRASS1,
	DGN_GRASS2,
	DGN_WALL,
	DGN_DOWNSTAIR,
	DGN_UPSTAIR,
	DGN_TREE,
	DGN_FLOWER1,
	DGN_FLOWER2,
	DGN_CORRIDOR
};

struct map_item_struct
{
	char name[20];
	glyph gl;
	SqAttr attr;
};

extern struct map_item_struct map_items[];
extern Graph map_graph;
int map_buffer (int, int);
int GETMAPITEMID(char);

#endif /* MAP_H_INCLUDED */

