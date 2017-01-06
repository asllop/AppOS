#include <term/term_internal.h>

static TERM term_driver_ID_count = 0;
static struct TermDriverStruct term_drivers_list[TERM_NUM_DRIVERS];

TERM term_register(struct TermDriverStruct *driver)
{
    if (term_driver_ID_count < TERM_NUM_DRIVERS)
    {
        TERM driverID = term_driver_ID_count ++;
        term_drivers_list[driverID] = *driver;
        
        return driverID;
    }
    else
    {
        return ERR_CODE_TERMDRIVERCNTOVR;
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
