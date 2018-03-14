#define _GNU_SOURCE 1
#include <ncurses.h>

#include <stdlib.h>	
#include <string.h>
#include <unistd.h>

#define xcenter  ((MB.xmin + MB.xmax) / 2)
#define ycenter  ((MB.ymin + MB.ymax) / 2)

#define zoom 2.0
#define max_iteration 250

typedef struct
{
	double xmax,xmin,ymax,ymin,
			xscale,yscale;
	short ascii;	
}Canvas;


void draw_mb(Canvas MB);
int main(int argc, char const *argv[])
{
	// init of ncurses environment
	initscr();				// start the curses mode
	noecho();				// Doesnt show which character is typed
	cbreak();				// disable input buffering 				
	keypad(stdscr, TRUE);	// enable keyboard support
	curs_set(0);			// hide cursor
	int width,height;
	getmaxyx(stdscr, height, width);

	// Start color
	start_color();			
	init_pair(1, COLOR_BLACK, 	COLOR_WHITE);
	init_pair(2, COLOR_WHITE, 	COLOR_BLACK);
	init_pair(3, COLOR_GREEN,	COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, 	COLOR_BLACK);
	init_pair(5, COLOR_RED,		COLOR_BLACK);


	Canvas MB;
	// default scaling values
	MB.xmax = 1;
	MB.xmin = -2.5;
	MB.ymax = 1;
	MB.ymin = -1;

	MB.xscale = ((MB.xmax - MB.xmin) / width);
	MB.yscale = ((MB.ymax - MB.ymin) / height);
	
	MB.ascii = 0;
	double newxmin,newxmax,newymin,newymax;
	
	draw_mb(MB);
	int help = 0;
	int zlevel=0;
	int c;
	while((c = getch())){
		switch(c){
			case KEY_UP:
				MB.ymin-= MB.yscale;
				MB.ymax-= MB.yscale;
				break;
			case KEY_DOWN:
				MB.ymin+= MB.yscale;
				MB.ymax+= MB.yscale;
				break;
			case KEY_LEFT:
				MB.xmin-= MB.xscale;
				MB.xmax-= MB.xscale;
				break;
			case KEY_RIGHT:
				MB.xmin+= MB.xscale;
				MB.xmax+= MB.xscale;
				break;
			case (int) 'a': // toggle ascii
				MB.ascii = MB.ascii == 1 ? 0 : 1;  
				break;
			case (int) 'h': // toggle ascii
				help = help == 1 ? 0 : 1;  
				break;
			case 10: // enter - zoom in
				newymin = ycenter - (MB.ymax - MB.ymin) / (2 * zoom);
				newymax = ycenter + (MB.ymax - MB.ymin) / (2 * zoom);
				newxmin = xcenter - (MB.xmax - MB.xmin) / (2 * zoom);
				newxmax = xcenter + (MB.xmax - MB.xmin) / (2 * zoom);

				MB.ymin = newymin; MB.ymax = newymax;
				MB.xmin = newxmin; MB.xmax = newxmax;
				zlevel++;
				break;
			case 32: // zoom out
				newymin = ycenter - (MB.ymax - MB.ymin) / (2 * 1/zoom);
				newymax = ycenter + (MB.ymax - MB.ymin) / (2 * 1/zoom);
				newxmin = xcenter - (MB.xmax - MB.xmin) / (2 * 1/zoom);
				newxmax = xcenter + (MB.xmax - MB.xmin) / (2 * 1/zoom);

				MB.ymin = newymin; MB.ymax = newymax;
				MB.xmin = newxmin; MB.xmax = newxmax;		
				zlevel--;
				break;
			case (int) 'q': // 113 = q
				endwin();
				return 0;
				break;

			default:
				break;
		}
		MB.xscale = ((MB.xmax - MB.xmin) / width);
		MB.yscale = ((MB.ymax - MB.ymin) / height);
		draw_mb(MB);
		if (help){
			mvprintw(0,0,"ymin:%f ymax:%f",MB.ymin,MB.ymax);
			mvprintw(1,0,"xmin:%f xmax:%f",MB.xmin,MB.xmax);
			mvprintw(2,0,"zlevel:%d",zlevel);
		}
	}
	endwin();
	return 0;
}

void draw_mb(Canvas MB)
{
	// inspired from the pseudocode of the wikipedia page
	double x0,y0;
	int width,height;
	getmaxyx(stdscr, height, width);
	char depth[6] = "  .:+#";
	short a;
	double xtemp;

	for (int i = 0; i < height; ++i)
	{
		y0 = MB.ymin + i * MB.yscale;
		for (int j = 0; j < width; ++j)
		{
			x0 = MB.xmin + j * MB.xscale;
			double x = 0.0,y = 0.0;
			int iteration = -1;

			while (x*x + y*y < 2*2  &&  ++iteration < max_iteration) {
			  xtemp = x*x - y*y + x0;
			  y = 2*x*y + y0;
			  x = xtemp;
			}
			a = iteration%5 +1;
			if (MB.ascii)
				mvaddch(i, j, depth[a]);
			else{
				attron(COLOR_PAIR(a) | A_REVERSE | A_BOLD);
				mvaddch(i, j, ' ');
				attroff(COLOR_PAIR(a) | A_REVERSE | A_BOLD);
			}
		}
	}
	mvaddch(height/2, width/2, '+');
}