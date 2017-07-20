#include <term/term_internal.h>
#include <term/term.h>
#include <term/ansi.h>
#include <lib/NQCLib/NQCLib.h>
#include "term_serial.h"

void term_serial_text(int customID, TERM_COLOR color)
{
    char cmd[10];
    ansi_set_gfx_mode(cmd, (int[]){color + 30}, 1);
    serial_send((PORT)customID, (byte *)cmd, strlen(cmd));
}

void term_serial_background(int customID, TERM_COLOR color)
{
    char cmd[10];
    ansi_set_gfx_mode(cmd, (int[]){color + 40}, 1);
    serial_send((PORT)customID, (byte *)cmd, strlen(cmd));
}

void term_serial_where(int customID, int *x, int *y)
{
    char cmd[10];
    
    ansi_report_cursor(cmd);
    serial_send((PORT)customID, (byte *)cmd, strlen(cmd));
    
    int i = 0;
    while (true) {
        if (serial_avail((PORT)customID)) {
            byte ch;
            serial_receive((PORT)customID, &ch, 1);
            cmd[i++] = (char)ch;
            if (ch == 'R')
            {
                cmd[i] = '\0';
                break;
            }
        }
    }
    ansi_parse_report(cmd, y, x);
    
    (*y) --;
    (*x) --;
}

void term_serial_position(int customID, int x, int y)
{
    char cmd[12];
    ansi_set_cursor(cmd, y + 1, x + 1);
    serial_send((PORT)customID, (byte *)cmd, strlen(cmd));
}

void term_serial_resolution(int customID, int *w, int *h)
{
    int x, y;
    
    term_serial_where(customID, &x, &y);
    term_serial_position(customID, 999, 999);
    term_serial_where(customID, w, h);
    term_serial_position(customID, x, y);
    (*w) ++;
    (*h) ++;
}

void term_serial_cursor(int customID, bool visible)
{
    char cmd[10];
    
    if (visible)
    {
        ansi_show_cursor(cmd);
    }
    else
    {
        ansi_hide_cursor(cmd);
    }
    
    serial_send((PORT)customID, (byte *)cmd, strlen(cmd));
}

void term_serial_reset(int customID)
{
    char cmd[10];
    
    ansi_clear_screen(cmd);
    serial_send((PORT)customID, (byte *)cmd, strlen(cmd));
    term_serial_position(customID, 0, 0);
}

void term_serial_putc(int customID, char c)
{
    serial_send((PORT)customID, (byte *)&c, 1);
}

int term_serial_getc(int customID)
{
    if (serial_avail((PORT)customID))
    {
        byte dat[1];
        serial_receive((PORT)customID, dat, 1);
        return (int)dat[0];
    }
    else
    {
        return -1;
    }
}

TERM term_serial_init(PORT port)
{
    struct TermDriverStruct driver = {
        .type = TERM_TYPE_SERIAL,
        .customID = port,
        .text = term_serial_text,
        .background = term_serial_background,
        .resolution = term_serial_resolution,
        .where = term_serial_where,
        .position = term_serial_position,
        .cursor = term_serial_cursor,
        .reset = term_serial_reset,
        .putc = term_serial_putc,
        .getc = term_serial_getc
    };
    
    return term_register(&driver);
}
