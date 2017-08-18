#include <curses.h>

int main()
{
	WINDOW *wnd;
	
	wnd = initscr();
	cbreak();
	noecho();
	clear();
	refresh();
	
	while(1){}

	return 0;
}
