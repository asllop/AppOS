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
        driver->text(driver->customID, color);
    }
}

void term_background(TERM term, TERM_COLOR color)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        driver->background(driver->customID, color);
    }
}

void term_resolution(TERM term, int *w, int *h)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        driver->resolution(driver->customID, w, h);
    }
}

void term_where(TERM term, int *x, int *y)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        driver->where(driver->customID, x, y);
    }
}

void term_position(TERM term, int x, int y)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        driver->position(driver->customID, x, y);
    }
}

void term_cursor(TERM term, bool visible)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        driver->cursor(driver->customID, visible);
    }
}

void term_reset(TERM term)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        driver->reset(driver->customID);
    }
}

void term_putc(TERM term, char c)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        driver->putc(driver->customID, c);
    }
}

void term_puts(TERM term, char *str)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        for (int i = 0 ; str[i] != 0 ; i++)
        {
            driver->putc(driver->customID, str[i]);
        }
    }
}

int term_getc(TERM term)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        return driver->getc(driver->customID);
    }
    else
    {
        return -1;
    }
}

int term_gets(TERM term, char *str, int sz)
{
    struct TermDriverStruct *driver = get_term_driver(term);
    if (driver)
    {
        int i = 0;
        
        while (i < sz - 1)
        {
            int ich;
            
            do
            {
                ich = driver->getc(driver->customID);
            }
            while (ich == -1);
            
            char ch = ich & 0xff;
            
            if (ch == '\r' || ch == '\n')
            {
                str[i] = '\0';
                return i;
            }
            else if (ch == 127 || ch == '\b')
            {
                i --;
                if (i < 0) i = 0;
                str[i] = '\0';
            }
            else if (ch >= 32)
            {
                str[i] = ch;
                i ++;
            }
        }
        
        str[i] = '\0';
        return i;
    }
    else
    {
        return -1;
    }
}
