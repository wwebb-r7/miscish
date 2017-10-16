#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <locale.h>
#include <string.h>
#include <poll.h>
#include <pty.h>
#include <unistd.h>

#include "msfterm.h"

bool running = true;


int main(int argc, char **argv)
{
    WINDOW *win;
    mt_t *mt;
    int maxx, maxy;
    pid_t child;
    //struct winsize ws;
    //struct termios child_termios;
    char *cmd = "touch";
    char *targv[3];
    targv[0] = "touch";
    targv[1] = "wut.txt";
    targv[2] = NULL;
    fd_set  fdread, fdwrite, fdex;
    printf("starting up.....\n");
    //block_winch();
    setlocale(LC_ALL,"UTF-8");

    initscr();
    refresh();
    start_color();
    //noecho();
    // raw();
    // nodelay(stdscr, TRUE);
    // keypad(stdscr, TRUE);

    getmaxyx(stdscr, maxy, maxx);


    int halfy = maxy >> 1;
    int ystart = maxy >> 2;
    int halfx = maxx >> 1;
    
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (i != 7 || j != 0) init_pair(j*8+7-i, i, j);
        }
    }

    // color stuff
    // the color system in ncurses is batshit insane imo
    // figure this out last
    
    attrset(COLOR_PAIR(30));
    for (int i = 0; i < maxy; i++)
    {
        for (int j = 0; j < maxx; j++) addch(' ');
    }
    refresh();
    // printf("what. the. fuck.\n"); // should not print
    int consolesz = 5;
    
    goutput      = newwin(maxy - consolesz, maxx, 0, 0);
    gconsole     = newwin(consolesz, maxx, maxy - consolesz, 0);

    init_pair(3, COLOR_BLACK, COLOR_GREEN);
    mvwaddstr(gconsole, 1, 1, "command> ");
    mvwaddstr(goutput, 1, 1, "Here is where the output of commands goes ...");
    wbkgd(gconsole, COLOR_PAIR(3));
    wrefresh(gconsole);
    wrefresh(goutput);

    /* 
        create terminal and plug it in here
        - create/open pty
        - exec program (bash)
        - plug slave end of pty into bash
        - plug master into ncurses window (win)
    */
  
    // you'll want to pass the shell to run -- or figure it out programmatically --
    // here before any kind of release
    mt = mt_create(halfy, maxx);
    
    if (mt == NULL)
    {
        perror("mt_create()");
        exit(-1);
    }

    mt_set_win(mt, gconsole);        // associate vt with our ncurses window

    /*
        now we need something to
        - select/read on gfd
        - parses ansi ctrl chars
        - check for input
        - update everything
        - probably ensure cursor is correct afterwards
     */

    while (running)
    {


        // if (select(3, &sfds, NULL, NULL) < 0)
        //     FD_ZERO(&sfds);

        // int r = wget_wch(mt->win, &x);
        mt_poll(mt);
        // only here to bailout during dev
        int c = getch();
        if (c == 'z')
            running = false;
    }

    getch();
    wrefresh(gconsole);  // refresh window after all this stuff
                    // eventually we should see program output
                    // at this point
    getch();
    delwin(gconsole);
    delwin(goutput);
    endwin();
    return 0;
}