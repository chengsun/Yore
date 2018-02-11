/* thing.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/item.h"
#include "include/monst.h"
#include "include/map.h"
#include "include/generate.h"
#include "include/drawing.h"
#include "include/graphics.h"
#include "include/dlevel.h"
#include "include/event.h"
#include "include/rand.h"

TID curID = 0;
TID getID ()
{
	return (++curID);
}

MID curMID = 0;
MID getMID ()
{
	return (++curMID);
}

void thing_watchvec (Vector vec)
{
	int i;
	for (i = 0; i < vec->len; ++ i)
	{
		struct Thing *th = v_at (vec, i);
		THIID (th->ID) = th;
	}
}

void rem_id (TID id)
{
	struct Thing *th = THIID(id);
	struct DLevel *lvl = dlv_lvl (th->dlevel);
	int y = th->yloc, x = th->xloc;
	int n = map_buffer (y, x);
	v_rptr (lvl->things[n], th);
	thing_watchvec (lvl->things[n]);
	THIID(id) = NULL;
	draw_map_buf (lvl, n);
}

void update_item_pointers (Vector vec)
{
	int i;
	for (i = 0; i < vec->len; ++ i)
	{
		struct Item *item = v_at (vec, i);
		ITEMID (item->ID) = item;
	}
}

void rem_itemid (TID ID)
{
	struct Item *item = ITEMID(ID);
	if (!item)
		return;
	int n;
	Vector items;
	struct DLevel *lvl = NULL;
	if (item->loc.loc == LOC_DLVL)
	{
		n = map_buffer (item->loc.dlvl.yloc, item->loc.dlvl.xloc);
		lvl = dlv_lvl(item->loc.dlvl.dlevel);
		items = lvl->items[n];
	}
	else if (item->loc.loc == LOC_FLIGHT)
	{
		n = map_buffer (item->loc.fl.yloc, item->loc.fl.xloc);
		lvl = dlv_lvl(item->loc.fl.dlevel);
		items = lvl->items[n];
	}
	struct Monster *mons;
	switch (item->loc.loc)
	{
	case LOC_NONE:
		return;
	case LOC_DLVL:
	case LOC_FLIGHT:
		ITEMID(ID) = NULL;
		v_rem (items, ((uintptr_t)item - (uintptr_t)items->data)/sizeof(*item));
		update_item_pointers (items);
		draw_map_buf (lvl, n);
		return;
	case LOC_INV:
		ITEMID(item->ID) = NULL;
		pack_rem (MTHIID(item->loc.inv.monsID)->pack, item->loc.inv.invnum);
		return;
	case LOC_WIELDED:
		ITEMID(item->ID) = NULL;
		mons = MTHIID(item->loc.wield.monsID);
		mons->wearing.weaps[item->loc.wield.arm] = NULL;
		pack_rem (mons->pack, item->loc.wield.invnum);
		return;
	}
	panic("End of rem_itemid reached");
}

void item_free (struct Item *item)
{
	rem_itemid (item->ID);
	if (item->name)
		free(item->name);
}

void item_makeID (struct Item *item)
{
	if (!item->ID)
	{
		item->ID = getID();
		v_push (all_ids, &item);
	}
	else
		ITEMID(item->ID) = item;
}

struct Item *instantiate_item (union ItemLoc loc, struct Item *from)
{
	struct Item it;
	struct Item *ret;
	int n;
	Vector items;
	struct DLevel *lvl = NULL;
	if (loc.loc == LOC_DLVL)
	{
		n = map_buffer (loc.dlvl.yloc, loc.dlvl.xloc);
		lvl = dlv_lvl(loc.dlvl.dlevel);
		items = lvl->items[n];
	}
	else if (loc.loc == LOC_FLIGHT)
	{
		n = map_buffer (loc.fl.yloc, loc.fl.xloc);
		lvl = dlv_lvl(loc.fl.dlevel);
		items = lvl->items[n];
	}
	struct Monster *th;
	switch (loc.loc)
	{
	case LOC_NONE:
		break;
	case LOC_DLVL:
	case LOC_FLIGHT:
		memcpy (&it, from, sizeof(struct Item));
		memcpy (&it.loc, &loc, sizeof(loc));
		ret = v_push (items, &it);
		item_makeID (ret);
		update_item_pointers (items);
		draw_map_buf (lvl, n);
		return ret;
	case LOC_INV:
		memcpy (&it, from, sizeof(struct Item));
		memcpy (&it.loc, &loc, sizeof(loc));
		th = MTHIID (loc.inv.monsID);
		if (!pack_add (&th->pack, &it, loc.inv.invnum))
			panic("item already in inventory location in new_item");
		ret = &th->pack->items[loc.inv.invnum];
		item_makeID (ret);
		return ret;
	case LOC_WIELDED:
		memcpy (&it, from, sizeof(struct Item));
		memcpy (&it.loc, &loc, sizeof(loc));
		th = MTHIID (loc.wield.monsID);
		if (!pack_add (&th->pack, &it, loc.wield.invnum))
			panic("item already in inventory location in new_item");
		ret = &th->pack->items[loc.wield.invnum];
		item_makeID (ret);
		if (th->wearing.weaps[loc.wield.arm])
			panic("already wielding an item in new_item");
		th->wearing.weaps[loc.wield.arm] = ret;
		return ret;
	}
	panic("end of new_item reached");
	return 0;
}

struct Item *item_put (struct Item *item, union ItemLoc loc)
{
	struct Item temp;
	memcpy (&temp, item, sizeof(temp));
	rem_itemid (item->ID);
	return instantiate_item (loc, &temp);
}

void thing_free (struct Thing *thing)
{
	if (!thing)
		return;

	switch (thing->type)
	{
		case THING_DGN:
		case THING_NONE:
			break;
	}
}

int TSIZ[] = {
	0,
	sizeof (struct map_item_struct)
};

struct Thing *new_thing (uint32_t type, struct DLevel *lvl, uint32_t y, uint32_t x, void *actual_thing)
{
	int n = map_buffer (y, x);
	struct Thing t = {type, lvl->level, getID(), y, x, {}};

	if (t.ID != all_ids->len)
		panic ("IDs error");

	memcpy (&t.thing, actual_thing, TSIZ[type]);
	struct Thing *ret;
	ret = v_push (lvl->things[n], &t);
	v_push (all_ids, &ret);
	thing_watchvec (lvl->things[n]);
	lvl->attr[n] = ret->thing.mis.attr & 1;
	draw_map_buf (lvl, n);

	return ret;
}

void rem_mid (MID id)
{
	struct Monster *th = MTHIID(id);
	struct DLevel *lvl = dlv_lvl (th->dlevel);
	int n = map_buffer (th->yloc, th->xloc);
	lvl->monsIDs[n] = 0;
	memset (th, 0, sizeof(struct Monster));
	draw_map_buf (lvl, n);
	return;
}

struct Monster *MTHIID (MID id)
{
	struct Monster *ret = v_at (cur_dlevel->mons, (id));
	if (!ret->ID)
		return NULL;
	return ret;
}

void monsthing_move (struct Monster *thing, int new_level, int new_y, int new_x)
{
	if (thing->yloc == new_y && thing->xloc == new_x && thing->dlevel == new_level)
		return;

	struct DLevel *olv = dlv_lvl (thing->dlevel),
	              *nlv = dlv_lvl (new_level);

	int old = map_buffer (thing->yloc, thing->xloc),
	    new = map_buffer (new_y, new_x);

	if (nlv->monsIDs[new]) panic ("monster already there");
	nlv->monsIDs[new] = olv->monsIDs[old];
	olv->monsIDs[old] = 0;

	thing->yloc = new_y;
	thing->xloc = new_x;
	thing->dlevel = new_level;
	draw_map_buf (nlv, new);
}

struct Monster *new_mons (struct DLevel *lvl, uint32_t y, uint32_t x, void *actual_thing)
{
	int n = map_buffer (y, x);
	struct Monster t;
	memcpy (&t, actual_thing, sizeof(struct Monster));
	t.ID = getMID();
	t.dlevel = lvl->level;
	t.yloc = y; t.xloc = x;
	mons_stats_changed (&t);
	if (lvl->monsIDs[n]) panic ("monster already there!");
	lvl->monsIDs[n] = t.ID;
	struct Monster *ret = v_push (lvl->mons, &t);
	draw_map_buf (lvl, n);
	return ret;
}

/* these are in binary order, clockwise from top */
char ACS_ARRAY[17] = {
	ACS_DOT,
	ACS_TSTUB,
	ACS_RSTUB,
	ACS_LLCORNER,
	ACS_BSTUB,
	ACS_VLINE,
	ACS_ULCORNER,
	ACS_LTEE,
	ACS_LSTUB,
	ACS_LRCORNER,
	ACS_HLINE,
	ACS_BTEE,
	ACS_URCORNER,
	ACS_RTEE,
	ACS_TTEE,
	ACS_PLUS,
	' '
};

/* unused */
int wall_output[256] = {
/* 0 */
0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3,
4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 5,
0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3,
4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 5,
/* 64 */
8, 9, 8, 9, 10, 11, 10, 11, 8, 9, 8, 9, 10, 11, 10, 11,
12, 13, 12, 13, 14, 15, 14, 15, 12, 13, 12, 13, 14, 15, 14, 13,
8, 9, 8, 9, 10, 11, 10, 11, 8, 9, 8, 9, 10, 11, 10, 11,
12, 13, 12, 13, 14, 15, 14, 15, 12, 13, 12, 13, 10, 11, 10, 9,
/* 128 */
0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3,
4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 5,
0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3,
4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 5,
/* 192 */
8, 9, 8, 9, 10, 11, 10, 10, 8, 9, 8, 9, 10, 11, 10, 10,
12, 13, 12, 13, 14, 15, 14, 14, 12, 13, 12, 13, 14, 15, 14, 12,
8, 9, 8, 9, 10, 11, 10, 10, 8, 9, 8, 9, 10, 11, 10, 10,
12, 5, 12, 5, 14, 7, 14, 6, 12, 5, 12, 5, 10, 3, 10, 16
/*256 */
};

/* What this function does is purely cosmetic - given whether or not
 * the squares surrounding are walls or spaces, this function returns what
 * character should be displayed (corner, straight line, tee, etc). */
glyph WALL_TYPE (int y, int u, int h, int j, int k, int l, int b, int n)
{
	int i = ((k==1) + (l==1)*2 + (j==1)*4 + (h==1)*8) &
		(~((h&&y&&u&&l)<<0)) & (~((j&&n&&u&&k)<<1)) & (~((l&&n&&b&&h)<<2)) & (~((k&&y&&b&&j)<<3));
	return ACS_ARRAY[i];
}
/*
void walls_test ()
{
	int i;
	for (i = 0; i < 256; ++ i)
	{
		int Y = (i&128) > 0,
		    H = (i&64) > 0,
		    B = (i&32) > 0,
		    J = (i&16) > 0,
		    N = (i&8) > 0,
		    L = (i&4) > 0,
		    U = (i&2) > 0,
		    K = (i&1) > 0;
		gra_mvaddch (map_graph, 0, 0, Y?'#':' ');
		gra_mvaddch (map_graph, 1, 0, H?'#':' ');
		gra_mvaddch (map_graph, 2, 0, B?'#':' ');
		gra_mvaddch (map_graph, 2, 1, J?'#':' ');
		gra_mvaddch (map_graph, 2, 2, N?'#':' ');
		gra_mvaddch (map_graph, 1, 2, L?'#':' ');
		gra_mvaddch (map_graph, 0, 2, U?'#':' ');
		gra_mvaddch (map_graph, 0, 1, K?'#':' ');
		gra_mvaddch (map_graph, 1, 1, ACS_ARRAY[wall_output[i]]);
		gra_mvprint (map_graph, 3, 0, "Number %d", i);
		gr_getch ();
	}
}*/

// more restrictive; gives less away
//#define US(w) ((!sq_cansee[w])*2 + (!sq_attr[w]))
// better-looking but leaks some info about layout
#define US(w) (lvl->attr[w]?(!lvl->seen[w])*2:1)

gflags map_flags;
glyph displayed_glyph (struct DLevel *lvl, int w)
{
	map_flags = 0;
	/* draw walls */
	if (lvl->remembered[w] == ACS_WALL)
	{
		int Y = w / map_graph->w, X = w % map_graph->w;
		int h = 0, j = 0, k = 0, l = 0,
			y = 0, u = 0, b = 0, n = 0;

		if (X)
			h = US(w - 1);
		if (Y)
			k = US(w - map_graph->w);
		if (X < map_graph->w - 1)
			l = US(w + 1);
		if (Y < map_graph->h - 1)
			j = US(w + map_graph->w);
		if (X && Y)
			y = US(w - map_graph->w - 1);
		if (X < map_graph->w - 1 && Y)
			u = US(w - map_graph->w + 1);
		if (X && Y < map_graph->h - 1)
			b = US(w + map_graph->w - 1);
		if (X < map_graph->w - 1 && Y < map_graph->h - 1)
			n = US(w + map_graph->w + 1);

		/* Finally, do the actual drawing of the wall. */
		glyph output = WALL_TYPE (y, u, h, j, k, l, b, n);
		if (lvl->seen[w] == 2)
			output |= COL_TXT_BRIGHT;
		return output;
	}
	/* draw nothing */
	if (!lvl->seen[w])
		return ' ';
	/* draw what you remember */
	if (lvl->seen[w] == 1)
		return lvl->remembered[w];
	
	/* draw monster */
	struct Monster *mons = MTHIID(lvl->monsIDs[w]);
	if (mons)
	{
		map_flags = 1 |
			(1<<12) | ((1+mons->status.moving.ydir)*3 + (1+mons->status.moving.xdir))<<8 |
			(1<<17) | ((1+mons->status.attacking.ydir)*3 + (1+mons->status.attacking.xdir))<<13;
		return mons->gl;
	}

	/* draw topmost item in pile */
	if (lvl->items[w]->len > 0)
		return ((struct Item *)v_at (lvl->items[w], lvl->items[w]->len-1))->type.gl;

	/* draw dungeon feature */
	int i;
	for (i = 0; i < lvl->things[w]->len; ++ i)
	{
		struct Thing *th = v_at (lvl->things[w], i);
		switch (th->type)
		{
		case THING_DGN: ;
			struct map_item_struct *m = &th->thing.mis;
			return m->gl;
		case THING_NONE:
			printf ("%d %d %d\n", w, i, th->type);
			getchar ();
			panic ("default reached in draw_map()");
			break;
		}
	}
	return 0;
}

glyph remembered_glyph (struct DLevel *lvl, int w)
{
	/* draw nothing */
	if (!lvl->seen[w])
		return ' ';

	/* draw topmost item in pile */
	if (lvl->items[w]->len > 0)
		return ((struct Item *)v_at (lvl->items[w], lvl->items[w]->len-1))->type.gl;

	/* draw dungeon feature */
	int i;
	for (i = 0; i < lvl->things[w]->len; ++ i)
	{
		struct Thing *th = v_at (lvl->things[w], i);
		switch (th->type)
		{
		case THING_DGN: ;
			struct map_item_struct *m = &th->thing.mis;
			return m->gl == ACS_BIGDOT ? ACS_DOT : m->gl;
		case THING_NONE:
			printf ("%d %d %d\n", w, i, th->type);
			getchar ();
			panic ("default reached in draw_map()");
			break;
		}
	}
	return ' ';
}

void update_knowledge (struct Monster *player)
{
	int Y, X, w;
	int Yloc = player->yloc, Xloc = player->xloc;
	struct DLevel *level = dlv_lvl (player->dlevel);
	/* Anything you could see before you can't necessarily now */
	for (w = 0; w < map_graph->a; ++ w)
		if (level->seen[w] == 2)
			level->seen[w] = 1;

	/* This puts values on the grid -- whether or not we can see (or have seen) this square */
	// TODO draw more lines (not just starting at player) so that "tile A visible to tile B"
	// is symmetric in A and B; also want that if the player moves between two adjacent squares,
	// then the player can see (from one or other of the squares) anything they plausibly could
	// have seen while moving between them. This doesn't currently hold (exercise); would it
	// hold if we drew every line through the player, not just those starting there?
	// TODO maybe add a range limit
	for (Y = 0, w = 0; Y < map_graph->h; ++Y) for (X = 0; X < map_graph->w; ++X, ++w)
		bres_draw (Yloc, Xloc, Y, X, map_graph->w, level->seen, level->attr, NULL);

	/* draw things you can see */
	for (w = 0; w < map_graph->a; ++ w)
	{
		if (level->seen[w] == 2)
			level->remembered[w] = remembered_glyph (level, w);
	}
}

void th_init ()
{
}

void draw_map ()
{
	int w;
	for (w = 0; w < map_graph->a; ++ w)
	{
		gra_baddch (map_graph, w, displayed_glyph (cur_dlevel, w));
		map_graph->flags[w] |= map_flags;
	}
}

void draw_map_xy (struct DLevel *lvl, int y, int x)
{
	int w = map_buffer (y, x);
	draw_map_buf (lvl, w);
}

void draw_map_buf (struct DLevel *lvl, int w)
{
	gra_baddch (map_graph, w, displayed_glyph (cur_dlevel, w));
	map_graph->flags[w] |= map_flags;
}

