#include <ncurses.h>

int main()
{
    initscr();
    cbreak();

    int height,width,start_x,start_y;
    height = 10;
    width = 20;
    start_y = start_x = 0;

    WINDOW *win = newwin(height,width,start_x,start_y);
    refresh();

    char c = '+';

    //box(win,inc(c),104); // Numbers is ascii symbols

    int left, right, top, bottom, tlc, trc, blc, brc;
    left = right = (int)'g';
    top = bottom = 104;
    tlc = trc = blc = brc = (int)c; // Angel

    wborder(win,left,right,top,bottom,tlc,trc,blc,brc);
    mvwprintw(win, 1, 1, "This is my box");
    wrefresh(win);


    getch();
    endwin();
    return 0;
}