#include <ncurses.h>

int main()
{

    // Ncurses start
    initscr();
    noecho();
    cbreak();

    // Get screen size
    int yMax,xMax;
    getmaxyx(stdscr,yMax,xMax);

    // Create a window for our input
    WINDOW *menuwin = newwin(6, xMax-12, yMax-8, 5);
    box(menuwin, 0, 0);
    refresh();
    wrefresh(menuwin);

    // Makes it so we can use arrow keys
    keypad(menuwin,true);

    char choises[3][10] = {"Walk","Jog","Run"};
    int choice;
    int highlight = 0;

    while(1)
    {
        for(int i = 0;i < 3; i++)
        {
            if(i == highlight)
                wattron(menuwin,A_REVERSE);
            //mvwprintw(menuwin,i+1,1,choises[i].c_str());
            wattroff(menuwin, A_REVERSE);
        }
        choice = wgetch(menuwin);
        
        switch(choice)
        {
            case KEY_UP:
                highlight--;
                if(highlight == -1)
                    highlight = 0;
                break;
            case KEY_DOWN:
                highlight++;
                if(highlight == 3)
                    highlight = 2;
                break;
            default: break;
        }
        if(choice == 10)
            break;
    }

    //printw("Your choice was: %s",choises[highlight].c_str());
    return 0;
}