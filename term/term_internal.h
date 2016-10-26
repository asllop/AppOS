#include <appos.h>

#ifndef TERM_INTERNAL_H
#define	TERM_INTERNAL_H

#include <term/term.h>

#ifndef TERM_NUM_DRIVERS
#define TERM_NUM_DRIVERS    2
#endif

struct TermDriverStruct
{
    TERM_TYPE               type;
    int                     customID;
    void                    (*text)(int customID, TERM_COLOR color);
    void                    (*background)(int customID, TERM_COLOR color);
    void                    (*resolution)(int customID, int *w, int *h);
    void                    (*position)(int customID, int x, int y);
    void                    (*cursor)(int customID, bool visible);
    void                    (*reset)(int customID);
    void                    (*putc)(int customID, char c);
    int                     (*getc)(int customID);
};

TERM                        term_register(struct TermDriverStruct *driver);
struct TermDriverStruct     *get_term_driver(TERM term);

#endif
