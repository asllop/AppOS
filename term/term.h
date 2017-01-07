#ifndef TERM_H
#define	TERM_H

#include <appos.h>

typedef enum
{
    TERM_COLOR_BLACK        = 0,
    TERM_COLOR_RED,
    TERM_COLOR_GREEN,
    TERM_COLOR_YELLOW,
    TERM_COLOR_BLUE,
    TERM_COLOR_MAGENTA,
    TERM_COLOR_CYAN,
    TERM_COLOR_WHITE
    
} TERM_COLOR;

typedef enum
{
    TERM_TYPE_CONSOLE       = 0,
    TERM_TYPE_SERIAL,
    TERM_TYPE_NET,
    TERM_TYPE_BT,
    TERM_TYPE_USB,
    TERM_TYPE_VIRTUAL
    
} TERM_TYPE;

typedef int                 TERM;

TERM_TYPE                   term_type(TERM term);
void                        term_text(TERM term, TERM_COLOR color);
void                        term_background(TERM term, TERM_COLOR color);
void                        term_resolution(TERM term, int *w, int *h);
void                        term_where(TERM term, int *x, int *y);
void                        term_position(TERM term, int x, int y);
void                        term_cursor(TERM term, bool visible);
void                        term_reset(TERM term);
void                        term_putc(TERM term, char c);
void                        term_puts(TERM term, char *str);
int                         term_getc(TERM term);
int                         term_gets(TERM term, char *str, int sz);

#endif
