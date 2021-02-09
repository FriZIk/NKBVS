#include <ncurses.h>

int main()
{
    initscr();

    int height,width,start_x,start_y;
    height = 10;
    width = 20;
    start_y = start_x = 10;

    WINDOW * win = newwin(height,width,start_x,start_y);
    refresh();


    box(win,0,0);
    wprintw(win,"This is my box");
    mvwprintw(win,1,1,"This is my box");
    wrefresh(win); // for special windows

    //Whats for user input
    int c = getch();
    
    endwin();
    // Dellocates memory and ends ncurses
    return 0;
}