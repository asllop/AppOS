#include <term/term_internal.h>

static TERM term_driver_ID_count = 0;
static struct TermDriverStruct term_drivers_list[TERM_NUM_DRIVERS];

TERM term_register(struct TermDriverStruct *driver)
{
    if (term_driver_ID_count < TERM_NUM_DRIVERS)
    {
        TERM driverID = term_driver_ID_count;
        term_driver_ID_count ++;
        
        term_drivers_list[driverID].type = driver->type;
        term_drivers_list[driverID].customID = driver->customID;
        term_drivers_list[driverID].text = driver->text;
        term_drivers_list[driverID].background = driver->background;
        term_drivers_list[driverID].resolution = driver->resolution;
        term_drivers_list[driverID].position = driver->position;
        term_drivers_list[driverID].cursor = driver->cursor;
        term_drivers_list[driverID].reset = driver->reset;
        term_drivers_list[driverID].putc = driver->putc;
        term_drivers_list[driverID].getc = driver->getc;
        
        return driverID;
    }
    else
    {
        return -1;
    }
}

struct TermDriverStruct *get_term_driver(TERM term)
{
    if (term < term_driver_ID_count)
    {
        return &term_drivers_list[term];
    }
    else
    {
        return NULL;
    }
}
