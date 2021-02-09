#include <ncurses.h>

int main()
{
    initscr();
    noecho();
    cbreak();

    // get screen size
    int yMax, xMax;
    getmaxyx(stdscr,yMax,xMax);
    

    //create a window for out input
    WINDOW *inputwin = newwin(3,xMax-12,yMax-5, 5);
    box(inputwin,0,0);
    refresh();
    wrefresh(inputwin);

    keypad(inputwin,true);

    int c = wgetch(inputwin);
    if(c == KEY_UP)
    {
        mvwprintw(inputwin, 1, 1, "You presses up!");
        wrefresh(inputwin);
    }

    getch();
    endwin();

    return 0;
}