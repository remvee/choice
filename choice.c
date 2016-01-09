/* choice/choice.c
 *   R.W. van 't Veer, Amsterdam, 17.IV.96
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <signal.h>

#include "keymap.h"
#include "line.h"
#include "choice.h"

int active, items_num;

void winsizechange(int signo) {
    struct winsize winsize;

    ioctl(1, TIOCGWINSZ, &winsize);
    tc_co = winsize.ws_col;
    lm_lines = winsize.ws_row - 1;
    lm_end();
    lm_start_scope(active, scroll_scope, items_num);
    signal(SIGWINCH, winsizechange);
}

/* dochoice:	IN:	items;	array of menu items
			num;	number of menu items
			init;	initial item
		OUT:	return;	index of selected item or -1
*/
int dochoice(item *items[], int num, char *init)
{
    int i, move, direction;
    int newpos; 
    char ch;

/* for SIGWINCH */
    items_num = num;

/* get and select initial/first */
    for (active = 0; active < num &&
	items[active]->status != ITEM; active++) ;
    if (init) {
	for (i = active; i < num; i++)
	    if (items[i]->status == ITEM &&
		!strncmp(init, items[i]->label, strlen(init))) break;
	if (i != num)
	    active = i;
    }

    items[active]->status = ACTIVE;
    lm_start_scope(active, scroll_scope, num);

/* interaction loop */
    while(1) {
	switch (ch = km_getkey(NULL)) {
	    case KUP: move = direction = -1; break;
	    case KDOWN: move = direction = 1; break;
	    case KPRIOR: move = -lm_lines-1; direction = -1; break;
	    case KNEXT: move = lm_lines+1; direction = 1; break;
	    case KTOP: move = -active; direction = 1; break;
	    case KBOTTOM: move = num-active; direction = -1; break;
	    case '\e': case 'q':
		active = -1;
	    case '\r': case '\n':
		lm_end();
		return active;
	    case '\14':		/* Ctrl L */
		lm_refresh();
		lm_goto(active);
		continue;
	    case '\0':
		continue;
	    default:
		putchar('\a');
		fflush(stdout);
		continue;
	}

	items[active]->status = ITEM;
/* select next */
	newpos = active + move;
	if (newpos < 0) newpos = 0;
	if (newpos >= num) newpos = num-1;
	while (items[newpos]->status != ITEM) {
	    newpos += direction;
	    if (newpos< 0) {
		newpos= 0;
		direction = 1;
	    }
	    if (newpos>= num) {
		newpos= num-1;
		direction = -1;
	    }
	}
	if (newpos == active) {
	    putchar('\a');
	    fflush(stdout);
	    continue;
	}

/* unselect */
	lm_goto(active);
/* scroll scope */
	if (newpos+scroll_scope >= num)
	    lm_goto(num-1);
	else if (newpos-scroll_scope < 0)
	    lm_goto(0);
	else if (newpos+scroll_scope > lm_bottom)
	    lm_goto(newpos+scroll_scope);
	else if (newpos-scroll_scope < lm_top)
	    lm_goto(newpos-scroll_scope);
/* select */
	items[active = newpos]->status = ACTIVE;
	lm_goto(active);
    }
} /* dochoice */
