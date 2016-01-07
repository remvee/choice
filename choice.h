/* choice/choice.h
 *   R.W. van 't Veer, Amsterdam, 16.IV.96
 * 
 * $Id: choice.h,v 1.1 1998-07-11 22:11:21 remco Exp $ 
 */

void doerror(char *m);

/* key mapping */
#define KUP	'k'
#define KDOWN	'j'
#define KPRIOR	'\2'
#define KNEXT	'\6'
#define KTOP	'<'
#define KBOTTOM	'>'

typedef struct {
    char *label;
    enum { ITEM, DUMMY, ACTIVE } status;
} item;

extern int scroll_scope;
extern int tc_co;
