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

#include <malloc.h>

TID curID = 0;
TID getID ()
{
	return (++curID);
}

char ACS_ARRAY[] = {
	' ',
	ACS_VLINE,
	ACS_HLINE,
	ACS_URCORNER,
	ACS_ULCORNER,
	ACS_LRCORNER,
	ACS_LLCORNER,
	ACS_RTEE,
	ACS_LTEE,
	ACS_TTEE,
	ACS_BTEE,
	ACS_PLUS
};

int wall_output[256] = {
/* 0 */
0, 1, 0, 1, 2, 6, 2, 6, 0, 1, 0, 1, 2, 6, 2, 6,
1, 1, 1, 1, 4, 8, 4, 8, 1, 1, 1, 1, 4, 8, 4, 1,
0, 1, 0, 1, 2, 6, 2, 6, 0, 1, 0, 1, 2, 6, 2, 6,
1, 1, 1, 1, 4, 8, 4, 8, 1, 1, 1, 1, 4, 8, 4, 1,
/* 64 */
2, 5, 2, 5, 2, 10, 2, 10, 2, 5, 2, 5, 2, 10, 2, 10,
3, 7, 3, 7, 9, 11, 9, 11, 3, 7, 3, 7, 9, 11, 9, 7,
2, 5, 2, 5, 2, 10, 2, 10, 2, 5, 2, 5, 2, 10, 2, 10,
3, 7, 3, 7, 9, 11, 9, 11, 3, 7, 3, 7, 2, 10, 2, 5,
/* 128 */
0, 1, 0, 1, 2, 6, 2, 6, 0, 1, 0, 1, 2, 6, 2, 6,
1, 1, 1, 1, 4, 8, 4, 8, 1, 1, 1, 1, 4, 8, 4, 1,
0, 1, 0, 1, 2, 6, 2, 6, 0, 1, 0, 1, 2, 6, 2, 6,
1, 1, 1, 1, 4, 8, 4, 8, 1, 1, 1, 1, 4, 8, 4, 1,
/* 192 */
2, 5, 2, 5, 2, 10, 2, 2, 2, 5, 2, 5, 2, 10, 2, 2,
3, 7, 3, 7, 9, 11, 9, 9, 3, 7, 3, 7, 9, 11, 9, 3,
2, 5, 2, 5, 2, 10, 2, 2, 2, 5, 2, 5, 2, 10, 2, 2,
3, 1, 3, 1, 9, 8, 9, 4, 3, 1, 3, 1, 2, 6, 2, 0
/* 256 */
};

/* What this function does is purely cosmetic - given whether or not
 * the squares surrounding are walls or spaces, this function returns what
 * character should be displayed (corner, straight line, tee, etc). */
glyph WALL_TYPE (glyph y, glyph u,
        glyph h, glyph j, glyph k, glyph l,
                 glyph b, glyph n)
{
	int H = !(h == DOT), J = !(j == DOT), K = !(k == DOT), L = !(l == DOT),
        Y = !(y == DOT), U = !(u == DOT), B = !(b == DOT), N = !(n == DOT);
	return ACS_ARRAY[wall_output[(((((((((((((Y<<1)+H)<<1)+B)<<1)+J)<<1)+N)<<1)+L)<<1)+U)<<1)+K]];
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
		txt_mvaddch (0, 0, Y?'#':' ');
		txt_mvaddch (1, 0, H?'#':' ');
		txt_mvaddch (2, 0, B?'#':' ');
		txt_mvaddch (2, 1, J?'#':' ');
		txt_mvaddch (2, 2, N?'#':' ');
		txt_mvaddch (1, 2, L?'#':' ');
		txt_mvaddch (0, 2, U?'#':' ');
		txt_mvaddch (0, 1, K?'#':' ');
		txt_mvaddch (1, 1, ACS_ARRAY[wall_output[i]]);
		txt_mvprint (3, 0, "Number %d", i);
		gr_getch ();
	}
}*/

void thing_bmove (struct Thing *thing, int level, int num)
{
	thing_move (thing, level, num/map_graph->w, num%map_graph->w);
}

void thing_watchvec (Vector vec)
{
	int i;
	for (i = 0; i < vec->len; ++ i)
	{
		struct Thing *th = v_at (vec, i);
		*(struct Thing **) v_at (all_ids, th->ID) = th;
	}
}

void rem_mons (struct DLevel *lvl, int id)
{
	int i;
	for (i = 0; i < lvl->mons->len; ++ i)
	{
		if (*(int*) v_at (lvl->mons, i) == id)
		{
			v_rem (lvl->mons, i);
			break;
		}
	}
}

void rem_id (int id)
{
	struct Thing *th = THIID(id);
	struct DLevel *lvl = dlv_lvl (th->dlevel);
	if (th->type == THING_MONS)
		rem_mons (lvl, th->ID);
	int n = map_buffer (th->yloc, th->xloc);
	v_rptr (lvl->things[n], th);
	thing_watchvec (lvl->things[n]);
	THIID(id) = NULL;
}

void thing_move (struct Thing *thing, int new_level, int new_y, int new_x)
{
	if (thing->yloc == new_y && thing->xloc == new_x && thing->dlevel == new_level)
		return;

	struct DLevel *olv = dlv_lvl (thing->dlevel),
	              *nlv = dlv_lvl (new_level);

	int old = map_buffer (thing->yloc, thing->xloc),
	    new = map_buffer (new_y, new_x);

	if (thing->type == THING_MONS && new_level != thing->dlevel)
	{
		rem_mons (olv, thing->ID);
		v_push (nlv->mons, &thing->ID);
	}

	thing->yloc = new_y;
	thing->xloc = new_x;
	thing->dlevel = new_level;

	v_push (nlv->things[new], thing);
	v_rptr (olv->things[old], thing); 

	thing_watchvec (nlv->things[new]);
	thing_watchvec (olv->things[old]);
}

void thing_free (struct Thing *thing)
{
	if (!thing)
		return;

	switch (thing->type)
	{
		case THING_ITEM:
		{
			struct Item *i = &thing->thing.item;
			if (i->name)
				free (i->name);
			break;
		}
		case THING_MONS:
		{
			struct Monster *monst = &thing->thing.mons;
			if (monst->name && monst->name[0])
				free (monst->name);
			//if (monst->eating)
			//	free (monst->eating);
			int i;
			for (i = 0; i < MAX_ITEMS_IN_PACK; ++ i)
			{
				struct Item *item = monst->pack.items[i];
				if (item)
					free (item);
			}
			break;
		}
		default:
			break;
	}
}

int TSIZ[] = {
	0,
	sizeof (struct Item),
	sizeof (struct Monster),
	sizeof (struct map_item_struct)
};

struct Thing *new_thing (uint32_t type, struct DLevel *lvl, uint32_t y, uint32_t x, void *actual_thing)
{
	//if (type == THING_ITEM)
	//	printf ("New: %u %d %dx%d  %p\n", type, dlevel, y, x, actual_thing);
	int n = map_buffer (y, x);
	struct Thing t = {type, lvl->level, getID(), y, x, {}};

	if (t.ID != all_ids->len)
		panic ("IDs error");

	memcpy (&t.thing, actual_thing, TSIZ[type]);
	struct Thing *ret = v_push (lvl->things[n], &t);

	v_push (all_ids, ret);
	thing_watchvec (lvl->things[n]);

	if (type == THING_MONS)
		v_push (lvl->mons, &t.ID);

	return ret;
}

#define US(w) (sq_seen[w]?(sq_attr[w]?DOT:ACS_WALL):ACS_WALL)

void set_can_see (struct Thing *player, uint8_t *sq_seen, uint8_t *sq_attr, glyph *sq_unseen)
{
	int Yloc = player->yloc, Xloc = player->xloc;
	int Y, X, w;

	/* Anything you could see before you can't necessarily now */
	for (w = 0; w < map_graph->a; ++ w)
		if (sq_seen[w] == 2)
			sq_seen[w] = 1;

	/* This puts values on the grid -- whether or not we can see (or have seen) this square */
	for (w = 0; w < gr_area; ++ w)
		bres_draw (Yloc, Xloc, sq_seen, sq_attr, NULL, map_graph->cy + w / gr_w, map_graph->cx + w % gr_w);
		//sq_seen[w] = 2;

	/* Make everything we can't see dark */
	for (w = 0; w < map_graph->a; ++ w)
	{
		if (!sq_seen[w])
			gra_baddch (map_graph, w, ' ');
		if (sq_seen[w] <= 1)
			gra_bsetbox (map_graph, w, 0);
	}

	/* Do the drawing */
	for (Y = 0, w = 0; Y < map_graph->h; ++Y)
	{
		for (X = 0; X < map_graph->w; ++X, ++w)
		{
			uint32_t y = DOT, u = DOT,
            h = DOT, j = DOT, k = DOT, l = DOT,
                     b = DOT, n = DOT;

			//if (sq_seen[w] == 2 && sq_attr[w] == 0 && us[w] != ' ')
			//	us[w] |= COL_TXT_BRIGHT; /* Brighten what you can see iff it's a wall. */

			/* Replace something unseeable with what's behind it. */
			if (sq_seen[w] == 1 && sq_attr[w] == 2)
				gra_baddch (map_graph, w, sq_unseen[w]);

			if (sq_attr[w] != 0 || (map_graph->data[w] & 0xFF) == ' ')
				continue; /* Only keep going if it is a wall. */

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
			if (map_graph->data[w] & COL_TXT_BRIGHT)
				gra_baddch (map_graph, w,
				            (unsigned char) WALL_TYPE(y, u, h, j, k, l, b, n) |
				            COL_TXT_BRIGHT);
			else
				gra_baddch (map_graph, w,
				            (unsigned char) WALL_TYPE(y, u, h, j, k, l, b, n));
		}
	}
}

uint32_t *type = NULL;
void draw_map (struct Thing *player)
{
	type = realloc (type, sizeof(*type) * map_graph->a);
	memset (type, 0, sizeof(*type) * map_graph->a);

	struct DLevel *lvl = cur_dlevel;
	Vector *things = lvl->things;
	uint8_t *sq_seen = lvl->seen, *sq_attr = lvl->attr;
	glyph *sq_unseen = lvl->unseen;

	LOOP_THINGS(things, at, i)
	{
		struct Thing *th = THING(things, at, i);
		bool changed = false;
		switch (th->type)
		{
			case THING_MONS:
			{
				struct Monster *m = &th->thing.mons;
				gra_bsetbox (map_graph, at, m->boxflags);
				changed = true;
				gra_baddch (map_graph, at, all_mons[m->type].col | all_mons[m->type].ch);
				map_graph->flags[at] |= 1 | (1<<12) | ((1+m->status.moving.ydir)*3    + 1+m->status.moving.xdir)   <<8;
				map_graph->flags[at] |= 1 | (1<<17) | ((1+m->status.attacking.ydir)*3 + 1+m->status.attacking.xdir)<<13;
				if (m->name)
					if (th == player)
					{
						//map_graph->data[ati] |= COL_TXT_BRIGHT;
					}
				sq_attr[at] = 2;
				break;
			}
			case THING_ITEM:
			{
				gra_bsetbox (map_graph, at, 0);
				if (type[at] != THING_MONS)
				{
					struct Item *t = &th->thing.item;
					gra_baddch (map_graph, at, t->type.col | t->type.ch);
					changed =  true;
				}
				sq_unseen[at] = map_graph->data[at];
				sq_attr[at] = 1;
				break;
			}
			case THING_DGN:
			{
				gra_bsetbox (map_graph, at, 0);
				if (type[at] == THING_NONE)
				{
					struct map_item_struct *m = &th->thing.mis;
					gra_baddch (map_graph, at, (glyph) ((unsigned char)(m->ch)));
					sq_attr[at] = m->attr & 1;
					changed = true;
				}
				sq_unseen[at] = map_graph->data[at];
				break;
			}
			default:
			{
				printf ("%d %d %d\n", at, i, th->type);
				getchar ();
				panic ("default reached in draw_map()");
			}
		}
		if (changed)
		{
			type[at] = th->type;
		}
	}
	set_can_see (player, sq_seen, sq_attr, sq_unseen);
}
/*
int pr_type;
int pr_energy;
char *pr_name;
struct Thing *pr_from;

void projectile (struct Thing *from, char *name, int type, int strength)
{
	pr_from = from;
	pr_type = type;
	if (type)
		pr_energy = type*(strength+10-type)/3;
	if (!type)
		pr_energy = strength*2;
	pr_name = name;
}

int pr_at (struct DLevel *dlevel, int yloc, int xloc)
{
	int n = map_buffer (yloc, xloc);
	LOOP_THING(dlevel->things, n, i)
	{
		struct Thing *th = THING(dlevel->things, n, i);
		if (th->type == THING_MONS)
		{
			if (mons_prhit (pr_from, th, pr_energy))
			{
				pr_energy -= 5;
				p_msg ("The %s hits the %s!", pr_name, mons[th->thing.mons.type].name);
			}
			break;
		}
		if (th->type == THING_DGN && th->thing.mis.attr == M_OPQ)
		{
			pr_energy = 0;
			break;
		}
	}
	if (pr_type)
		-- pr_energy;
	return (pr_energy > 0);
}*/

