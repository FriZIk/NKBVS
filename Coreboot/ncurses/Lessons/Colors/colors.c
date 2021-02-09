#include <ncurses.h>

int main()
{
    initscr();

    if(!has_colors())
    {
        printw("Terminal does not supported colors");
        getch();
        return -1;
    }

    start_color();
    /*
     * A_NORMAL
     * A_STANDOUT
     * A_REVERSE
     * A_BLINK
     * A_DIM
     * A_BOLD
     * A_PROTECT
     * A_INVIS
     * A_ALTCHARSET
     * A_CHARTEXT
     */

    init_pair(1, COLOR_CYAN, COLOR_MAGENTA);
    init_pair(2, COLOR_GREEN, COLOR_YELLOW);

    if(can_change_color())
    {
        printw("Can change color");
        init_color(COLOR_CYAN, 9, 999, 999);
    }

    //attron(A_STANDOUT);
    attron(COLOR_PAIR(1));
    printw("This is some text");
    attroff(COLOR_PAIR(1));
    
    /*
     * COLOR_PAIR(n)
     * COLOR_BLACK   0
     * COLOR_RED     1
     * COLOR_GREEN   2
     * COLOR_YELLOW  3
     * COLOR_BLUE    4
     * COLOR_MAGENTA 5
     * COLOR_CYAN    6
     * COLOR_WHITE   7
     */

    getch();

}