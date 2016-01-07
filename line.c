/* line.c
 *   R.W. van 't Veer, 2.VI.96, Amsterdam
 * 
 * $Id: line.c,v 1.1 1998-07-11 22:11:21 remco Exp $ 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <term.h>
#include <curses.h>

#include "line.h"

/* globals from termcap */
char *lm_up, *lm_al;
int lm_lines;

/* globals for lm_goto, lm_end, lm_update */
int lm_y = 0, lm_top = 0, lm_bottom = 0;
void (*lm_put)(int);

/* init menu */
void lm_start(int y, int num)
{
    if (y > lm_lines)
	lm_y = y - lm_lines;
    else if (num > lm_lines)
	lm_goto(lm_lines);
    else
	lm_goto(num-1);
    lm_goto(y);
}
	
/* drop menu */
void lm_end()
{
    for (; lm_y <= lm_bottom; lm_y++)
	putchar('\n');

    lm_y = lm_top = lm_bottom = 0;
}

/* refresh menu */
void lm_refresh()
{
    int y;
    for (y = lm_top; y <= lm_bottom; y++)
	lm_goto(y);
}

/* make item visible */
void lm_goto(int ypos)
{
/* beyond bottom */
    if (ypos > lm_bottom) {
	for (; lm_y < lm_bottom; lm_y++)
	    putchar('\n');
	if (ypos - lm_y > lm_lines) {
	    lm_y = ypos - lm_lines;
	    putchar('\n');
	}
	for (; lm_y < ypos; lm_y++) {
	    putchar('\r');
	    (*lm_put)(lm_y);
	    putchar('\n');
	}

	lm_bottom = lm_y;
	if (lm_bottom - lm_top > lm_lines)
	    lm_top = lm_bottom - lm_lines;
    }

/* beyond top */
    if (ypos < lm_top) {
	for (; lm_y > lm_top; lm_y--)
	    tputs(lm_up, 1, (outfuntype) putchar);
	if (lm_y - ypos > lm_lines) {
	    lm_y = ypos + lm_lines;
	    tputs(lm_al, lm_lines, (outfuntype) putchar);
	}
	for (; lm_y > ypos; lm_y--) {
	    putchar('\r');
	    (*lm_put)(lm_y);
	    tputs(lm_al, lm_lines, (outfuntype) putchar);
	}

	lm_top = lm_y;
	if (lm_bottom - lm_top > lm_lines)
	    lm_bottom = lm_top + lm_lines;
    }

/* on screen */
    if (lm_y != ypos) {
	for (; lm_y < ypos; lm_y++)
	    putchar('\n');
	for (; lm_y > ypos; lm_y--)
	    tputs(lm_up, 1, (outfuntype) putchar);
    }
    putchar('\r');
    (*lm_put)(lm_y);

    fflush(stdout);
} /* lm_goto */

void lm_setup()
{
    char tstrbuf[1024], tbuf[1024], *tp = tstrbuf; 
/* get terminal capabilities */
    if (tgetent(tbuf, getenv("TERM")) == 1) {
	if ((lm_lines = tgetnum("li")) == -1) {
	    fputs("line: termcap `li' not avialable\n", stderr);
	    exit(1);
	}
	lm_lines--;

	if (tgetstr("up", &tp) == NULL || tgetstr("al", &tp) == NULL) {
	    fputs("line: termcap `up' and/ or `al' not avialable\n", stderr);
	    exit(1);
	}
	lm_up = strdup(tgetstr("up", &tp));
	lm_al = strdup(tgetstr("al", &tp));
    } else {
	fputs("line: termcap not avialable\n", stderr);
	exit(1);
    }
} /* set_term */

