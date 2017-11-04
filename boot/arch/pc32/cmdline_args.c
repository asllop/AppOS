#include <appos.h>

#define MAX_ARGS 256

extern void *grub_struct;

static char *string_pointers[MAX_ARGS];

// Read command line arguments from GRUB structure
char **setup_cmdline(int *argcPointer)
{
    int argc = 0;
    char **argv = 0;

    uint32_t grub_flags = *((uint32_t *)grub_struct);

    // command line is present
    if (grub_flags & (1 << 2))
    {
        argv = string_pointers;

        char **p = grub_struct + 16;
        char *cmdLine = *p;

        // Name of kernel file
        string_pointers[argc] = &cmdLine[argc];

        for (int i = 0 ; cmdLine[i] != '\0' ; i++)
        {
            // Found space, go for next argument
            if (cmdLine[i] == ' ')
            {
                cmdLine[i] = '\0';

                if (cmdLine[i + 1] != ' ')
                {
                    argc ++;
                    string_pointers[argc] = &cmdLine[i + 1];
                }
            }

            if (argc >= MAX_ARGS) break;
        }

        argc ++;
    }

    *argcPointer = argc;
    return argv;
}
