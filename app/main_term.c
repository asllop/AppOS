#include <mem/mem.h>
#include <task/task.h>
#include <event/event.h>
#include <sys/sys.h>
#include <term/drivers/term_serial/term_serial.h>
#include <term/term.h>
#include "utils.h"

void main(__unused int argc, __unused char **argv)
{
    // Init Term Serial driver on port 0
    TERM termID = term_serial_init(0);
    
    if (termID == -1)
    {
        core_fatal("Term serial driver failed to init");
    }
    
    term_reset(termID);
    term_text(termID, TERM_COLOR_RED);
    term_position(termID, 0, 2);
    
    term_puts(termID, "                                                          \n");
    term_puts(termID, "         _/_/                          _/_/      _/_/_/   \n");
    term_puts(termID, "      _/    _/  _/_/_/    _/_/_/    _/    _/  _/          \n");
    term_puts(termID, "     _/_/_/_/  _/    _/  _/    _/  _/    _/    _/_/       \n");
    term_puts(termID, "    _/    _/  _/    _/  _/    _/  _/    _/        _/      \n");
    term_puts(termID, "   _/    _/  _/_/_/    _/_/_/      _/_/    _/_/_/         \n");
    term_puts(termID, "            _/        _/                                  \n");
    term_puts(termID, "           _/        _/                                   \n");
    term_puts(termID, "                                                          \n");
    
    term_puts(termID, "\n\n");
    term_text(termID, TERM_COLOR_WHITE);
    term_background(termID, TERM_COLOR_BLUE);
    
    for (;;)
    {
        int ch = term_getc(termID);
        if (ch != -1)
        {
            if (ch == 27)
            {
                break;
            }
            
            if (ch == 127 || ch == '\b')
            {
                term_putc(termID, '\b');
                term_putc(termID, ' ');
                term_putc(termID, '\b');
                continue;
            }
            
            if (ch == '\r')
            {
                term_putc(termID, '\n');
                continue;
            }
            
            term_putc(termID, (char)ch);
        }
        else
        {
            core_sleep(0);
        }
    }
    
    term_reset(termID);
    term_puts(termID, "Good bye!\n");
    
    core_halt();
}
