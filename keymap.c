/* keymap.c: globals and functions for keymapping
 *	R.W. van 't Veer, Amsterdam, 1.V.96
 *
 * $Id: keymap.c,v 1.1 1998-07-11 22:11:21 remco Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <term.h>
#include <curses.h>

#include "keymap.h"

/* globals */

int km_len = 0;
struct km_struct km_data[KM_MAX];
struct timeval km_timeout = {0, 500000};	/* 1/2 second */

/* functions */

/* km_add:	IN:	s;	source string
			c;	target character
		OUT;	RET;	0 on failure or 1 on successful
*/
int km_add (char *s, const char c) {
    if(s && km_len < KM_MAX) {
	km_data[km_len].s = s;
	km_data[km_len++].c = c;
    } else
	return 0;
    return 1;
} /* km_add */

/* km_tcmap:	IN:	map;	termcap names and characters to map them to
		OUT;	RET;	0 on failure or 1 on successful
*/
int km_tcmap (const struct km_struct map[]) {
    char tstrbuf[1024], tbuf[1024], *tp = tstrbuf;

    if (tgetent(tbuf, getenv("TERM")) == 1) {
	int i;
	char *s;

	for (i = 0; map[i].s != NULL; i++) {
	    if ((s = tgetstr(map[i].s, &tp)) != NULL) {
		if (!km_add(strdup(s), map[i].c))
		    return 0;
	    }
	}
    } else
	return 0;
    return 1;
} /* km_tcmap */

/* km_inchar:	IN:	timeout;	select timeout
		OUT:	RET;		character or '\0' (timeout)
*/
char km_inchar(const struct timeval *timeout) {
    char ch;
    fd_set read_stdin;

    struct timeval tv;		  	/* timeout no longer undefined! */
    struct timeval *tv_p = &tv;
    if (timeout) tv = *timeout; else tv_p = NULL;

    FD_ZERO(&read_stdin);
    FD_SET(STDIN_FILENO, &read_stdin);
    switch (select(1, &read_stdin, NULL, NULL, tv_p)) {
	case -1: case 0:
	    return '\0';
	default:
	    if (read (STDIN_FILENO, &ch, 1) == -1)
		return '\0';
    }
    return ch;
} /* km_inchar */

/* km_getkey:	IN:	timeout;	select timeout
		OUT:	RET;		char from mapping or last char
*/
char km_getkey(const struct timeval *timeout) {
    char keybuf[KM_MAXSIZE];
    char c, last;
    int ccnt, acnt, found;

    last = km_inchar(timeout);
    for (ccnt  = 0; ccnt < KM_MAXSIZE; ccnt++) {
	keybuf[ccnt] = last;

	for (acnt = found = 0; acnt < km_len &&
	    !(found = !strncmp(keybuf, km_data[acnt].s, ccnt + 1)); acnt++)
	    ;

	if (!found)
	    return last;
	if (km_data[acnt].s[ccnt + 1] == '\0')
	    return km_data[acnt].c;

	if ((c = km_inchar(&km_timeout)) == '\0')
	    return last;
	last = c;
    }
    return last;
} /* km_getkey */
