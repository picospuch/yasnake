/* yasnake.c --- yet another snake game.

Copyright (c) 2008, 2019 picospuch.

This file is licensed under GPLv3.  */

#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <bios.h>

void debug(void);				/*display some imformation on the bottom of screen */

void opening(void);
void init_run(void);
void dir_select(void);
void run(void);
void draw(void);
void judge(void);
void fod(void);
void closing(void);
void update_shift_status(void);


typedef struct
{
	int x;
	int y;
} location;

location spirit,head,tail,*body,food;

typedef enum {UP=0x4800,DOWN=0x5000,LEFT=0x4b00,RIGHT=0x4d00,PAGEUP=0x4900,PAGEDOWN=0x5100,ESC=0x11b} scankey;

scankey d;						/* define keyboard direction ctrl */

int shift_mode = 0;

int n=6;						/* define snake's length(num of nodes is n) */
#define SPEED_MAX 1000
#define SPEED_MIN 50
#define CYCLE (1.0/1000.0) /* millisecond */
#define DELAY_HINT ((1.0/3000.0) / CYCLE)
int speed=200;					/* define speed */
long score=0;					/* define score */

int main(void)
{
	opening();
	init_run();
	while(1) {
      dir_select();
      run();
      draw();
      judge();
      if (!shift_mode) delay((int)((SPEED_MAX - speed) * DELAY_HINT));
    }
	return 0;
}

void opening(void)
{
	int gdriver=VGA,gmode=VGAHI,i,k;				/*-----initialize VGA graphic screen-----*/

	registerbgidriver(EGAVGA_driver);				/* make program run independently */
	registerbgifont(gothic_font);
	registerbgifont(sansserif_font);

	initgraph(&gdriver,&gmode,"");
	cleardevice();									/*-----------------------------------------------*/

	setcolor(DARKGRAY);
	rectangle(117,197,522,282);
	settextstyle(GOTHIC_FONT,HORIZ_DIR,10);
	setcolor(BLUE);
	outtextxy(120,160,"SHIFT");    		     /* DEFAULT:120,200*/
	settextstyle(SANS_SERIF_FONT,HORIZ_DIR,2);
	setcolor(LIGHTRED);
	outtextxy(40,448,"shift is yet another snake game, created for GameShell");   /*DEFAULT:192,448*/
	settextstyle(DEFAULT_FONT,HORIZ_DIR,1);
	setcolor(CYAN);
	outtextxy(190,468,"Copyright (c) 2008, 2019 picospuch");

	spirit.x=108;
	spirit.y=188;
	while (1)
	{
		setcolor(RED);
		outtextxy(spirit.x,spirit.y,"@");
		delay((int)((SPEED_MAX - speed) * DELAY_HINT));
		setcolor(BLACK);
		outtextxy(spirit.x,spirit.y,"@");
		if (spirit.x<524 && spirit.y==188)
		spirit.x+=8;
		else if (spirit.x>108 && spirit.y==284)
		spirit.x-=8;
		else if (spirit.y<284 && spirit.x==524)
		spirit.y+=8;
		else if (spirit.y>188 && spirit.x==108)
		spirit.y-=8;

		if (kbhit()) {
			k=bioskey(0);
			if (k==PAGEUP && speed>=SPEED_MIN) speed-=10;
			else if (k==PAGEDOWN && speed<=SPEED_MAX) speed+=10;
			else break;
		}
	}

	cleardevice();

}

void update_shift_status(void) {
  if (shift_mode) {
    setcolor(LIGHTGREEN);
  } else {
    setcolor(BLACK);
  }
  settextstyle(DEFAULT_FONT,HORIZ_DIR,3);
  outtextxy(320,448,"//SHIFTING//");

  settextstyle(DEFAULT_FONT,HORIZ_DIR,1);             /*set back to default font*/
}

void init_run()
{
	int i;

	setcolor(GREEN);
	rectangle(19,29,620,430);
	line(19,440,620,440);

	setcolor(LIGHTBLUE);
	settextstyle(DEFAULT_FONT,HORIZ_DIR,3);
	outtextxy(19,448,"SCORE:");
	setcolor(DARKGRAY);
	outtextxy(163,448,"000000");
	settextstyle(DEFAULT_FONT,HORIZ_DIR,1);             /*set back to default font*/

	body=(location *) calloc(n,sizeof(location));

	head.x=20+n*8;                                      /* initialize 'head' */
	head.y=30;                                          /*-------------------*/

	for (i=0;i<n;++i)									/* initialize 'body' */
		{body[i].x=20+(n-i-1)*8;body[i].y=30;}			/*-------------------*/

	tail.x=-1*8+20;                                     /* initialize 'tail' */
	tail.y=30;                                          /*-------------------*/

	setcolor(RED);
	outtextxy(20,30,"*******");                         /*draw it!*/

	d=RIGHT;

	if (getch()==0) getch();

	fod();
}

void dir_select(void)
{
	int k;
	while (kbhit()) k=bioskey(0);
	if (k==UP && d != DOWN)
		d=UP;
	else if (k==DOWN && d != UP)
		d=DOWN;
	else if (k==LEFT && d != RIGHT)
		d=LEFT;
	else if (k==RIGHT && d != LEFT)
		d=RIGHT;

    if ((k & 0xff) == 'a') {
      shift_mode = !shift_mode;
      update_shift_status();
    }

    if (k==ESC) exit(32);
}

void run(void)
{
	int i;

	tail.x=body[n-1].x;
	tail.y=body[n-1].y;

	for (i=n-1;i>0;--i) {
		body[i].x=body[i-1].x;
		body[i].y=body[i-1].y;
		}

	body[0].x=head.x;
	body[0].y=head.y;

	switch (d)
	{
	case UP:			 head.y-=8;  break;
	case DOWN:		 head.y+=8;  break;
	case LEFT:		 head.x-=8;  break;
	case RIGHT:		 head.x+=8;  break;
	}

	if (score%1000==0 && (head.x==620 || head.y==430 || head.x==-8+20 || head.y==-8+30))
		closing();
	else
		{
		if (head.x==620) head.x=20;							/*no wall,so reset the xy*/
		else if (head.y==430) head.y=30;					/*-----------------------*/
		else if (head.x==-8+20) head.x=612;					/*-----------------------*/
		else if (head.y==-8+30) head.y=422;					/*-----------------------*/
		}
}

void draw(void)
{
	setcolor(BLACK);
	outtextxy(tail.x,tail.y,"*");
	outtextxy(tail.x,tail.y,"@");
	outtextxy(tail.x,tail.y,"!");

	if (RED+score/1000==BLACK)
		setcolor(random(16));
	else
		setcolor(RED+score/1000);
	outtextxy(head.x,head.y,"*");
}

void judge(void)
{
	int i;
	char *s = 0;

	if (head.x==food.x && head.y==food.y)			/* if eat an apple then (lengthen it)&(place another apple) */
	{
      for (i=0;i<1000;i+=50) {sound(i);delay((int)(100 * DELAY_HINT));}
		nosound();

		body=(location *) realloc(body,++n*sizeof(location));

		settextstyle(DEFAULT_FONT,HORIZ_DIR,3);
		sprintf(s,"%06lu",score);
		setcolor(BLACK);
		outtextxy(163,448,s);
		score+=100;
		sprintf(s,"%06lu",score);
		setcolor(BROWN);
		outtextxy(163,448,s);
		settextstyle(DEFAULT_FONT,HORIZ_DIR,1);

		fod();
	}

	for (i=2;i<n;++i) {
		if (head.x==body[i].x && head.y==body[i].y)
		closing();
	}

}

void fod(void)
{
	food.x=20+random(75)*8;                          /*non-ANSI LIB FUNction,so i will change it in the future!*/
	food.y=30+random(50)*8;                          /*--------------------------------------------------------*/

	if (score%1000==0)
		{setcolor(LIGHTRED);outtextxy(food.x,food.y,"!");rectangle(19,29,620,430);}				/*draw a food*/
	else
		{setcolor(GREEN);outtextxy(food.x,food.y,"@");rectangle(19,29,620,430);}
}

void closing(void)
{
	char *s;
	while (!kbhit()) {
		setcolor(random(16));
		outtextxy(random(640),random(480),"$");
		setcolor(random(16));
		outtextxy(random(640),random(480),"Y");
	}

	cleardevice();
	if (getch()==0) getch();							/*read out last (kbhit()) key*/
	setcolor(WHITE);
	sprintf(s,"YOUR FINAL SCORE:%06lu",score);
	settextstyle(GOTHIC_FONT,HORIZ_DIR,4);
	outtextxy(64+random(32),64+random(32),s);
	outtextxy(128+random(32),360+random(32),"GAME OVER!");
	if (getch()==0) getch();

	closegraph();
	printf("NEVER GIVE UP!");
	getch();
	exit(0);
}

void debug(void)
{
	char s[100];
	sprintf(s,"x = %d y= %d",head.x,head.y);
	setcolor(WHITE);
	outtextxy(620-strlen(s)*8,460,s);
	setcolor(BLACK);
	outtextxy(620-strlen(s)*8,460,s);
}
