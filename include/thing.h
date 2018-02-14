#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include "include/vector.h"
#include "include/graphics.h"
#include "include/item.h"
#include "include/monst.h"
#include "include/map.h"

#define LOOP_THING(t,n,i)  int i;                                        for (i = 0; i < (t)[n]->len; ++ i)
#define LOOP_THINGS(t,n,i) int i, n; for (n = 0; n < map_graph->a; ++ n) for (i = 0; i < (t)[n]->len; ++ i)
#define BREAK(n)           {n = map_graph->a; break;}

#define THIID(id)          (*(struct Thing **)  v_at (all_ids, (id)))
#define ITEMID(id)         (*(struct Item **) v_at (all_ids, (id)))
#define THING(t,n,i)       ((struct Thing*)(v_at((t)[n], i)))

typedef int TID, MID;

enum THING_TYPE
{
	THING_NONE = 0,   /* not used */
	THING_DGN         /* a dungeon feature (wall, floor, trap etc) */
};

struct Thing
{
	enum THING_TYPE type;
	int dlevel;
	TID ID;
	uint32_t yloc, xloc;
	union
	{
		struct map_item_struct mis;
	}
	thing;
};

/* see dlevel.h */
struct DLevel;

struct Thing *new_thing      (uint32_t, struct DLevel *, uint32_t, uint32_t, void *);
void          thing_free     (struct Thing *);
void          rem_id         (TID);

struct Monster *new_mons     (struct DLevel *, uint32_t, uint32_t, void *);
void          monsthing_move (struct Monster *, int, int, int);
void          rem_mid        (MID);

struct Monster *MTHIID       (MID);
void          draw_map       ();
void          draw_map_xy    (struct DLevel *, int, int);
void          draw_map_buf   (struct DLevel *, int);
void          update_knowledge (struct Monster *);
void          th_init        ();

int           get_thing_type (char);
const char *  get_thing_name (struct Thing);

struct Item * item_put       (struct Item *, union ItemLoc loc);
void          item_free      (struct Item *);

//void walls_test ();

#endif /* THING_H_INCLUDED */

