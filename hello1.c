#include	<stdio.h>
#include	<curses.h>
#include    <ncurses.h>
#include    <string.h>
#include    <signal.h>
#include    <termios.h>
#include    <fcntl.h>
#include    <unistd.h>
#include    <sys/select.h>
void start_game(void);
int kbhit();
#define MAP_X 30
#define MAP_Y 22
void set_cr_noecho_mode(void);
void tty_mode(int);
void init_keyboard();
void close_keyboard();
int _getch();
static struct termios initial_settings, new_settings;
 
static int peek_character = -1;
typedef struct xy
{
    int x;
    int y;
};
struct xy charic;

main()
{

	initscr() ;		/* turn on curses	*/
				/* send requests	*/
	clear();			/* clear screen	*/
				/* row10,col20	*/
  
	mvaddstr(11,20,"-----------------------------------------\n");
	mvaddstr(12,20,"|                                       |\n");
	mvaddstr(13,20,"|               gong ju game            |\n");
	mvaddstr(14,20,"|                                       |\n");
	mvaddstr(15,20,"|      game start after 3 seconds       |\n");
	mvaddstr(16,20,"|       press spacebar to move !!!      |\n");
	mvaddstr(17,20,"|                                       |\n");
	mvaddstr(18,20,"-----------------------------------------\n");
	move(LINES-1,0);		/* move to LL	*/

	refresh();		/* update the screen	*/
	//getch();		/* wait for user input	*/
    sleep(3);
    clear();
    refresh();
    start_game();
	endwin();		/* turn off curses	*/
}

void start_game(void)
{
    init_keyboard();

    char input = '\0';
    //set_cr_noecho_mode();	
    charic.x = MAP_X;
    charic.y = MAP_Y-1;
    
    clear();
    move(charic.y-2,charic.x);
    addstr("O");
    move( charic.y-1,charic.x);
    addstr("|");
    move(charic.y,charic.x);
    addstr("A");

    refresh();
    while(1)
    {
        if(kbhit())
        {
            input=_getch();
            if(input == 'j')
            {
                if(charic.x-2>0)
                {
                    move(charic.y-2,charic.x);
                    addstr(" ");
                    move(charic.y-1,charic.x);
                    addstr(" ");
                    move(charic.y,charic.x);
                    addstr(" ");
                    charic.x = charic.x-2;
                    move(charic.y-2,charic.x);
                   
                    addstr("O");
                    move(charic.y-1,charic.x);
                    addstr("|");
                    move(charic.y,charic.x);
                    addstr("A");
                    refresh();
                }

            }
            else if(input == 'l')
            {
                if(charic.x+2<MAP_X*2)
                {
                    move(charic.y-2,charic.x);
                    addstr(" ");
                    move(charic.y-1,charic.x);
                    addstr(" ");
                    move(charic.y,charic.x);
                    addstr(" ");
                    charic.x = charic.x+2;
                    move(charic.y-2,charic.x);
                    addstr("O");
                    move(charic.y-1,charic.x);
                    addstr("|");
                    move(charic.y,charic.x);
                    addstr("A");
                    refresh();
                }
            }
    
        }
        else
        {
            
        }
    }
    close_keyboard();

}



int kbhit(void)
{
  
    unsigned char ch;
    int nread;
 
    if (peek_character != -1) 
        return 1;
    new_settings.c_cc[VMIN]=0;
    tcsetattr(0, TCSANOW, &new_settings);
    nread = read(0,&ch,1);
    new_settings.c_cc[VMIN]=1;
    tcsetattr(0, TCSANOW, &new_settings);
    if(nread == 1)
    {
        peek_character = ch;
        return 1;
    }
    return 0;
}
int _getch()
{
    char ch;
 
    if(peek_character != -1)
    {
        ch = peek_character;
        peek_character = -1;
        return ch;
    }
    read(0,&ch,1);
    return ch;
}
 
void close_keyboard()
{
    tcsetattr(0, TCSANOW, &initial_settings);
}
void init_keyboard()
{
    tcgetattr(0,&initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
}
