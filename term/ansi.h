#include <appos.h>

#ifndef ANSI_H
#define	ANSI_H

char                        *ansi_set_cursor(char *cmd, int line, int column);
char                        *ansi_set_gfx_mode(char *cmd, int *modes, int num);
char                        *ansi_clear_screen(char *cmd);
char                        *ansi_hide_cursor(char *cmd);
char                        *ansi_show_cursor(char *cmd);
char                        *ansi_report_cursor(char *cmd);
bool                        ansi_parse_report(char *cmd, int *line, int *column);

#endif