#include <appos.h>
#include <sys/sys.h>

#define MAX_ARGS            16
#define MAX_ARGS_BUF_SZ     256

extern void *grub_struct;

static char *string_pointers[MAX_ARGS];
static char cmdLineBuf[MAX_ARGS_BUF_SZ];

// Read command line arguments from GRUB structure
char **setup_cmdline(int *argcPointer)
{
    int argc = 0;
    char **argv = NULL;

    uint32_t grub_flags = *((uint32_t *)grub_struct);

    // command line is present
    if (grub_flags & (1 << 2))
    {
        argv = string_pointers;
        
        long offset = 4;
        if (grub_flags & (1 << 0))
        {
            offset += 8;
        }
        
        if (grub_flags & (1 << 1))
        {
            offset += 4;
        }
        
        char **p = grub_struct + offset;
        char *args = p[0];
        
        if (!args)
        {
            *argcPointer = 0;
            return NULL;
        }
        
        // Copy cmd line into buffer to save it from destruction
        int bufIndex;
        for (bufIndex = 0 ; bufIndex < MAX_ARGS_BUF_SZ ; bufIndex ++) {
            cmdLineBuf[bufIndex] = args[bufIndex];
        }
        cmdLineBuf[bufIndex] = '\0';
        
        char *currArg = cmdLineBuf;
        bool insideArg = YES;
        
        for (int i = 0 ; cmdLineBuf[i] != '\0' ; i++)
        {
            if (cmdLineBuf[i] == '\0')          // End of cmd line, last arg
            {
                if (insideArg)
                {
                    string_pointers[argc] = currArg;
                    insideArg = NO;
                    argc ++;
                }
            }
            else if (cmdLineBuf[i] == ' ')      // Space, end of arg
            {
                if (insideArg)
                {
                    string_pointers[argc] = currArg;
                    cmdLineBuf[i] = '\0';
                    insideArg = NO;
                    argc ++;
                }
            }
            else                                // Any other char
            {
                if (!insideArg)
                {
                    currArg = &cmdLineBuf[i];
                    insideArg = YES;
                }
            }
        }
        
        if (insideArg)
        {
            string_pointers[argc++] = currArg;
        }
    }

    *argcPointer = argc;
    return argv;
}
