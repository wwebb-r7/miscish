#ifndef _MSFTERM_H_
#define _MSFTERM_H_

#include <ncurses.h>
#include <unistd.h>
#include "tmt.h"

struct _mt_mainwin {
    WINDOW *title;
    WINDOW *output;
    WINDOW *console;
};

typedef struct _mt_mainwin mt_mainwin;

WINDOW *goutput;  
WINDOW *gconsole;

fd_set global_fds;

struct _mt_t {
    char    ttyname[128];
    char    buf[8193];
    WINDOW  *window;
    TMT     *vt;
    pid_t   pid_child;
    int     ptyfd, height, width, rows, columns;
};

typedef struct _mt_t mt_t;

void tmt_cb();
mt_t* mt_create(uint16_t width, uint16_t height);
void mt_destroy(mt_t *mt);
void mt_render(mt_t *mt, const char *data, int len);
mt_t* termify_app(const char *app, char * const* argv, uint16_t width, uint16_t height);
void mt_set_win(mt_t *msfterm, WINDOW *window);
int mt_poll(mt_t *mt);
WINDOW *mt_get_win(mt_t *msfterm);
#endif