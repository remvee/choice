/* line.h
 *   R.W. van 't Veer, 2.VI.96, Amsterdam
 * 
 * $Id: line.h,v 1.1 1998-07-11 22:11:21 remco Exp $ 
 */

/* FUNCTIONS */

extern void lm_setup();
extern void lm_refresh();
extern void lm_start(int y, int n);
extern void lm_goto(int y);
extern void lm_scope (int y, int scope, int num);
extern void lm_end();

/* VARIABLES */

extern void (*lm_put)(int);
extern int lm_y, lm_bottom, lm_top, lm_lines;

/* MACROS */

#define lm_visible(y) !(y > lm_bottom || y < lm_top)
#define lm_update(y) if (lm_visible(y)) lm_goto(y)
/* make item visible, when scrolling take note of a scroll-scope */
#define lm_scope(y,scope,num) \
    if (y+scope >= num) lm_goto(num-1); \
    else if (y-scope < 0) lm_goto(0); \
    else if (y+scope > lm_bottom) lm_goto(y+scope); \
    else if (y-scope < lm_top) lm_goto(y-scope); \
    lm_goto(y)
#define lm_start_scope(y,scope,num) \
    if (y+scope >= num) lm_start(num-1, num); \
    else if (y-scope < 0) lm_start(0, num); \
    else lm_start(y+scope, num); \
    lm_goto(y)


/* "MISSING" */

typedef int (*outfuntype) (int);
