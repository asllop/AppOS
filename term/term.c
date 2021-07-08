#include <sys/sys.h>
#include <term/term.h>
#include <term/term_internal.h>

TERM_TYPE term_type(TERM term)
{
    struct TermDriverStruct *driver = term_get_driver(term);
    if (driver)
    {
        return driver->type;
    }
    else
    {
        return ERR_CODE_NOTERMDRIVER;
    }
}

void term_text(TERM term, TERM_COLOR color)
{
    struct TermDriverStruct *driver = term_get_driver(term);
    if (driver)
    {
        core_lock(MUTEX_TERM);
        driver->text(driver->customID, color);
        core_unlock(MUTEX_TERM);
    }
}

void term_background(TERM term, TERM_COLOR color)
{
    struct TermDriverStruct *driver = term_get_driver(term);
    if (driver)
    {
        core_lock(MUTEX_TERM);
        driver->background(driver->customID, color);
        core_unlock(MUTEX_TERM);
    }
}

void term_resolution(TERM term, int *w, int *h)
{
    struct TermDriverStruct *driver = term_get_driver(term);
    if (driver)
    {
        core_lock(MUTEX_TERM);
        driver->resolution(driver->customID, w, h);
        core_unlock(MUTEX_TERM);
    }
}

void term_where(TERM term, int *x, int *y)
{
    struct TermDriverStruct *driver = term_get_driver(term);
    if (driver)
    {
        core_lock(MUTEX_TERM);
        driver->where(driver->customID, x, y);
        core_unlock(MUTEX_TERM);
    }
}

void term_position(TERM term, int x, int y)
{
    struct TermDriverStruct *driver = term_get_driver(term);
    if (driver)
    {
        core_lock(MUTEX_TERM);
        driver->position(driver->customID, x, y);
        core_unlock(MUTEX_TERM);
    }
}

void term_cursor(TERM term, bool visible)
{
    struct TermDriverStruct *driver = term_get_driver(term);
    if (driver)
    {
        core_lock(MUTEX_TERM);
        driver->cursor(driver->customID, visible);
        core_unlock(MUTEX_TERM);
    }
}

void term_reset(TERM term)
{
    struct TermDriverStruct *driver = term_get_driver(term);
    if (driver)
    {
        core_lock(MUTEX_TERM);
        driver->reset(driver->customID);
        core_unlock(MUTEX_TERM);
    }
}

// NOTE: Thread safety depends on driver implementation
void term_putc(TERM term, char c)
{
    struct TermDriverStruct *driver = term_get_driver(term);
    if (driver)
    {
        driver->putc(driver->customID, c);
    }
}

void term_puts(TERM term, char *str)
{
    struct TermDriverStruct *driver = term_get_driver(term);
    if (driver)
    {
        core_lock(MUTEX_TERM);
        
        for (int i = 0 ; str[i] != 0 ; i++)
        {
            driver->putc(driver->customID, str[i]);
        }
        
        core_unlock(MUTEX_TERM);
    }
}

// NOTE: Thread safety depends on driver implementation
int term_getc(TERM term)
{
    struct TermDriverStruct *driver = term_get_driver(term);
    if (driver)
    {
        return driver->getc(driver->customID);
    }
    else
    {
        return ERR_CODE_NOTERMDRIVER;
    }
}

int term_gets(TERM term, char *str, int sz)
{
    struct TermDriverStruct *driver = term_get_driver(term);
    
    if (driver)
    {
        core_lock(MUTEX_TERM);
        
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
                // Echo
                driver->putc(driver->customID, ch);
                
                str[i] = '\0';
                core_unlock(MUTEX_TERM);
                return i;
            }
            else if (ch == 127 || ch == '\b')
            {
                i --;
                if (i < 0) i = 0;
                str[i] = '\0';
                
                // Echo
                driver->putc(driver->customID, ch);
                driver->putc(driver->customID, ' ');
                driver->putc(driver->customID, ch);
            }
            else if (ch >= 32)
            {
                // Echo
                driver->putc(driver->customID, ch);
                
                str[i] = ch;
                i ++;
            }
        }
        
        str[i] = '\0';
        
        core_unlock(MUTEX_TERM);
        
        return i;
    }
    else
    {
        return ERR_CODE_NOTERMDRIVER;
    }
}
