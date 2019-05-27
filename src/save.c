/* save.c */

/*#include "include/thing.h"
#include "include/save.h"
#include "include/panel.h"
#include "include/monst.h"
#include "include/vector.h"
#include "include/pack.h"
#include "include/map.h"
#include "include/graphics.h"
#include "include/dlevel.h"*/
#include "include/panel.h"
  
//#include <stdlib.h>
//#include <stdio.h>
//#include <assert.h>

//FILE *game_save_file;

/* Saves a native type (eg int, char). Not pointers. Also, multiple evaluations. */
//#define SAVE_NATIVE(n) fwrite(&(n), sizeof(n), 1, game_save_file)
/*
struct ref_it_type
{
	char type;
	uint32_t dat;
};

struct List blocks = LIST_INIT;

void store_type(ityp *ptr)
{
	struct block_ptr *bp = malloc(sizeof(*bp));
	bp->ptr = ptr;
	bp->len = len;
	push_back(&blocks, bp);
}

void save_block()
{
	struct list_iter *li;
	fwrite("1111", 4, 1, game_save_file);
	for (li = blocks.beg; iter_good(li); next_iter(&li))
	{
		struct block_ptr *bp = li->data;
		SAVE_NATIVE(bp->ptr);
		fwrite(bp->ptr, bp->len, 1, game_save_file);
	}
}*/
/*
void save_item (struct Item *item)
{
	void *p = NULL;
	//if (!item)
	{
		SAVE_NATIVE(p);
		return;
	}
	//store_type(item->type);
	SAVE_NATIVE(item->type);
	SAVE_NATIVE(item->attr);
	SAVE_NATIVE(item->cur_weight);

	if (!item->name)
		fwrite("\0", 1, 1, game_save_file);
	else
		fwrite(item->name, strlen(item->name)+1, 1, game_save_file);
}*/

/* The three main things needing saving are: the List all_things, the uint64_t Time, and
 * the player_struct U. There is also the list of things you have seen.
 * This function saves in the following order: Time; U; all_things; sq_attr. */
int save (char *filename)
{
	//printf("See you soon...\n");
	/*int i;
	if (p_ask ("yn", "Save and quit?") == 'y')
	{
		p_msg("Saving...");
		game_save_file = fopen(filename, "wb");
		fwrite("YOREv"YORE_VERSION, sizeof("YOREv"YORE_VERSION), 1, game_save_file);

		/ * Time * /
		SAVE_NATIVE(Time);

		/ * U * /
		SAVE_NATIVE(U.hunger);
		SAVE_NATIVE(U.role);
		SAVE_NATIVE(U.playing);
		for (i = 0; i < 6; ++ i)
			SAVE_NATIVE(U.attr[i]);
		SAVE_NATIVE(U.luck);
		SAVE_NATIVE(U.m_glflags);
		SAVE_NATIVE(U.magic);
		
		/ * all_things * /
		LOOP_THINGS((char*)(0xdeadbeef), n, i)
		{
			struct Thing *th = THING((char*)(0xdeadbeef), n, i);
			int enum_saver = (int)(th->type);
			assert(enum_saver);
			SAVE_NATIVE(enum_saver);
			SAVE_NATIVE(th->yloc);
			SAVE_NATIVE(th->xloc);
			switch (th->type)
			{
				case THING_ITEM:
				{
					save_item (&th->thing.item);
					break;
				}
				case THING_MONS:
				{
					struct Monster *mn = &th->thing.mons;
					SAVE_NATIVE(mn->type);
					SAVE_NATIVE(mn->level);
					SAVE_NATIVE(mn->exp);
					SAVE_NATIVE(mn->HP);
					SAVE_NATIVE(mn->HP_max);
					SAVE_NATIVE(mn->cur_speed);

					if (mn->name)
						fwrite (mn->name, 20, 1, game_save_file);
					else
						fwrite ("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 20, 1, game_save_file);

					/ * pack * /
					for (i = 0; i < MAX_ITEMS_IN_PACK; ++ i)
						save_item (mn->pack.items[i]);

					/ * wearing * /
					save_item (mn->wearing.head);
					save_item (mn->wearing.torso);
					save_item (mn->wearing.legs);
					save_item (mn->wearing.feet);
					save_item (mn->wearing.hands);
					save_item (mn->wearing.arms);
					save_item (mn->wearing.rfin);
					save_item (mn->wearing.lfin);
					save_item (mn->wearing.rweap);
					save_item (mn->wearing.lweap);
					SAVE_NATIVE(mn->wearing.two_weaponing);

					SAVE_NATIVE(mn->status);
					save_item (mn->eating);
					break;
				}
				case THING_DGN:
				{
					struct map_item_struct *mapit = &th->thing.mis;
					long type = GETMAPITEMonsID(mapit->ch);
					SAVE_NATIVE (type);
				}
				default:
				{
				}
			}
		}
		fprintf (game_save_file, "1111");

		for (i = 0; i < 1680; ++ i) fprintf(game_save_file, "%c", sq_seen[i]);
		
		//save_block();
		
		fclose (game_save_file);
		return 0;
	}*/
	return 1;
}

#define LOAD_NATIVE(n) fread(&(n), sizeof(n), 1, game_save_file)
/*
struct List load_blocks = LIST_INIT;

void *store_load_block(void **mem, int len)
{
	struct block_ptr *bp = malloc(sizeof(*bp));
	bp->ptr = mem;
	bp->len = len;
	push_back(&blocks, bp);
	return *mem;
}

void load_block()
{
}*/
/*
void load_item (struct Item *item)
{
	void *ptr;
	LOAD_NATIVE(ptr);
	if (!(ptr&255))
	{
		memclr (item, sizeof(*item));
		return;
	}
	memcpy (&item->type, &items[0], sizeof(ityp));
	//store_load_block(&item->type, ptr, sizeof(ityp));
	LOAD_NATIVE(item->attr);
	LOAD_NATIVE(item->cur_weight);
	char temp[100];
	fgets(temp, 100, game_save_file);
	int len = strlen(temp);
	if (len < 2) item->name = NULL;
	else
	{
		item->name = malloc(len+1);
		strcpy(item->name, temp); safe (commented)
	}
}

struct Item *make_item ()
{
}

void restore (char *filename)
{
	int i;
	U.playing = PLAYER_ERROR; * for premature returning *
	game_save_file = fopen (filename, "rb");
	if (!game_save_file) panic ("Save file nonexistent\n");
	
	char *ftest = malloc (strlen("YOREv"YORE_VERSION))+1;

	fread (ftest, strlen("YOREv"YORE_VERSION) + 1, 1, game_save_file);
	if (strcmp (ftest, "YOREv"YORE_VERSION))
		panic ("Save file invalid\n");

	LOAD_NATIVE(Time);

	LOAD_NATIVE(U.hunger);
	player = NULL;
	LOAD_NATIVE(U.role);
	LOAD_NATIVE(U.playing);
	for (i = 0; i < 6; ++ i)
		LOAD_NATIVE(U.attr[i]);
	LOAD_NATIVE(U.luck);
	LOAD_NATIVE(U.m_glflags);
	LOAD_NATIVE(U.magic);

	while (1)
	{
		struct Thing th;

		LOAD_NATIVE(th.type);
		if (th.type == 0x31313131)
			break;

		if (th.type > 6)
			panic ("Load file is messed up.");

		LOAD_NATIVE(th.yloc);
		LOAD_NATIVE(th.xloc);
		
		switch (th.type)
		{
			case THING_ITEM:
			{
				load_item (&th.thing.item);
				break;
			}
			case THING_MONS:
			{
				struct Monster *mn = &th.thing.mons;

				LOAD_NATIVE(mn->type);
				LOAD_NATIVE(mn->level);
				LOAD_NATIVE(mn->exp);
				LOAD_NATIVE(mn->HP);
				LOAD_NATIVE(mn->HP_max);
				LOAD_NATIVE(mn->cur_speed);
				mn->name = malloc (20);
				fread (mn->name, 20, 1, game_save_file);

				for (i = 0; i < MAX_ITEMS_IN_PACK; ++ i)
					load_item (&(mn->pack.items[i]));

				load_item (&mn->wearing.head);
				load_item (&mn->wearing.torso);
				load_item (&mn->wearing.legs);
				load_item (&mn->wearing.feet);
				load_item (&mn->wearing.hands);
				load_item (&mn->wearing.arms);
				load_item (&mn->wearing.rfin);
				load_item (&mn->wearing.lfin);
				load_item (&mn->wearing.rweap);
				load_item (&mn->wearing.lweap);
				LOAD_NATIVE(mn->wearing.two_weaponing);

				LOAD_NATIVE(mn->status);
				load_item (&mn->eating);
				break;
			}
			case THING_DGN:
			{
				long num;
				LOAD_NATIVE(num);
				memcpy (&th.thing.mis, &map_items[num], sizeof(*map_items));
				break;
			}
			default:
			{
			}
		}

		if (!player)
			player = new_thing (th.type, cur_dlevel, th.yloc, th.xloc, &th.thing);
		else
			new_thing (th.type, cur_dlevel, th.yloc, th.xloc, &th.thing);
	}

	for (i = 0; i < 1680; ++ i) LOAD_NATIVE(sq_seen[i]);

	//load_block();
	
	fclose (game_save_file);

	fclose (game_save_file);

	U.playing = PLAYER_PLAYING;	* success *
}*/

/* 0 is quit, 1 is stay */
int quit ()
{
	if (p_ask (0, "yn", "Are you sure you want to quit?") == 'y')
	{
	//	printf ("Quitting...\n");
		//destroy_save_file (get_filename());
		return 0;
	}
	return 1;
}

#if defined(__WIN32__)
#  define SH_RM "del"
#else
#  define SH_RM "rm"
#endif

void destroy_save_file (char *filename)
{
	/*char str[1000];

    FILE *file = fopen (filename, "wb+");
    fwrite ("", 1, 1, file);
    fclose (file);

	snprintf (str, 1000, "%s %s", SH_RM, filename);
	system (str);*/
}
