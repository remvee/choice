/* choice/choice.h
 *   R.W. van 't Veer, Amsterdam, 16.IV.96
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
