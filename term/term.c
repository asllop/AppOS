#include <term/term.h>

// TODO all

int term_init(TERM term)
{
    return 0;
}

void term_text(TERM term, TERM_COLOR color)
{}

void term_background(TERM term, TERM_COLOR color)
{}

void term_resolution(TERM term, int *w, int *h)
{}

void term_position(TERM term, int x, int y)
{}

void term_cursor(TERM term, bool visible)
{}

void term_reset(TERM term)
{}

void term_putc(TERM term, char c)
{}

int term_getc(TERM term)
{
    return 0;
}
