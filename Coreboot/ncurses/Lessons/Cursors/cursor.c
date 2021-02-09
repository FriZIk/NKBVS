#include <ncurses.h>

int main()
{
    initscr();

    // Example coordinates
    int x,y;
    x = y = 10;


    // Moves cursor to the specified location
    move(x,y); 
    printw("Hello world!!");


    int c = getch();
    
    // Clears the screen
    clear();
    
    //move(0,0);
    //printf("%d",c);
    mvprintw(0,0,"%d",c);
    refresh();


    getch();
    endwin();

    return 0;
}