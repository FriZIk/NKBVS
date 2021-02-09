#include <ncurses.h>
#include <string.h>

void start_ncurses(bool useRaw, bool useNoecho);
void printMenu(WINDOW * menu, char choices[],int size,int highlight);

int main()
{
    initscr();
    noecho();
    cbreak();

    int y,x,yBeg,xBeg,yMax,xMax;
    getyx(stdscr,y,x);
    getbegyx(stdscr,yBeg,xBeg);
    getmaxyx(stdscr,yMax,xMax);

    printw("%d %d %d %d %d %d",y,x,yBeg,xBeg,yMax,xMax);

    // Print symbols on the middle of screen
    mvprintw(yMax/2, xMax/2, "Hello");


    // Make sure program waits before exiting...
    getch();
    endwin();
    //Ncurses end

    return 0;
}