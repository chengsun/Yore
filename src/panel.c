/* panel.c */

#include "include/panel.h"
#include "include/thing.h"
#include "include/graphics.h"
#include "include/words.h"
#include "include/dlevel.h"
#include "include/drawing.h"
#include "include/skills.h"
#include "include/event.h"
#include "include/player.h"

#include <stdlib.h>
#include <stdarg.h>

int p_height, p_width;
int sb_width;
//glyph *sidebar = NULL;

Vector messages = NULL;

Graph gpan = NULL;

void p_pane (struct Monster *player)
{
	int i;
	int xpan = 0, ypan = gr_h - PANE_H;
	if (!gpan)
	{
		gpan = gra_init (p_height, p_width, ypan, xpan, p_height, p_width);
		gpan->def = COL_PANEL;
	}
	/*for (i = 0; i < p_height; ++ i)
		for (j = 0; j < p_width; ++ j)
			txt_mvaddch (ypan + i, xpan + j, ' ');*/
	gra_fbox (gpan, 0, 0, p_height-1, p_width-1, ' ');

	//int max = gr_h;

	/* TODO health bar in its own graph
	for (i = 0; i < max; ++ i)
	{
		int curHP = (player.HP_max*i)/max;
		if (curHP > player.HP)
		{
			txt_mvaddch (max - i - 1, txt_w - 1, ' ');
			continue;
		}
		int gpart = (16*i)/max, rpart = (16*(max-i))/max;
		gpart = (gpart == 0) ? 0 : gpart - 1;
		rpart = (rpart == 0) ? 0 : rpart - 1;
		txt_mvaddch (max - i - 1, txt_w - 1, COL_BG_RED(rpart) | COL_BG_GREEN(gpart) | ' ');
	} */

	/*for (i = 0; i < max; ++ i)
	{
		int curXP = ((level_sum[player.level+1] - level_sum[player.level])*i)/max;
		if (curXP + level_sum[player.level] >= player.exp || i == max-1)
			txt_mvaddch (max - i - 1, txt_w - 2, ' ');
		else
			txt_mvaddch (max - i - 1, txt_w - 2, COL_BG_BLUE(10) | ' ');
	}*/

	gra_mvprint (gpan, 1, 1, "T %llu", curtick/1000);

	gra_mvprint (gpan, 2, 1, "%s the Player", w_short (player->name, 20));
	gra_mvprint (gpan, 3, 1, "HP %d/%d", player->HP, player->HP_max);
	gra_mvprint (gpan, 4, 1, "ST %d/%d", player->ST, player->ST_max);
	gra_mvprint (gpan, 5, 1, "LV %d:%d/infinity", player->level, player->exp); // TODO?
	if (pl_focus_mode)
		gra_mvprint (gpan, 1, 110, "FOCUS MODE");
	gra_mvprint (gpan, 2, 100, "SHIELD Y:%d X:%d", player->status.defending.ydir, player->status.defending.xdir);
	if (player->status.charging)
		gra_mvprint (gpan, 3, 110, "CHARGING");
	/*char *rank = get_rank ();
	int rlen = strlen (rank);
	txt_mvprint (ypan + 1, xpan + p_width - 1 - rlen, rank);*/

	if (sb_width == 0)
		goto skip1;
	/*for (i = 0; i < map_graph->vh; ++ i)
	{
		txt_mvaddch (i, map_graph->vw, ACS_VLINE);
		for (j = map_graph->vw + 1; j < txt_w - 1; ++ j)
		{
			txt_mvaddch (i, j, sidebar[sb_buffer(i, j-map_graph->vw)]);
		}
	}*/
skip1:
	if (messages == NULL)
		goto skip2;
	for (i = 0; i < 8 && i < messages->len; ++ i)
	{
		struct P_msg *msg = v_at (messages, messages->len - i - 1);
		int len = strlen (msg->msg);
		gra_mvprint (gpan, 1 + i, (gr_w - len)/2, msg->msg);
	}
skip2:
	;
}

void p_init ()
{
	if (!messages)
		messages = v_dinit (sizeof(struct P_msg));

	p_height = PANE_H;
	p_width = map_graph->vw;
}

void p_amsg (const char *str)
{
	struct P_msg msg;
	msg.expiry = 0;
	strncpy (msg.msg, str, P_MSG_LEN-1);
	msg.msg[P_MSG_LEN-1] = 0;
	v_push (messages, &msg);
}

void p_msg (const char *str, ...)
{
	va_list args;
	char out[100];

	snprintf(out, 10, "(%llu) ", curtick/1000);

	va_start (args, str);
	vsnprintf (out + strlen(out), 90, str, args);
	va_end (args);

	p_amsg (out);
}

char p_ask (struct Monster *player, const char *results, const char *question)
{
	p_amsg (question);
	if (player)
		p_pane (player);
	char in;
	do
		in = (char)gr_getch();
	while (!strchr(results, in));
	return in;
}

char p_lines (Vector lines)
{
	int i;
	int max = 0;
	for (i = 0; i < lines->len; ++ i)
	{
		int len = strlen(v_at(lines, i));
		if (len > max)
			max = len;
	}

	int h = lines->len + 2, w = max+4;
	int yloc = (gr_h - h)/2, xloc = (gr_w - w)/2;

	Graph box = gra_init (h, w, yloc, xloc, h, w);
	gra_fbox (box, 0, 0, h-1, w-1, ' ');

	for (i = 0; i < lines->len; ++ i)
	{
		char *str = v_at(lines, i);
		if (str[0] != '#')
			gra_mvaprint (box, i+1, 2, str);
		else
			gra_cprint (box, i+1, str+1);
	}
	gra_box (box, 0, 0, h-1, w-1);
	char ret = gr_getch();

	gra_free (box);

	return ret;
}

/* Status screen */
Graph sc_status = NULL;
int p_status (struct Monster *player, enum PanelType type)
{
	int h = 20, w = 81;
	int y = (gr_h - h)/2 - 10, x = (gr_w - w)/2;
	sc_status = gra_init (h, w, y, x, h, w);
	sc_status->def = COL_STATUS;
	gra_fbox (sc_status, 0, 0, h-1, w-1, ' ');
	gra_cprint (sc_status, 2, "STATUS SCREEN");
	gra_mvprint (sc_status, 0, w-5, "(Esc)");
	gra_cprint (sc_status, 4, "Name: %12s  %c  Race:  %s       ", player->name, ACS_VLINE, "Human");
	gra_cprint (sc_status, 5, "  HP: %d/%d  %c  ST:    %d/%d",
				player->HP, player->HP_max, ACS_VLINE, player->ST, player->ST_max);
	gra_cprint (sc_status, 6, "LV %d:%d/infinity   %c  Speed: %d       ", player->level, player->exp, ACS_VLINE, player->speed);
	gra_cprint (sc_status, 7, "Str: %d", player->str);
	gra_cprint (sc_status, 8, "Con: %d", player->con);
	gra_cprint (sc_status, 9, "Agi: %d", player->agi);
	gra_cprint (sc_status, 10, "Dex: %d", player->dex);
	gra_cprint (sc_status, 11, "Press 'S' to save.");

	switch (type)
	{
		int ret;
	case P_STATUS:
		break;
	case P_SKILLS:
		ret = p_skills (player, type);
		if (ret > 0)
		{
			gra_free (sc_status);
			return ret;
		}
		else if (ret == -1)
		{
			gra_free (sc_status);
			return 0;
		}
		break;
	}

	while (1)
	{
		char in = gr_getch ();
		if (in == CH_ESC || in == ' ')
			break;
		else if (in == 's')
		{
			int ret = p_skills (player, type);
			if (ret > 0)
			{
				gra_free (sc_status);
				return ret;
			}
			else if (ret == -1)
			{
				gra_free (sc_status);
				return 0;
			}
		}
		else if (in == 'S')
		{
			U.playing = PLAYER_SAVEGAME;
			return -1;
		}
	}
	gra_free (sc_status);
	return 0;
}

/* Skills screen */
Graph sc_skills = NULL;
int p_skills (struct Monster *player, enum PanelType type)
{
	int h = 10, w = 41;
	int y = (gr_h - h)/2, x = (gr_w - w)/2;
	Vector pskills = player->skills;

	sc_skills = gra_init (h, w, y, x, h, w);
	sc_skills->def = COL_SKILLS;

	gra_fbox (sc_skills, 0, 0, h-1, w-1, ' ');
	gra_mvprint (sc_skills, 0, w-7, "(Space)");
	gra_cprint (sc_skills, 2, "SKILLS");
	if (pskills->len == 0)
		gra_cprint (sc_skills, 3, "(no skills available)");
	else
		gra_cprint (sc_skills, 3, "(enter to inspect; letter to use)");

	int i, selected = 0;
	int j;
	while (1)
	{
		char letter = 'a';
		for (i = 0; i < pskills->len; ++ i)
		{
			Skill sk = v_at (pskills, i);
			int row = 4+i;
			if (sk_isact (sk))
				gra_cprint (sc_skills, row, "%c - %s %d:%d", letter, sk_name(sk), sk->level, sk->exp);
			else
				gra_cprint (sc_skills, row, "%s %d:%d", sk_name(sk), sk->level, sk->exp);
			++ letter;
		}
		if (pskills->len) for (j = 1; j < w-1; ++ j)
			gra_invert (sc_skills, 4+selected, j);
		char in = gr_getch();
		if (pskills->len) for (j = 1; j < w-1; ++ j)
			gra_invert (sc_skills, 4+selected, j);
		if (in == CH_ESC)
		{
			gra_free (sc_skills);
			return -1;
		}
		else if (in == ' ')
		{
			gra_free (sc_skills);
			return 0;
		}
		else if (pskills->len == 0)
			continue;
		else if (in == GRK_UP && selected > 0)
			-- selected;
		else if (in == GRK_DN && selected < pskills->len-1)
			++ selected;
		else if (in == CH_LF || in == CH_CR)
		{
			Skill sk = v_at (pskills, selected);
			Vector lines = w_lines (sk_desc(sk), MAX_BOX_LENGTH);
			p_lines (lines);
			v_free (lines);
		}
		else if (in >= 'a' && in < letter)
		{
			int n = in - 'a';
			int yloc, xloc;
			Skill sk = v_at (pskills, n);
			switch (sk->type)
			{
			case SK_NONE:
				break;
			case SK_CHARGE:
				gra_hide (sc_status);
				gra_hide (sc_skills);
				if (player->status.charging)
				{
					gra_free (sc_skills);
					sk_charge (player, 0, 0, v_at (pskills, selected));
					return 1;
				}
				p_mvchoose (player, &yloc, &xloc, "Charge where?", NULL, 1);
				if (yloc == -1)
				{
					gra_show (sc_status);
					gra_show (sc_skills);
					continue;
				}
				gra_free (sc_skills);
				sk_charge (player, yloc, xloc, v_at (pskills, selected));
				//mons_cont (player, &pl_charge_action, NULL);
				return 1;
			case SK_DODGE:
				break;
			case SK_FLAMES:
				gra_free (sc_skills);
				sk_flames (player);
				return 1;
			case SK_NUM:
				break;
			}
		}
	}
	gra_free (sc_skills);
	return 0;
}

Graph overlay = NULL;
int path_hit (struct DLevel *dlevel, int y, int x)
{
	gra_mvaddch (overlay, y-map_graph->cy, x-map_graph->cx, ' '|COL_BG_RED(8));
	//gra_highlight (map_graph, y, x);
	return 1;
}

void p_mvchoose (struct Monster *player, int *yloc, int *xloc, const char *instruct, const char *confirm, int showpath)
{
	int orig_y = map_graph->csr_y, orig_x = map_graph->csr_x;
	if (showpath)
	{
		overlay = gra_init (gr_h, gr_w, 0, 0, gr_h, gr_w);
		gra_clear (overlay);
	}
	if (instruct)
		p_msg (instruct);
	p_pane (player);
	gr_refresh ();
	int xmove, ymove;
	char key = gr_getch ();
	p_move (&ymove, &xmove, key);
	while (key != '.' || (confirm && (p_ask (player, "yn", confirm) != 'y')))
	{
		if (key == CH_ESC)
		{
			*yloc = -1;
			gra_cmove (map_graph, orig_y, orig_x);
			gra_free (overlay);
			return;
		}
		if (ymove == -1 && map_graph->csr_y > 0 && map_graph->csr_y - map_graph->cy > 0)
			gra_cmove (map_graph, map_graph->csr_y-1, map_graph->csr_x);
		else if (ymove == 1 && map_graph->csr_y < map_graph->h-1 && map_graph->csr_y - map_graph->cy < map_graph->vh-1)
			gra_cmove (map_graph, map_graph->csr_y+1, map_graph->csr_x);
		if (xmove == -1 && map_graph->csr_x > 0 && map_graph->csr_x - map_graph->cx > 0)
			gra_cmove (map_graph, map_graph->csr_y, map_graph->csr_x-1);
		else if (xmove == 1 && map_graph->csr_x < map_graph->w-1 && map_graph->csr_x - map_graph->cx < map_graph->vw-1)
			gra_cmove (map_graph, map_graph->csr_y, map_graph->csr_x+1);
		//if (gra_nearedge (map_graph, map_graph->cy + csr_y, map_graph->cx + csr_x))
		//	gra_centcam (map_graph, map_graph->cy + csr_y, map_graph->cx + csr_x);
		if (showpath)
		{
			gra_clear (overlay);
			bres_draw (player->yloc, player->xloc, NULL, dlv_attr(player->dlevel), &path_hit, map_graph->csr_y, map_graph->csr_x);
		}
		key = gr_getch ();
		p_move (&ymove, &xmove, key);
	}
	*yloc = map_graph->csr_y;
	*xloc = map_graph->csr_x;
	gra_cmove (map_graph, orig_y, orig_x);
	gra_free (overlay);
}

