#include <term/ansi.h>
#include <mem/mem.h>

static char *ESC_str = "\x1b[";

// Rewrite some standard functions to avoid libC dependency

char *ansi_itoa(int i, char *b)
{
    char const digit[] = "0123456789";
    char *p = b;
    
    if (i < 0)
    {
        *p++ = '-';
        i *= -1;
    }
    
    int shifter = i;
    
    do
    {
        // Move to where representation ends
        ++p;
        shifter = shifter / 10;
    }
    while (shifter);
    
    *p = '\0';
    
    do
    {
        // Move back, inserting digits as u go
        *--p = digit[i % 10];
        i = i / 10;
    }
    while (i);
    
    return b;
}

int ansi_atoi(char *str)
{
    int res = 0; // Initialize result
    
    // Iterate through all characters of input string and
    // update result
    for (int i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';
    
    // return result.
    return res;
}

size_t ansi_strlen(const char *str)
{
    size_t i;
    
    for (i = 0 ; str[i] != 0 ; i++)
    {
        // Limit
        if (i > 1000)
        {
            break;
        }
    }
    
    return i;
}

char *ansi_set_cursor(char *cmd, int line, int column)
{
    char tmp[5];
    int index = 0;
    
    // ESC
    core_copy(cmd + index, ESC_str, 2);
    index += 2;
    
    // Line
    ansi_itoa(line, tmp);
    core_copy(cmd + index, tmp, ansi_strlen(tmp));
    index += ansi_strlen(tmp);
    
    // Separator (;)
    core_copy(cmd + index, ";", 1);
    index += 1;
    
    // Column
    ansi_itoa(column, tmp);
    core_copy(cmd + index, tmp, ansi_strlen(tmp));
    index += ansi_strlen(tmp);
    
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
        ansi_itoa(modes[i], tmp);
        core_copy(cmd + index, tmp, ansi_strlen(tmp));
        index += ansi_strlen(tmp);
        
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
    
    *line = ansi_atoi(cmd + index);
    index += ansi_strlen(cmd + index);
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
    
    *column = ansi_atoi(cmd + index);
    
    return YES;
}
