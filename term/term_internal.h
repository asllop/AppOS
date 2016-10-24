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
    void                    (*text)(TERM_COLOR color);
    void                    (*background)(TERM_COLOR color);
    void                    (*resolution)(int *w, int *h);
    void                    (*position)(int x, int y);
    void                    (*cursor)(bool visible);
    void                    (*reset)();
    void                    (*putc)(char c);
    int                     (*getc)();
};

int                         term_register(struct TermDriverStruct *driver);
struct TermDriverStruct     *get_term_driver(TERM term);

#endif
