/* keymap.h: defines for keymapping
 *   R.W. van 't Veer, Amsterdam, 1.V.96
 * 
 * $Id: keymap.h,v 1.1 1998-07-11 22:11:21 remco Exp $ 
 */

#include <sys/time.h>

/* defines */
#define KM_MAX 20
#define KM_MAXSIZE 20
struct km_struct { char *s; char c; };

/* globals */
extern int km_len;
extern struct km_struct km_data[];
extern struct timeval km_timeout;

/* functions */
extern char km_getkey(const struct timeval *timeout);
extern int km_add(char *s, const char c);
extern int km_tcmap(const struct km_struct map[]);
