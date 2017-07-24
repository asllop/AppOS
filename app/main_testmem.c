#include <mem/mem.h>
#include <mem/mem_internal.h>
#include <task/task.h>
#include <sys/sys.h>
#include <term/drivers/term_serial/term_serial.h>
#include <term/term.h>
#include <lib/NQCLib/NQCLib.h>
#include "utils.h"

static TERM termID;

void counter_foo(int line)
{
    char *str = "Hello World using term serial\n";
    char var_str[100];
    
    console_put_string(0x4f, "                               ", 2, line);
    
    for (unsigned int i = 0 ;; i++)
    {
        char *mem0 = core_malloc(5000);
        
        //term_text(termID, TERM_COLOR_GREEN);
        //term_puts(termID, str);
        
        //core_sleep(0);
        
        char *mem1 = core_malloc(5000);
        
        console_put_string(0x4f, itoa(i, var_str, 10), 2, line);
        
        if (mem0)
        {
            core_free(mem0);
        }
        else
        {
            // Not enough free mem
            core_fatal("Task could not malloc 0");
        }
        
        if (mem1)
        {
            core_free(mem1);
        }
        else
        {
            // Not enough free mem
            core_fatal("Task could not malloc 1");
        }
    }
}

void cnt0_task()
{
    counter_foo(24);
}

void cnt1_task()
{
    counter_foo(23);
}

void cnt2_task()
{
    counter_foo(22);
}

void cnt3_task()
{
    counter_foo(21);
}

void cnt4_task()
{
    counter_foo(20);
}

void cnt5_task()
{
    counter_foo(19);
}

void cnt6_task()
{
    counter_foo(18);
}

void cnt7_task()
{
    counter_foo(17);
}

void cnt8_task()
{
    counter_foo(16);
}

void cnt9_task()
{
    counter_foo(15);
}

void cnt10_task()
{
    counter_foo(14);
}

void cnt11_task()
{
    counter_foo(13);
}

void showUsedMem()
{
    char var_str[20];
    size_t totalMem = core_avail(MEM_TYPE_USED);
    console_put_string(0x4f, "Used Mem:", 60, 3);
    console_put_string(0x4f, "          ", 60, 4);
    console_put_string(0x4f, ltoa(totalMem, var_str, 10), 60, 4);
}

void print_segs()
{
    char var_str[10];
    byte numBlocks;
    struct BlockStruct *blocks = get_blocks(&numBlocks);
    
    for (byte i = 0 ; i < numBlocks ; i++)
    {
        size_t numSegments = blocks[i].blockSize / SEGMENT_SIZE;
        void *p = blocks[i].block;
        size_t j = 0;
        int k = 0;
        
        while (j < numSegments)
        {
            SEGMENT sizeSegs = *((SEGMENT *)p);
            if (sizeSegs == 0)
            {
                // Empty segment
                j ++;
            }
            else
            {
                // Used segment, part of a buffer. Jump to the end of the buffer
                j += sizeSegs;
            }
            
            term_puts(termID, "\n\nAddress = ");
            //ultoa((unsigned long)p, var_str, 16);
            ultoa((unsigned long)p, var_str, 16);
            term_puts(termID, var_str);
            term_puts(termID, " Segments = ");
            itoa(sizeSegs, var_str, 10);
            term_puts(termID, var_str);
            
            p = blocks[i].block + (SEGMENT_SIZE * j);
            
            // Only print first 10 segments
            if (k > 15) break;
            else k++;
        }
    }
}

void main(__unused int argc, __unused char **argv)
{
    char var_str[20];
    
    // Init Term Serial driver on port 0
    termID = term_serial_init(0);
    
    if (termID == -1)
    {
        core_fatal("Term serial driver failed to init");
    }
    
    term_puts(termID, "Inicial:\n");
    print_segs();
    
    term_puts(termID, "\n---------------\n");
    
    // Draw Background
    console_put_data(0x1b, 176, 80*25, 0);
    
    //void *p0 = core_malloc(70000);
    //size_t p0sz = core_size(p0);
    
    //core_log("P0 real size = ");
    //core_log(ultoa((unsigned long)p0sz, var_str, 10));
    //core_log("\n");
    
    // Create a task with priority 0 (the highest)
//    core_create(cnt0_task, 0, DEFAULT_STACK_SIZE);
//    
//    core_create(cnt1_task, 5, DEFAULT_STACK_SIZE);
//
//    core_create(cnt2_task, 10, DEFAULT_STACK_SIZE);
//    
//    core_create(cnt3_task, 20, DEFAULT_STACK_SIZE);
//    
//    core_create(cnt4_task, 0, DEFAULT_STACK_SIZE);
//    
//    core_create(cnt5_task, 5, DEFAULT_STACK_SIZE);
//    
//    core_create(cnt6_task, 10, DEFAULT_STACK_SIZE);
//    
//    core_create(cnt7_task, 20, DEFAULT_STACK_SIZE);
//    
//    core_create(cnt8_task, 0, DEFAULT_STACK_SIZE);
//    
//    core_create(cnt9_task, 5, DEFAULT_STACK_SIZE);
//    
//    core_create(cnt10_task, 10, DEFAULT_STACK_SIZE);
//    
//    core_create(cnt11_task, 20, DEFAULT_STACK_SIZE);
    
    console_put_string(0x4f, " Hello AppOS ", 34, 1);
    
    //core_free(p0);
    
    void *p0 = core_malloc(5000);
    void *p1 = core_malloc(3000);
    void *p2 = core_malloc(4000);
    
    term_puts(termID, "Hem fet 3 mallocs:\n");
    print_segs();
    
    term_puts(termID, "\n---------------\n");
    
    core_free(p1);
    
    term_puts(termID, "Free segon malloc (p1):\n");
    print_segs();
    
    term_puts(termID, "\n---------------\n");
    
    core_realloc(p0, 5000, 7000, 0);
    
    term_puts(termID, "Realloc p0 de 5000 a 7000:\n");
    print_segs();
    
    term_puts(termID, "\n---------------\n");
    
    // aquest hauria de fallar
    core_realloc(p0, 7000, 10000, 0);
    
    term_puts(termID, "Realloc p0 de 7000 a 10000 (hauria de fallar):\n");
    print_segs();
    
    term_puts(termID, "\n---------------\n");
    core_realloc(p2, 4000, 10000, 0);
    term_puts(termID, "Realloc p2 de 4000 a 10000:\n");
    print_segs();
    
    int line = 5;
    
    for (unsigned int i = 0 ;; i++)
    {
        char *mem = core_malloc(5000);
        
        if (i == 0)
        {
            console_put_string(0x4f, "                               ", 2, line);
        }
        
        console_put_string(0x4f, itoa(i, var_str, 10), 2, line);
        
        showUsedMem();
        
        if (mem)
        {
            core_free(mem);
        }
        else
        {
            core_fatal("Main could not malloc");
        }
    }
    
    line++;
    
    console_put_string(0x4f, "End", 2, line++);
    
    // When main function reaches end, the system is restarted
    for (;;);
}
