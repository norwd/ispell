#ifndef FIELDS_H_INCLUDED
#define FIELDS_H_INCLUDED
/*
 * $Id: fields.h,v 1.7 2021-06-02 16:52:07-07 geoff Exp $
 *
 * $Log: fields.h,v $
 * Revision 1.7  2021-06-02 16:52:07-07  geoff
 * Document that FLD_NOSHRINK is no longer meaningful.
 *
 * Revision 1.6  2005-04-26 15:40:07-07  geoff
 * Add double-inclusion protection.  Include ispell.h for the definition of P.
 *
 * Revision 1.5  2005/04/14 14:38:23  geoff
 * Make maxf unsigned.
 *
 * Revision 1.4  1994/01/05  20:13:43  geoff
 * Add the maxf parameter
 *
 * Revision 1.3  1994/01/04  02:40:22  geoff
 * Add field_line_inc, field_field_inc, and the FLD_NOSHRINK flag.
 *
 * Revision 1.2  1993/09/09  01:11:12  geoff
 * Add a return value to fieldwrite and support for backquotes.
 *
 * Revision 1.1  1993/08/25  21:32:05  geoff
 * Initial revision
 *
 */

/*
 * Structures used by the field-access package.
 */

#include "ispell.h"

typedef struct
    {
    unsigned int nfields;	/* Number of fields in the line */
    int		hadnl;		/* NZ if line ended with a newline */
    char *	linebuf;	/* Malloc'ed buffer containing the line */
    char **	fields;		/* Malloc'ed array of pointers to fields */
    }
		field_t;

/*
 * Flags to fieldread and fieldmake
 */
#define FLD_RUNS	0x0001	/* Consider runs of delimiters same as one */
#define FLD_SNGLQUOTES	0x0002	/* Accept single-quoted fields */
#define FLD_BACKQUOTES	0x0004	/* Accept back-quoted fields */
#define FLD_DBLQUOTES	0x0008	/* Accept double-quoted fields */
#define FLD_SHQUOTES	0x0010	/* Use shell-style (embedded) quoting rules */
#define FLD_STRIPQUOTES	0x0020	/* Strip quotes from fields */
#define FLD_BACKSLASH	0x0040	/* Process C-style backslashes */
#define FLD_NOSHRINK	0x0080	/* No longer meaningful */

#undef P
#ifdef __STDC__
#define P(x)	x
#else /* __STDC__ */
#define P(x)	()
#endif /* __STDC__ */

extern field_t *	fieldread P ((FILE * file, char * delims,
			  int flags, unsigned int maxf));
extern field_t *	fieldmake P ((char * line, int allocated,
			  char * delims, int flags, unsigned int maxf));
extern int		fieldwrite P ((FILE * file, field_t * fieldp,
			  int delim));
extern void		fieldfree P ((field_t * fieldp));

extern unsigned int	field_field_inc;
				/* Increment for expanding fields */
extern unsigned int	field_line_inc;
				/* Increment for expanding lines */

#endif /* FIELDS */
