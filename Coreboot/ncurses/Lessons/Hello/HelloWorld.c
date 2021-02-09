#include <ncurses.h>

int main()
{
    //Initializes the screen
    initscr();

    // Prints a string (const char *) to a window
    printw("Heloow world!");
    
    // refreshes the screen to match whats in memory
    refresh();

    // Whats for user input, returns int value of that key
    int c = getch();

    printw("%d",c);

    getch();
    endwin();
    // Deallocates memory and ends ncurses  
    return 0;
}