#include <term/term.h>
#include <term/term_internal.h>

TERM_TYPE term_type(TERM term)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        return driver->type;
    }
    else
    {
        return -1;
    }
}

void term_text(TERM term, TERM_COLOR color)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        driver->text(color);
    }
}

void term_background(TERM term, TERM_COLOR color)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        driver->background(color);
    }
}

void term_resolution(TERM term, int *w, int *h)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        driver->resolution(w, h);
    }
}

void term_position(TERM term, int x, int y)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        driver->position(x, y);
    }
}

void term_cursor(TERM term, bool visible)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        driver->cursor(visible);
    }
}

void term_reset(TERM term)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        driver->reset();
    }
}

void term_putc(TERM term, char c)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        driver->putc(c);
    }
}

int term_getc(TERM term)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        return driver->getc();
    }
    else
    {
        return -1;
    }
}
