/* choice/main.c
 *   R.W. van 't Veer, Amsterdam, 17.IV.96
 * 
 * $Id: main.c,v 1.1 1998-07-11 22:11:21 remco Exp $ 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include <term.h>
#include <curses.h>
#include <termios.h>

#include "keymap.h"
#include "line.h"
#include "choice.h"

/* keymap */
struct km_struct keymap[] = {{"ku", KUP}, {"kd", KDOWN},
    {"kP", KPRIOR}, {"kN", KNEXT}, {"kh", KTOP}, {"kH", KBOTTOM},
    {NULL, '\0'}};

/* choice.c */
int dochoice(item *a[], int num, char *init);
void winsizechange(int signo);

/* globals and definitions for main */
#define USAGE	"usage: %s [-c] [-i INITITIAL] [-s SCOPE] ITEM ...\n"
char *myname;
int scroll_scope = 1;
int center = 0;
int center_co = 0;

item **items = NULL;
item *item_buf = NULL;

#define TBUFSZ	1024
char tstrbuf[TBUFSZ], tbuf[TBUFSZ], *tp = tstrbuf;
int tc_co;
char *tc_ks, *tc_ke, *tc_sactive, *tc_eactive, *tc_sdummy, *tc_edummy;

int changedtermios = 0;		/* changed terminal ioctl settings */
struct termios savedtermios;

/* reset everything and exit */
void reset_term() {
    if (tc_ks != NULL && tc_ke != NULL)
	tputs(tc_ke, 1, (outfuntype) putchar);
    if (changedtermios)
	tcsetattr(0, TCSAFLUSH, &savedtermios);
}

/* signal stuff */
void sighandler(int signo) {
    reset_term();
    exit(EXIT_FAILURE);
}

/* error handling */
void doerror(char *msg) {
    fprintf(stderr, "%s: %s\n", myname, msg);
    reset_term();
    exit(EXIT_FAILURE);
}

void dousage() {
    fprintf(stderr, USAGE, myname);
    reset_term();
    exit(EXIT_FAILURE);
}

void putitem_setup() {
    if (tgetent(tbuf, getenv("TERM")) != 1)
	doerror("no info about terminal avialable");
    if ((tc_co = tgetnum("co")) == -1) tc_co = 80;

/* get modes to show item status */
    tc_sactive = tgetstr("mr", &tp);
    tc_eactive = tgetstr("me", &tp);
    if (tc_sactive == NULL || tc_eactive == NULL) {
	tc_sactive = tgetstr("so", &tp);
	tc_eactive = tgetstr("se", &tp);
    }
    tc_sdummy = tgetstr("mh", &tp);
    tc_edummy = tgetstr("me", &tp);
    if (tc_sdummy == NULL || tc_edummy == NULL) {
	tc_sdummy = tgetstr("us", &tp);
	tc_edummy = tgetstr("ue", &tp);
    }
}

/* putitem:	IN:	arg; structure with position, label and status
		OUT:
*/
void putitem(int pos) {
    char label[tc_co];
    int c = center_co;
    int i;

    if (center && center_co > tc_co-3)
	c = tc_co-3;

    if (!*items[pos]->label)
	return;

    strncpy(label, items[pos]->label, tc_co-3);
    label[tc_co-3] = '\0';
    switch (items[pos]->status) {
	case ITEM:
	    putchar(' ');
	    break;
	case DUMMY:
	    if (tc_sdummy != NULL && tc_edummy != NULL) {
		tputs(tc_sdummy, 1, (outfuntype) putchar);
		putchar(' ');
	    } else
		putchar('-');
	    break;
	case ACTIVE:
	    if (tc_sactive != NULL && tc_eactive != NULL)  {
		tputs(tc_sactive, 1, (outfuntype) putchar);
		putchar(' ');
	    } else
		putchar('>');
	    break;
    }

    for (i = 0; center && i < ((c+1 - strlen(label))-1) / 2; i++)
	putchar(' ');

    fputs(label, stdout);

    for (i = 0; center && i < (c - strlen(label)) / 2; i++)
	putchar(' ');

    switch (items[pos]->status) {
	case ITEM:
	    putchar(' ');
	    break;
	case DUMMY:
	    if (tc_sdummy != NULL && tc_edummy != NULL) {
		putchar(' ');
		tputs(tc_edummy, 1, (outfuntype) putchar);
	    } else
		putchar('-');
	    break;
	case ACTIVE:
	    if (tc_sactive != NULL && tc_eactive != NULL)  {
		putchar(' ');
		tputs(tc_eactive, 1, (outfuntype) putchar);
	    } else
		putchar('<');
	    break;
    }
} /* putitem */

/* MAIN */
int main(int argc, char *argv[]) {
    int ret, count = 0;
    int selectable_found = 0;
    char *initial = NULL;
    int ch;

    myname = argv[0];

/* get options */
    while ((ch = getopt(argc, argv, "ci:s:")) != -1)
	switch((char) ch) {
	    case 'c':		/* center */
		center = 1;
		break;
	    case 'i':		/* initial item */
		if (initial) {
		    fprintf(stderr, "%s: multiple -i options\n", myname);
		    return EXIT_FAILURE;
		}
		initial = optarg;
		break;
	    case 's':		/* scroll scope */
		scroll_scope = atoi(optarg);
		break;
	    default:
		dousage();
	}
    argc -= optind-1;
    argv += optind-1;

/* get parameters */
    if (argc > 1) {
	if (!( items = calloc(argc, sizeof(item *)) ) ||
	  !( item_buf = calloc(argc, sizeof(item)) ))
	    doerror("unable to allocate memory");
	for (count = 0; count < argc-1; count++) {
	    items[count] = item_buf + count;
	    if (*argv[count+1] == '!' || *argv[count+1] == '#') {
		items[count]->status = DUMMY;
		items[count]->label = argv[count + 1] + 1;
	    } else {
		selectable_found++;
		items[count]->status = ITEM;
		items[count]->label = argv[count + 1];
	    }
	    if (center && strlen(items[count]->label) > center_co)
		center_co = strlen(items[count]->label);
	}
	if (selectable_found == 0)
	    doerror("no selectable items found");
    } else
	dousage();

/* set up signals */
    signal(SIGTERM, sighandler);
    signal(SIGQUIT, sighandler);
    signal(SIGINT, sighandler);

/* setup keymapping */
    if (!km_tcmap(keymap))
	doerror("keymapping failed");

/* setup line management */
    putitem_setup();
    lm_put = *putitem;
    lm_setup();

/* setup terminal io-controle */
    {
	struct termios tss;
	tcgetattr(0, &savedtermios);
	tss = savedtermios;
	tss.c_lflag |= ISIG;
	tss.c_lflag &= ~(ICANON|ECHO);
	tss.c_oflag &= ~(OCRNL|ONLRET|ONOCR);
	tss.c_cc[VMIN] = 1;
	tss.c_cc[VTIME] = 0;
	tcsetattr(0, TCSAFLUSH, &tss);
    }
    changedtermios = 1;
    signal(SIGWINCH, winsizechange);

/* enter keypadmode (if needed/possible) */
    {
	char *s = tgetstr("ks", &tp), *e = tgetstr("ke", &tp);
	if (s != NULL && e != NULL) {
	    tc_ks = strdup(s);
	    tc_ke = strdup(e);
	    tputs(tc_ks, 1, (outfuntype) putchar);
	}
    }

/* do it */
    if ((ret = dochoice(items, count, initial)) != -1)
	fputs(items[ret]->label, stderr);

    reset_term();
    return EXIT_SUCCESS;
} /* main */
