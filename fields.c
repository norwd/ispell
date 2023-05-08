#ifndef lint
static char Rcs_Id[] =
    "$Id: fields.c,v 1.13 2021-06-02 16:52:07-07 geoff Exp $";
#endif

/*
 * $Log: fields.c,v $
 * Revision 1.13  2021-06-02 16:52:07-07  geoff
 * When maxf is nonzero, don't truncate the last field.  Get rid of
 * O(N^2) performance when expanding buffers.  Remove FLD_NOSHRINK.
 *
 * Revision 1.12  2021-05-29 22:53:37-07  geoff
 * Correctly handle quoted strings in the last field on a line.
 *
 * Revision 1.11  2005-04-14 07:38:23-07  geoff
 * Make maxf unsigned.
 *
 * Revision 1.10  1999/01/06 20:57:19  geoff
 * Include ispell.h so proto.h will work
 *
 * Revision 1.9  1999/01/05  20:46:38  geoff
 * Get declarations from proto.h
 *
 * Revision 1.8  1998/07/06  04:56:19  geoff
 * Make strlen return an unsigned int
 *
 * Revision 1.7  1994/01/06  05:26:37  geoff
 * Get rid of all references to System V string routines, for portability
 * (sigh).
 *
 * Revision 1.6  1994/01/05  20:13:43  geoff
 * Add the maxf parameter
 *
 * Revision 1.5  1994/01/04  02:40:21  geoff
 * Make the increments settable (field_line_inc and field_field_inc).
 * Add support for the FLD_NOSHRINK flag.
 *
 * Revision 1.4  1993/09/27  17:48:02  geoff
 * Fix some lint complaints and some parenthesization errors.
 *
 * Revision 1.3  1993/09/09  01:11:11  geoff
 * Add a return value to fieldwrite.  Add support for backquotes and for
 * unstripped backslashes.
 *
 * Revision 1.2  1993/08/26  00:02:50  geoff
 * Fix a stupid null-pointer bug
 *
 * Revision 1.1  1993/08/25  21:32:05  geoff
 * Initial revision
 *
 */

#include <stdio.h>
#include "config.h"
#include "fields.h"
#include "ispell.h"
#include "proto.h"

field_t *	fieldread P ((FILE * file, char * delims,
				  int flags, unsigned int maxf));
				/* Read a line with fields from a file */
field_t *	fieldmake P ((char * line, int allocated, char * delims,
				  int flags, unsigned int maxf));
				/* Make a field structure from a line */
static field_t * fieldparse P ((field_t * fieldp, char * line, char * delims,
				  int flags, unsigned int maxf));
				/* Parse the fields in a line */
static int	fieldbackch P ((char * str, char ** out, int strip));
				/* Process backslash sequences */
int		fieldwrite P ((FILE * file, field_t * fieldp, int delim));
				/* Write a line with fields to a file */
void		fieldfree P ((field_t * fieldp));
				/* Free a field returned by fieldread */

#define INIT_FIELDS     8       /* Number of fields to allocate initially */
#define INIT_LINE       128     /* Initial size of a line in fieldread */

#ifndef USG
#define strchr	index
#endif /* USG */

/*
 * Read one line of the given file into a buffer, break it up into
 * fields, and return them to the caller.  The field_t structure
 * returned must eventually be freed with fieldfree.
 */
field_t * fieldread (file, delims, flags, maxf)
    FILE *		file;	/* File to read lines from */
    char *		delims;	/* Characters to use for field delimiters */
    int			flags;	/* Option flags;  see fields.h */
    unsigned int	maxf;	/* Maximum number of fields to parse */
    {
    register char *	linebuf; /* Buffer to hold the line read in */
    int			linemax; /* Maximum line buffer size */
    int			linesize; /* Current line buffer size */

    linebuf = (char *) malloc (INIT_LINE);
    if (linebuf == NULL)
	return NULL;
    linemax = INIT_LINE;
    linesize = 0;
    /*
     * Read in the line.
     */
    while (fgets (&linebuf[linesize], linemax - linesize, file)
      != NULL)
	{
	linesize += strlen (&linebuf[linesize]);
	if (linebuf[linesize - 1] == '\n')
	    break;
	else
	    {
	    linemax *= 2;
	    linebuf = (char *) realloc (linebuf, linemax);
	    if (linebuf == NULL)
		return NULL;
	    }
	}
    if (linesize == 0)
	{
	free (linebuf);
	return NULL;
	}
    return fieldmake (linebuf, 1, delims, flags, maxf);
    }

field_t * fieldmake (line, allocated, delims, flags, maxf)
    char *		line;	/* Line to make into a field structure */
    int			allocated; /* NZ if line allocated with malloc */
    char *		delims;	/* Characters to use for field delimiters */
    int			flags;	/* Option flags;  see fields.h */
    unsigned int	maxf;	/* Maximum number of fields to parse */
    {
    register field_t *	fieldp;	/* Structure describing the fields */
    int			linesize; /* Current line buffer size */

    fieldp = (field_t *) malloc (sizeof (field_t));
    if (fieldp == NULL)
	return NULL;
    fieldp->nfields = 0;
    fieldp->linebuf = allocated ? line : NULL;
    fieldp->fields = NULL;
    fieldp->hadnl = 0;
    linesize = strlen (line);
    if (line[linesize - 1] == '\n')
	{
	line[--linesize] = '\0';
	fieldp->hadnl = 1;
	}
    return fieldparse (fieldp, line, delims, flags, maxf);
    }

static field_t * fieldparse (fieldp, line, delims, flags, maxf)
    register field_t *	fieldp;	/* Field structure to parse into */
    register char *	line;	/* Line to be parsed */
    char *		delims;	/* Characters to use for field delimiters */
    int			flags;	/* Option flags;  see fields.h */
    unsigned int	maxf;	/* Maximum number of fields to parse */
    {
    unsigned int	fieldmax; /* Max size of fields array */
    char *		lineout; /* Where to store xlated char in line */
    char		quote;	/* Quote character in use */

    fieldp->nfields = 0;
    fieldmax =
      (maxf != 0  &&  maxf < INIT_FIELDS) ? maxf + 2 : INIT_FIELDS;
    fieldp->fields = (char **) malloc (fieldmax * sizeof (char *));
    if (fieldp->fields == NULL)
	{
	fieldfree (fieldp);
	return NULL;
	}
    if ((flags
	& (FLD_SHQUOTES | FLD_SNGLQUOTES | FLD_BACKQUOTES | FLD_DBLQUOTES))
      == FLD_SHQUOTES)
	flags |= FLD_SNGLQUOTES | FLD_BACKQUOTES | FLD_DBLQUOTES;
    while (1)
	{
	if (flags & FLD_RUNS)
	    {
	    while (*line != '\0'  &&  strchr (delims, *line) != NULL)
		line++;			/* Skip runs of delimiters */
	    if (*line == '\0')
		break;
	    }
	fieldp->fields[fieldp->nfields] = lineout = line;
	/*
	 * Skip to the next delimiter.  At the end of skipping, "line" will
	 * point to either a delimiter or a null byte.
	 */
	if (flags
	  & (FLD_SHQUOTES | FLD_SNGLQUOTES | FLD_BACKQUOTES
	    | FLD_DBLQUOTES | FLD_BACKSLASH))
	    {
	    while (*line != '\0')
		{
		if (strchr (delims, *line) != NULL)
		    break;
		else if (((flags & FLD_SNGLQUOTES)  &&  *line == '\'')
		  ||  ((flags & FLD_BACKQUOTES)  &&  *line == '`')
		  ||  ((flags & FLD_DBLQUOTES)  &&  *line == '"'))
		    {
		    if ((flags & FLD_SHQUOTES) == 0
		      &&  line != fieldp->fields[fieldp->nfields])
			quote = '\0';
		    else
			quote = *line;
		    }
		else
		    quote = '\0';
		if (quote == '\0')
		    {
		    if (*line == '\\'  &&  (flags & FLD_BACKSLASH))
			{
			line++;
			if (*line == '\0')
			    break;
			line += fieldbackch (line, &lineout,
			  flags & FLD_STRIPQUOTES);
			}
		    else
			*lineout++ = *line++;
		    }
		else
		    {
		    /* Process quoted string */
		    if ((flags & FLD_STRIPQUOTES) == 0)
			*lineout++ = quote;
		    ++line;
		    while (*line != '\0')
			{
			if (*line == quote)
			    {
			    if ((flags & FLD_STRIPQUOTES) == 0)
				*lineout++ = quote;
			    line++;		/* Go on past quote */
			    if ((flags & FLD_SHQUOTES) == 0)
				{
				while (*line != '\0'
				  &&  strchr (delims, *line) == NULL)
				    line++;	/* Skip to delimiter */
				}
			    break;
			    }
			else if (*line == '\\')
			    {
			    if (flags & FLD_BACKSLASH)
				{
				line++;
				if (*line == '\0')
				    break;
				else
				    line += fieldbackch (line, &lineout,
				      flags & FLD_STRIPQUOTES);
				}
			    else
				{
				*lineout++ = '\\';
				if (*++line == '\0')
				    break;
				*lineout++ = *line;
				}
			    }
			else
			    *lineout++ = *line++;
			}
		    }
		}
	    }
	else
	    {
	    while (*line != '\0'  &&  strchr (delims, *line) == NULL)
		line++;			/* Skip to delimiter */
	    lineout = line;
	    }
	fieldp->nfields++;
	if (*line++ == '\0')
	    {
	    *lineout = '\0';
	    break;
	    }
	if (maxf != 0  &&  fieldp->nfields > maxf)
	    break;
	*lineout = '\0';
	if (fieldp->nfields >= fieldmax)
	    {
	    fieldmax *= 2;
	    fieldp->fields =
	      (char **) realloc (fieldp->fields, fieldmax * sizeof (char *));
	    if (fieldp->fields == NULL)
		{
		fieldfree (fieldp);
		return NULL;
		}
	    }
	}
    fieldp->fields[fieldp->nfields] = NULL;
    return fieldp;
    }

static int fieldbackch (str, out, strip)
    register char *	str;		/* First char of backslash sequence */
    register char **	out;		/* Where to store result */
    int			strip;		/* NZ to convert the sequence */
    {
    register int	ch;		/* Character being developed */
    char *		origstr;	/* Original value of str */

    if (!strip)
	{
	*(*out)++ = '\\';
	if (*str != 'x'  &&  *str != 'X'  &&  (*str < '0'  ||  *str > '7'))
	    {
	    *(*out)++ = *str;
	    return *str != '\0';
	    }
	}
    switch (*str)
	{
	case '\0':
	    *(*out)++ = '\0';
	    return 0;
	case 'a':
	    *(*out)++ = '\007';
	    return 1;
	case 'b':
	    *(*out)++ = '\b';
	    return 1;
	case 'f':
	    *(*out)++ = '\f';
	    return 1;
	case 'n':
	    *(*out)++ = '\n';
	    return 1;
	case 'r':
	    *(*out)++ = '\r';
	    return 1;
	case 'v':
	    *(*out)++ = '\v';
	    return 1;
	case 'X':
	case 'x':
	    /* Hexadecimal sequence */
	    origstr = str++;
	    ch = 0;
	    if (*str >= '0'  &&  *str <= '9')
		ch = *str++ - '0';
	    else if (*str >= 'a'  &&  *str <= 'f')
		ch = *str++ - 'a' + 0xa;
	    else if (*str >= 'A'  &&  *str <= 'F')
		ch = *str++ - 'A' + 0xa;
	    if (*str >= '0'  &&  *str <= '9')
		ch = (ch << 4) | (*str++ - '0');
	    else if (*str >= 'a'  &&  *str <= 'f')
		ch = (ch << 4) | (*str++ - 'a' + 0xa);
	    else if (*str >= 'A'  &&  *str <= 'F')
		ch = (ch << 4) | (*str++ - 'A' + 0xa);
	    break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	    /* Octal sequence */
	    origstr = str;
	    ch = *str++ - '0';
	    if (*str >= '0'  &&  *str <= '7')
		ch = (ch << 3) | (*str++ - '0');
	    if (*str >= '0'  &&  *str <= '7')
		ch = (ch << 3) | (*str++ - '0');
	    break;
	default:
	    *(*out)++ = *str;
	    return 1;
	}
    if (strip)
	{
	*(*out)++ = ch;
	return str - origstr;
	}
    else
	{
	for (ch = 0;  origstr < str;  ch++)
	    *(*out)++ = *origstr++;
	return ch;
	}
    }

int fieldwrite (file, fieldp, delim)
    FILE *		file;	/* File to write to */
    register field_t *	fieldp;	/* Field structure to write */
    int			delim;	/* Delimiter to place between fields */
    {
    int			error;	/* NZ if an error occurs */
    register unsigned int
			fieldno; /* Number of field being written */

    error = 0;
    for (fieldno = 0;  fieldno < fieldp->nfields;  fieldno++)
	{
	if (fieldno != 0)
	    error |= putc (delim, file) == EOF;
	error |= fputs (fieldp->fields[fieldno], file) == EOF;
	}
    if (fieldp->hadnl)
	error |= putc ('\n', file) == EOF;
    return error;
    }

void fieldfree (fieldp)
    register field_t *	fieldp;	/* Field structure to free */
    {

    if (fieldp == NULL)
	return;
    if (fieldp->linebuf != NULL)
	free ((char *) fieldp->linebuf);
    if (fieldp->fields != NULL)
	free ((char *) fieldp->fields);
    free ((char *) fieldp);
    }
