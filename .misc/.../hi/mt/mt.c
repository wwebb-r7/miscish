#include <stdlib.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <pty.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <sys/ioctl.h>
#include "msfterm.h"

void tmt_cb(tmt_msg_t m, TMT *vt, const void *a, void *p)
{
    WINDOW *pw = mt_get_win(((mt_t *)p));

    /* grab a pointer to the virtual screen */
    const TMTSCREEN *s = tmt_screen(vt);
    const TMTPOINT *c = tmt_cursor(vt);
    int y, x = 0;

    switch (m){
        case TMT_MSG_BELL:
            /* the terminal is requesting that we ring the bell/flash the
             * screen/do whatever ^G is supposed to do; a is NULL
             */
            break;

        case TMT_MSG_UPDATE:
            getyx(pw, y, x);
            /* the screen image changed; a is a pointer to the TMTSCREEN */
            for (size_t r = 0; r < s->nline; r++){
                if (s->lines[r]->dirty){
                    for (size_t c = 0; c < s->ncol; c++){
                        // printf("contents of %zd,%zd: %lc (%s bold)\n", r, c,
                        //        s->lines[r]->chars[c].c,
                        //        s->lines[r]->chars[c].a.bold? "is" : "is not");
                        mvwaddch(pw, y, x, s->lines[r]->chars[c].c);
                        // y++;
                        x++;
                        refresh();
                        wrefresh(pw);
                    }
                }
            }

            /* let tmt know we've redrawn the screen */
            tmt_clean(vt);
            break;

        case TMT_MSG_ANSWER:
            /* the terminal has a response to give to the program; a is a
             * pointer to a string */
            // printf("terminal answered %s\n", (const char *)a);
            break;

        case TMT_MSG_MOVED:
            /* the cursor moved; a is a pointer to the cursor's TMTPOINT */
            // printf("cursor is now at %zd,%zd\n", c->r, c->c);
            break;
    }
    // //printf("cb!\t");
    // mvwaddstr(goutput, 1, 1, "cb!\n");
    // wrefresh(goutput);
}

// dead code
mt_t* termify_app(const char *app, char * const* argv, uint16_t width, uint16_t height)
{
    mt_t    *term;
    int     mfd, stat;
    pid_t   pid_child = 0;
    struct  winsize  ws;

    term = (mt_t *)calloc(1,sizeof(mt_t));
    term->rows      = height;
    term->columns   = width;
    
    term->vt     = tmt_open(term->rows, term->columns, tmt_cb, NULL, NULL);
    ws.ws_ypixel = 0;
    ws.ws_xpixel = 0;
    
    ws.ws_row = height;
    ws.ws_col = width;


    pid_child = forkpty(&mfd, NULL, NULL, NULL);
    term->ptyfd = mfd;

    if (pid_child == 0)
    {
        signal(SIGINT,SIG_DFL);
        setenv("TERM", "rxvt", 1);

        if (execl("./tester", "./tester", "wat", NULL) == -1)
        {
            exit(-1);
        }

    }
    term->pid_child = pid_child;
    return term;
}

static void mt_forkpty(mt_t *mt)
{
    pid_t   pid;
    struct  winsize  ws;

    ws.ws_row = mt->height;
    ws.ws_col = mt->width;

    pid = forkpty(&mt->ptyfd, NULL, NULL, &ws);

    if (pid == 0)
    {
        signal(SIGINT,SIG_DFL);
        setenv("TERM", "rxvt", 1);

        if (execl("/bin/bash", "/bin/bash", NULL) == -1)
        {
            exit(-1);
        }

    }

    FD_SET(mt->ptyfd, &global_fds);
    fcntl(mt->ptyfd, F_SETFL, O_NONBLOCK);
    return;
}

mt_t *mt_create(uint16_t width, uint16_t height)
{
    mt_t    *mt;
    int     mfd, stat;
    pid_t   pid_child = 0;

    mt = (mt_t *)calloc(1,sizeof(mt_t));
    mt->rows      = height;
    mt->columns   = width;
    
    mt->vt     = tmt_open(mt->rows, mt->columns, tmt_cb, (mt_t *)mt, NULL);

    raw();
    nodelay(mt->window, TRUE);
    keypad(mt->window, TRUE);
    
    mt_forkpty(mt);
    return mt;
}

void mt_set_win(mt_t *msfterm, WINDOW *window)
{
    if(msfterm != NULL) {
        msfterm->window = window;
        return;
    } else {
        return;
    }
}

WINDOW *mt_get_win(mt_t *msfterm)
{
    return msfterm->window;
}

void mt_destroy(mt_t *mt)
{
    if (mt == NULL)
        return;
    
    tmt_close(mt->vt);
    free(mt);
    mt = NULL;
    return;
}

int mt_poll(mt_t *mt)
{
    unsigned int av, wt, rm , rd, writ, tot = 0;    // available, waiting, remaining
                                                    // read, written, total
    fd_set sfds = global_fds;
    struct pollfd fds;
    struct timeval tv = {60, 0};
    char *readbuf = NULL;
    char *p;

    // probably do some kind of check to make sure
    // child proc hasn't died

    fds.fd      = mt->ptyfd;
    fds.events  = POLLIN;
    
    int ret = poll(&fds, 1, 15);

    if (ret <= 0)
    {
        if (errno == EINTR)
            return 0;
        else
            return errno;
    }

    ret = ioctl(mt->ptyfd, FIONREAD, &av);

    // error reading or nothing to read
    if (ret == -1 || av == 0)
        return 0;
    
    wt = av;
    if(wt > 8192)
        wt = 8192;
    rm = wt;

    readbuf = (char *)calloc(wt + 32, sizeof(char));
    p = readbuf;

    // snatch it all
    while (rm > 0)
    {
        rd = read(mt->ptyfd, p, rm);

        if (rd == -1)
        {
            rd = 0; // nothing for now
        }
        if (rd <= 0);
            break;
        
        rm  = rm - rd;
        tot = tot + rd;
        p   = p + rd;
    }
    if ((wt > 0) && (rd != -1))
    {
        // rendering goes here ...
        tmt_write(mt->vt, readbuf, 0);
    }

    if(readbuf != NULL)
        free(readbuf);
    
    return wt - rm;
}