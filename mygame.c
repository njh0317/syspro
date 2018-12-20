#include	<stdio.h>
#include	<curses.h>
#include	<signal.h>
#include	<fcntl.h>
#include 	<string.h>
#include	<unistd.h>
#include	<termio.h> 

#define MAP_X 30
#define MAP_Y 32

#define ITEM MAP_X-1

#define LEFT 75
#define RIGHT 77
#define gotoxy(x,y) wmove(stdscr,y,x) //리눅스상에선 wmove라는 함수로 커서를 옮긴다.

#define DEAD 0
#define CLEAR 1

void endGame(int);
void init_field(void);
void init_game(void);
void start_game(void);
void print_item(void);
void check_good_hit(void);
void check_bad_hit(void);

void start_game(void);
int kbhit();
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

struct xy good_item[ITEM];
struct xy bad_item[ITEM];
struct xy gongju;

int delay = 200;
int max_bad_item = 0;
int max_good_item =0;
int count_bad_item = 0;
int count_good_item = 0;
int game_speed = 0;
int stage  = 0;
int life = 5;
int done = 0;
int dir = 1;
main()
{	
	initscr() ;		/* turn on curses	*/
	//set_cr_noecho_mode();	/* send requests	*/
	clear();			/* clear screen	*/
	
	mvaddstr(11,20,"-----------------------------------------\n");
	mvaddstr(12,20,"|                                       |\n");
	mvaddstr(13,20,"|               gong ju game            |\n");
	mvaddstr(14,20,"|                                       |\n");
	mvaddstr(15,20,"|         game start after 3 sec        |\n");
	mvaddstr(16,20,"|       press spacebar to move !!!      |\n");
	mvaddstr(17,20,"|                                       |\n");
	mvaddstr(18,20,"-----------------------------------------\n");
	move(LINES-1,0);		/* move to LL	*/
	
	refresh();
	sleep(3);
	clear();
	refresh();
	init_field();
	// 게임의 기본적인 초기화 작업
	init_game();

	// 게임을 시작한다
	start_game();
	
	endwin();
	// while( !done )			  /* the main loop */
		// pause();
	
	

   return 0;
}

void init_field(void)
{
	int col = 0;
	int row = 0;
	
	clear();

	for(row = 1; row<= MAP_X*2; row++)
	{
		{
			gotoxy(row, 1);
			addstr("-");
			refresh();
		}
	}
}

void init_game(void)
{
	int count = 0;
	max_good_item = 50;
	max_bad_item = 30;
	
	count_good_item = 0;
	count_bad_item = 0;
	
	game_speed = 100;
	
	for(count = 0 ; count < ITEM ; count ++)

	{
		//good_item[count].x = count+1;
		//good_item[count].y = rand()%50;
		//good_item[count].x *= 2;
		//good_item[count].y *= -1;
		
		bad_item[count].x = count+1;
		bad_item[count].y = rand()%50;
		bad_item[count].x *= 2;
		bad_item[count].y *= -1;
	}	
	gongju.x = MAP_X;
	gongju.y = MAP_Y-1;
		
	gotoxy(gongju.x, gongju.y-2);
	addstr("O");
	gotoxy(gongju.x, gongju.y-1);
	addstr("|");
	gotoxy(gongju.x, gongju.y);
	addstr("A");
		
	printw("\nSTAGE [%d]\n", stage+1);
	printw(">> avoid_bomb_num : %d\n", count_bad_item);
	printw(">> get_life_num : %d\n", life);
	refresh();

}

void print_bad_item(void)
{
	int count = 0;
	// 하트을 내리게 하는 함수
	for (count = 0; count < ITEM; count++)
    {
      // 땅에 떨어졌을 경우
      if (bad_item[count].y == MAP_Y - 1) 
      {
        gotoxy(gongju.x, gongju.y-2);
		addstr("O");
		gotoxy(gongju.x, gongju.y-1);
		addstr("|");
		gotoxy(gongju.x, gongju.y);
		addstr("A");
		// 기존 위치의 하트을 삭제한다.
        gotoxy(bad_item[count].x, bad_item[count].y);
        addstr(" ");
         // 떨어진 하트의 갯수 증가
        count_bad_item++;

        printw("\nSTAGE [ %d ]\n", stage + 1);
        printw(">> avoid_bomb_num : %d\n", count_bad_item);
		printw(">> get_life_num : %d\n", life);
		refresh();
        // 떨어진 하트의 갯수와 최대 하트의 갯수가 같을 경우
        if (max_bad_item == count_bad_item)
        {
			gotoxy(MAP_X, MAP_Y + 1);
			addstr("[ you avoid all bomb. press key ]");
			refresh();
			done = 1;
			endGame(CLEAR);
			return;
        }

         // 떨어진 하트의 갯수가 최대 갯수가 아닐 경우
         else if (max_bad_item > count_bad_item)
         {
            // 하트을 위에서 다시 내리게 한다
            bad_item[count].y = rand() % 50;
            bad_item[count].y *= -1;
         }
         continue;
      }

      // 하트을 밑으로 한칸 내린다
      bad_item[count].y = bad_item[count].y + 1;
      if (bad_item[count].y > 1)
      {
        // 기존 위치의 하트을 삭제한다.
        gotoxy(bad_item[count].x, bad_item[count].y - 1);
        addstr(" ");
         // 새로운 위치에 하트을 출력한다.
         gotoxy(bad_item[count].x, bad_item[count].y);
         addstr("B");
         refresh();
      }
    }
}

void check_bad_hit(void)
{
	int count = 0;
	for(count = 0; count < max_bad_item; count++)
	{
		// 사용자와 같은 열인지 확인

		if(bad_item[count].x == gongju.x)

		{

			// 같은 행인지 확인

			if(bad_item[count].y == gongju.y || bad_item[count].y == gongju.y-1|| bad_item[count].y == gongju.y-2)
			{
				gotoxy(MAP_X, MAP_Y+1);
				printw("\n[ you lose all life. press any button ]");
				life --;
				gotoxy(gongju.x, gongju.y-2);
				addstr("O");
				gotoxy(gongju.x, gongju.y-1);
				addstr("|");
				gotoxy(gongju.x, gongju.y);
				addstr("A");
				if(life == 0)
				{
					endGame(DEAD);
					return;
				}

			}

		}

	}

}

void endGame(int flag)
{
   // 사용자가 죽었을 경우
   if (flag == DEAD)
   {
	  getch();
      clear();
      gotoxy(MAP_X / 2, MAP_Y / 2);
      printw("## game over ##\n");
      refresh();
	 close_keyboard();
	 exit(1);
	
   }
   else // 사용자가 스테이지를 클리어했을 경우
   {
      getch();
      getch();
      clear();

      gotoxy(MAP_X / 2, MAP_Y / 2);
      printw("## stage clear ##\n");
      gotoxy(MAP_X / 2, MAP_Y / 2 + 1);
      printw("## level up ##\n");
      gotoxy(MAP_X / 2, MAP_Y / 2 + 2);
      printw("## press key ##\n");
	  refresh();
      getch();
      getch();
      clear();
      // 맵 출력
      init_field();
      // 스테이지 증가
      stage++;

      // 게임 초기화
      init_game();
      // 내리는 하트의 갯수 증가
      max_bad_item += stage * 30;

      // 게임 속도 증가
      game_speed -= stage*10;
   }

}

void start_game(void)
{
    init_keyboard();

    char input = '\0';
    //set_cr_noecho_mode();	
    //gongju.x = MAP_X;
    //gongju.y = MAP_Y-1;
    
    // clear();
    // move(gongju.y-2,gongju.x);
    // addstr("☺");
    // move( gongju.y-1,gongju.x);
    // addstr("△");
    // move(gongju.y,gongju.x);
    // addstr("A");

    refresh();
    while(1)
    {
        if(kbhit())
        {
            input=_getch();
            if(input == 'j')
            {
                if(gongju.x-2>0)
                {
                    move(gongju.y-2,gongju.x);
                    addstr(" ");
                    move(gongju.y-1,gongju.x);
                    addstr(" ");
                    move(gongju.y,gongju.x);
                    addstr(" ");
                    gongju.x = gongju.x-2;
                    move(gongju.y-2,gongju.x);
                   
                    addstr("O");
                    move(gongju.y-1,gongju.x);
                    addstr("|");
                    move(gongju.y,gongju.x);
                    addstr("A");
                    refresh();
                }

            }
            else if(input == 'l')
            {
                if(gongju.x+2<MAP_X*2)
                {
                    move(gongju.y-2,gongju.x);
                    addstr(" ");
                    move(gongju.y-1,gongju.x);
                    addstr(" ");
                    move(gongju.y,gongju.x);
                    addstr(" ");
                    gongju.x = gongju.x+2;
                    move(gongju.y-2,gongju.x);
                    addstr("O");
                    move(gongju.y-1,gongju.x);
                    addstr("|");
                    move(gongju.y,gongju.x);
                    addstr("A");
                    refresh();
                }
            }
    
        }
        else
        {
            usleep(game_speed*2000);
			print_bad_item();
        }
		check_bad_hit();
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


