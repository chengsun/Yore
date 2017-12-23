#ifndef PANEL_H_INCLUDED
#define PANEL_H_INCLUDED

#include "include/all.h"
#include "include/vector.h"
#include "include/graphics.h"

#define NUM_TABS 3

#define PANEL_CLOSED  0
#define PANEL_SELF    1
#define PANEL_MSG     2
#define PANEL_OPTIONS 3

#define P_MSG_LEN 60

struct Monster;

struct P_msg
{
	uint64_t expiry;
	char msg[P_MSG_LEN];
};

void p_init    ();
void p_pane    (struct Monster *);
//void p_panel   (int);
//void p_update  ();
//void p_tab     (int);
void p_amsg    (const char *);
void p_msg     (const char *, ...);
char p_ask     (struct Monster *, const char *, const char *);
char p_lines   (Vector);

enum PanelType
{
	P_STATUS = 0,
	P_SKILLS
};

int  p_status   (struct Monster *, enum PanelType);
int  p_skills   (struct Monster *, enum PanelType);
void p_mvchoose (struct Monster *, int *, int *, const char *, const char *, int);

extern int p_width, p_height
//, p_open
;

extern Graph gpan;

#endif /* PANEL_H_INCLUDED */

