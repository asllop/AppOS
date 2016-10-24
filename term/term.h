#include <appos.h>

#ifndef TERM_H
#define	TERM_H

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
    TERM_CTRL_UP            = 256,
    TERM_CTRL_DOWN,
    TERM_CTRL_LEFT,
    TERM_CTRL_RIGHT
    // TODO: more control seq returned by term_getc
    
} TERM_CTRL;

#define TERM_EMPTY          -1

typedef unsigned char       TERM;

int                         term_init(TERM term);
void                        term_text(TERM term, TERM_COLOR color);
void                        term_background(TERM term, TERM_COLOR color);
void                        term_resolution(TERM term, int *w, int *h);
void                        term_position(TERM term, int x, int y);
void                        term_cursor(TERM term, bool visible);
void                        term_reset(TERM term);
void                        term_putc(TERM term, char c);
int                         term_getc(TERM term);

/* Architecture dependant functions */

#endif
