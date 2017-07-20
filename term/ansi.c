#include <term/ansi.h>
#include <mem/mem.h>
#include <lib/NQCLib/NQCLib.h>

static char *ESC_str = "\x1b[";


char *ansi_set_cursor(char *cmd, int line, int column)
{
    char tmp[5];
    int index = 0;
    
    // ESC
    core_copy(cmd + index, ESC_str, 2);
    index += 2;
    
    // Line
    itoa(line, tmp, 10);
    core_copy(cmd + index, tmp, strlen(tmp));
    index += strlen(tmp);
    
    // Separator (;)
    core_copy(cmd + index, ";", 1);
    index += 1;
    
    // Column
    itoa(column, tmp, 10);
    core_copy(cmd + index, tmp, strlen(tmp));
    index += strlen(tmp);
    
    // End
    cmd[index] = 'H';
    index ++;
    cmd[index] = '\0';
    
    return cmd;
}

char *ansi_set_gfx_mode(char *cmd, int *modes, int num)
{
    char tmp[5];
    int index = 0;
    
    // ESC
    core_copy(cmd + index, ESC_str, 2);
    index += 2;
    
    // Modes
    for (int i = 0 ; i < num ; i++)
    {
        itoa(modes[i], tmp, 10);
        core_copy(cmd + index, tmp, strlen(tmp));
        index += strlen(tmp);
        
        // Separator (;)
        if (i != num - 1)
        {
            core_copy(cmd + index, ";", 1);
            index += 1;
        }
    }
    
    // End
    cmd[index] = 'm';
    index ++;
    cmd[index] = '\0';
    
    return cmd;
}

char *ansi_clear_screen(char *cmd)
{
    int index = 0;
    
    // ESC
    core_copy(cmd + index, ESC_str, 2);
    index += 2;
    
    // Command
    core_copy(cmd + index, "2J", 2);
    index += 2;
    
    // End
    cmd[index] = '\0';
    
    return cmd;
}

char *ansi_hide_cursor(char *cmd)
{
    int index = 0;
    
    // ESC
    core_copy(cmd + index, ESC_str, 2);
    index += 2;
    
    // Command
    core_copy(cmd + index, "?25l", 4);
    index += 4;
    
    // End
    cmd[index] = '\0';
    
    return cmd;
}

char *ansi_show_cursor(char *cmd)
{
    int index = 0;
    
    // ESC
    core_copy(cmd + index, ESC_str, 2);
    index += 2;
    
    // Command
    core_copy(cmd + index, "?25h", 4);
    index += 4;
    
    // End
    cmd[index] = '\0';
    
    return cmd;
}

char *ansi_report_cursor(char *cmd)
{
    int index = 0;
    
    // ESC
    core_copy(cmd + index, ESC_str, 2);
    index += 2;
    
    // Command
    core_copy(cmd + index, "6n", 2);
    index += 2;
    
    // End
    cmd[index] = '\0';
    
    return cmd;
}

bool ansi_parse_report(char *cmd, int *line, int *column)
{
    if (cmd[0] != '\x1b' || cmd[1] != '[')
    {
        return NO;
    }
    
    int index = 2;
    
    // Find separator ';'
    for (int i = index ; ; i++)
    {
        if (i > 1000)
        {
            return NO;
        }
        
        if (cmd[i] == ';')
        {
            cmd[i] = '\0';
            break;
        }
    }
    
    *line = atoi(cmd + index);
    index += strlen(cmd + index);
    index ++;       // ';' place
    
    // Find command letter 'R'
    for (int i = index ; ; i++)
    {
        if (i > 1000)
        {
            return NO;
        }
        
        if (cmd[i] == 'R')
        {
            cmd[i] = '\0';
            break;
        }
    }
    
    *column = atoi(cmd + index);
    
    return YES;
}
