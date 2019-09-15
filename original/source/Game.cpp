/*
changes since last release:
- animated plants every 5 seconds
- spell matthew smith right
- fonts not used
- option to go through levels
 
   to do: , move over spider
*/

#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include "styx.h"
#include "styx_mn.h"
#include "styx_mus.h"

//externals
extern CAllegro myGame;  //allegro details
extern volatile int fps; //actual fps
extern volatile int frame_count; //updated every frame to keep fps in sync
extern volatile int digipurge;
extern BITMAP* gbuffer;
extern int gbufw; //quick access to gbuffer height/width
extern int gbufh;
extern int gbufw_c;
extern int gbufh_c;
extern int logo_shown;
extern int musicdat;

static char files[][11]={"styx01.lev","styx02.lev","styx03.lev","styx04.lev","styx05.lev"};
static char currentkeys[][15]={"Cursors, Space","QAOP Space","PLZX Space", "6758 9"};
static int currentfile=0;
static int currentkey=0;

static int sfish=FALSE;
static int sspider=FALSE;
static int leveldone=FALSE;
static int showblue=TRUE;
static int totmammyspiders=0;	//max in all spiders
static int totdemonspiders=0;	//max in all spiders

int plantswitch=FALSE;
int switchin=FALSE;
int gtl=0,gtr=0,gbl=0,gbr=0;
int demonawake;
int demonsleep;
int plantskill=FALSE;
int swimming=FALSE;

//globals
int gapcount=0;
DATAFILE* dat_main;
DATAFILE* dat_music;
int screendata[32*24+1];

int cheatmode=FALSE;
RGB* pal_logo;
int leftmostpos;
int fishblocks=0;
//note changing this requires all externs to be updated also
/*int player[][7]=     { { 8,SK_WLKL1_BMP,SK_WLKL2_BMP,SK_WLKL3_BMP,SK_WLKL2_BMP,-1,-1 }, //walking left
                       { 8,SK_WLKR1_BMP,SK_WLKR2_BMP,SK_WLKR3_BMP,SK_WLKR2_BMP,-1,-1 }, //walking right
                       { 20,SK_STND1_BMP,SK_STND2_BMP,SK_STND3_BMP,SK_STND2_BMP,-1,-1}, //standing
                       { 3,SK_JMP1_BMP,SK_JMP2_BMP,SK_JMP3_BMP,SK_JMP2_BMP,-1,-1}, //jumping
                       { 12,SK_DIZ1_BMP,SK_DIZ2_BMP,SK_DIZ3_BMP,SK_DIZ4_BMP,-1,-1}, //dizzy
                       { 3,SK_JMP1_BMP,SK_JMP2_BMP,SK_JMP3_BMP,SK_JMP2_BMP,-1,-1}, //fall down gap after hit
                       { 3,SK_JMP1_BMP,SK_JMP2_BMP,SK_JMP3_BMP,SK_JMP2_BMP,-1,-1}, //fall down gap
                       { 300,SK_DEAD1_BMP,SK_DEAD2_BMP,SK_DEAD3_BMP,SK_DEAD4_BMP,SK_DEAD5_BMP,-5} //dead
                     };
*/
int player[][7]=	{
						{8,XMAN_WALK1L_BMP,XMAN_WALK2L_BMP,-1,-1,-1,-1},
						{8,MAN_WALK1_BMP,XMAN_WALK2_BMP,-1,-1,-1,-1},
						{8,MAN_WALK1_BMP,MAN_WALK1_BMP,-1,-1,-1,-1},
						{8,MAN_WALK1_BMP,XMAN_WALK2_BMP,-1,-1,-1,-1},
						{8,MAN_WALK1_BMP,XMAN_WALK2_BMP,-1,-1,-1,-1},
						{8,XXXDEAD_BMP,XXXDEAD_BMP,-1,-1,-1,-1},
						{8,MAN_WALK1_BMP,XMAN_WALK2_BMP,-1,-1,-1,-1},
						{8,MAN_WALK1_BMP,XMAN_WALK2_BMP,-1,-1,-1,-1},
						{8,MAN_WALK1_BMP,XMAN_WALK2_BMP,-1,-1,-1,-1},
						{8,MAN_WALK1_BMP,XMAN_WALK2_BMP,-1,-1,-1,-1},
						{8,MAN_WALK1_BMP,XMAN_WALK2_BMP,-1,-1,-1,-1}
					};
int spiderlittle[2][7]={	{16,SPIDER1_BMP,XSPIDER2_BMP,-1,-1,-1,-1},
						{16,XXXDEAD_BMP,XXXDEAD_BMP,-1,-1,-1,-1}
					};
int spiderbig[7]=		{16,XMAMMY_BMP,-1,-1,-1,-1,-1};
int demon[][7]=			{
							{16,XDEMON2_BMP,-1,-1,-1,-1,-1},
							{16,DEMON1_BMP,-1,-1,-1,-1,-1},
						};
int fish[][7]=			{
						{20,XFISH1_BMP,XFISH2_BMP,-1,-1,-1,-1},
						{40,XXFISH3_BMP,XXFISH4_BMP,-2,-2,-2,-2}
						};

//auto-geordies only differ when walking and standing
//no real need to change when jumping or dizzy as difference is seen only when standing/walking
//auto-geordies are smaller than would-be mackems due to a generic gene
//that only pure geordies get which stunts their growth and reduces intelligence
//as opposed to those who knew something was wrong when they were young
//when they were bigger and brighter and new they weren't geordie :)
/*int autogeordie[][7]={ { 8,AG_WLKL1_BMP,AG_WLKL2_BMP,AG_WLKL3_BMP,AG_WLKL2_BMP,-1,-1 }, //walking left
                       { 8,AG_WLKR1_BMP,AG_WLKR2_BMP,AG_WLKR3_BMP,AG_WLKR2_BMP,-1,-1 }, //walking right
                       { 20,AG_STND1_BMP,AG_STND2_BMP,AG_STND3_BMP,AG_STND2_BMP,-1,-1}, //standing
                       { 3,SK_JMP1_BMP,SK_JMP2_BMP,SK_JMP3_BMP,SK_JMP2_BMP,-1,-1}, //jumping
                       { 12,SK_DIZ1_BMP,SK_DIZ2_BMP,SK_DIZ3_BMP,SK_DIZ4_BMP,-1,-1}, //dizzy
                       { 3,SK_JMP1_BMP,SK_JMP2_BMP,SK_JMP3_BMP,SK_JMP2_BMP,-1,-1}, //fall down gap after hit
                       { 3,SK_JMP1_BMP,SK_JMP2_BMP,SK_JMP3_BMP,SK_JMP2_BMP,-1,-1}, //fall down gap
                       { 300,SK_DEAD1_BMP,SK_DEAD2_BMP,SK_DEAD3_BMP,SK_DEAD4_BMP,SK_DEAD5_BMP,-5} //dead
                     };

int playersafe[6]={12,SK_DUGF1_BMP,SK_DUGF2_BMP,SK_DUGF3_BMP,SK_DUGF2_BMP,-1};
*/
int playerkeys[4][5]={ {KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN,KEY_SPACE},
                       {KEY_O, KEY_P, KEY_Q, KEY_A,KEY_SPACE},
                       {KEY_Z, KEY_X, KEY_P, KEY_L,KEY_SPACE},
                       {KEY_5, KEY_8, KEY_6, KEY_7,KEY_9}
                       };

int showplayernumber=FALSE; //show position until player moves
int autoshownumber=TRUE; //show number on startup
int playersleft=0;

int game_level=1;       //default level
int levelsafeavailable[8]; //player can go into safety zone - bottom layer included to make code simpler
int startspider=FALSE;
int stopspider=FALSE;
int startfish=FALSE;
int stopfish=FALSE;
int startg1=FALSE;	//no stops as gates 1 to 3 are latches
int startg2=FALSE;
int startg3=FALSE;
int startdemon=FALSE;
int stopdemon=FALSE;
int playerstartx=-1;
int playerstarty=-1;

//module level global control variables
static int music_level=MUSIC_ALL;
static int game_type=1;

//single player game carry over to next level
static int carryscore=0;
static int carrylives=MAX_LIVES_NORMAL; //as opposed to deathmatch

static const int hazstartnum=6; //how many of the below hazards there are
static int showfps=FALSE;
static int autoshowfps=TRUE;
static int noplayersshown=FALSE; //message box when all humans are dead
//colours
static int fg1=179; //main title text (yellow)
static int fg2=186; //main title text (red)
static int fg3=86; //blueish (For boxes/lines)
static int fgdark=155; //dark text colour
//static int bg1=255; //white
static int black=8;



//memory locks
volatile int game_time;
void game_timer()
{
 game_time++;
}
END_OF_FUNCTION(game_timer);


////////////////////////////
//main_loop
//main game loop
///////////////////////////
void game_loop()
{

 int choice=0;
 int keypress;
 int xoff;

 BITMAP* buffer=NULL;
 BITMAP* logo;  //repeat of game
 char* law1;
 char* law2;

 LOCK_VARIABLE(game_time);
 LOCK_FUNCTION(game_timer);

 int bx,by,bw,bh;
 int textxgap=5;
 int textygap=10+text_height(font);
 int restart=TRUE;
 char animtype[20]="";
 char gametype[20]="";
 FONT* font1;

 //set colour depth and transparent text mode
 text_mode(-1);
 //open main datafiles
 dat_main=dat_music=NULL;

 myGame.datload(&dat_main,MAINDAT_FILE,TRUE);

 if (musicdat==TRUE) myGame.datload(&dat_music,MUSICDAT_FILE,FALSE);
 if(dat_music==NULL) {music_level=MUSIC_NONE; myGame.noMusic(); myGame.noDigi();}

 //set correct palette and set playing flags
 //set bitmaps for options screen
 
 if(dat_main==NULL) myGame.abortsystem("Data file could not be found.");
 
 logo=(BITMAP*)dat_main[ZLOGO_BMP].dat;
 pal_logo=(RGB*)dat_main[MAIN_PAL].dat;
 set_palette(pal_logo);
 font1=(FONT*)dat_main[ZFONT1].dat;
 //font=font1;
 
 //get copyright stuff
 law1=(char*)malloc(sizeof(char)*dat_main[LAW_TXT].size);
 law2=(char*)malloc(sizeof(char)*dat_main[LAW2_TXT].size);
 if(law1!=NULL && law2!=NULL) {
   strncpy(law1,(char*)dat_main[LAW_TXT].dat,dat_main[LAW_TXT].size);
   strncpy(law2,(char*)dat_main[LAW2_TXT].dat,dat_main[LAW2_TXT].size);
   law1[dat_main[LAW_TXT].size-1]=0;
   law2[dat_main[LAW2_TXT].size-1]=0;
 }

 //main options loop
 //loops at beginning and after a redraw is required
 MusicPlay();


 do {
	//display main option screen
	//draw logo to left,heading to top, flag to bottom right
	//draw options to right - in buffer space
	set_palette(pal_logo);
	clear_keybuf();
	clear_to_color(screen,black);

	//setup positions
	xoff=position_logo(screen,logo);
	//heading and flag
	//assumes game_quit is the last one
	bx=textxgap+xoff+logo->w;
	by=SCREEN_H/2-(logo->h/2);
	bw=SVGA_W-bx;
	bh=logo->h;

	//screen space for menu display
	if(buffer==NULL) buffer=create_sub_bitmap(screen,bx,by,bw,bh);

	//output menu options
	clear_to_color(buffer,black);
	int textystart=(buffer->h)/2-((textygap*GAME1_QUIT)/2);

	//setup menu dependant upon level
	//main
	//play
	textprintf(buffer,font,textxgap,textystart+textygap,fg1,"0 Play With Current Level");

	//instructions
	textprintf(buffer,font,textxgap,textystart+(textygap*2),fg1,"1 Instructions");
	//credits
	textprintf(buffer,font,textxgap,textystart+(textygap*3),fg1,"2 Credits");
	//game type
	game_string(gametype);
	textprintf(buffer,font,textxgap,textystart+(textygap*4),fg1,"3 Game Type    : %s",gametype);
	textprintf(buffer,font,textxgap+text_length(font,"3 Game Type    : "),textystart+(textygap*4),fg2,"%s",gametype);
	//level
	textprintf(buffer,font,textxgap,textystart+(textygap*5),fg1,"4 Current Level: ");
	textprintf(buffer,font,textxgap+text_length(font,"4 Current Level: "),textystart+(textygap*5),fg2,"%s",files[currentfile]);
	//edit
	textprintf(buffer,font,textxgap,textystart+(textygap*6),fg1,"5 Game Editor");
	//music
	music_string(animtype);
	textprintf(buffer,font,textxgap,textystart+(textygap*8),fg1,"6 Music/Sound  : %s",animtype);
	textprintf(buffer,font,textxgap+text_length(font,"6 Music/Sound  : "),textystart+(textygap*8),fg2,"%s",animtype);
	textprintf(buffer,font,textxgap,textystart+(textygap*9),fg1,"7 Keys         : %s",currentkeys[currentkey]);
	textprintf(buffer,font,textxgap+text_length(font,"7 Keys         : "),textystart+(textygap*9),fg2,"%s",currentkeys[currentkey]);

	//quit
	textprintf(buffer,font,textxgap,textystart+(textygap*10),fg1,"9 Quit");
	if(law1!=NULL && law2!=NULL) 
	{
	 textout_centre(screen,font,law1,SCREEN_W/2,SCREEN_H-(text_height(font)*2),fg2);
	 textout_centre(screen,font,law2,SCREEN_W/2,SCREEN_H-text_height(font),fg2);
	}

   //if keypressed check which option, otherwise scroll scoreboard songs
	do 
	{
		//main processing loop while no keys are being pressed
		restart=TRUE;  //signify that inner loop ends now
		if(keypressed()) 
		{
			keypress=readkey();

			switch(keypress>>8) {
				case KEY_0: //play
				  if(game_type==1) currentfile=0;
				  play_game();
				  //redraw screen and scroll
				  break;
				case KEY_1: //instructions
				  //clear scroll first as don't want to put scroll in a timer
				  //and easiest way to make it work
				  instructions(buffer);
				  restart=TRUE;
				  //don't redraw screen as nothing changed, restart scroller
				  break;
				case KEY_2: //credits
				  //see instructions for this
				  credits(buffer);
				  restart=TRUE;
				  //see instructions for this
				  break;
				case KEY_3:	//game type
					gamelevel();
					if(game_type==1) currentfile=0;
					break;
				case KEY_4:	//level
					//if(game_type==0) currentfile++;
					currentfile++;
					if(currentfile>4) currentfile=0;
					break;
				case KEY_5:	//edit
					myGame.playdigi((SAMPLE*)dat_music[MAGIC_WAV].dat, MEDIUM, NORMAL,FALSE);
					edit_game();
					break;
				case KEY_6:	//sound
					music();
					break;
				case KEY_7:	//keys
					currentkey++;
					if(currentkey>3) currentkey=0;
					break;
				case KEY_C:
					if(cheatmode==TRUE) cheatmode=FALSE;
					else cheatmode=TRUE;
					break;
				case KEY_9:	//quit
					choice=-1;
					break;
				default:
				  restart=FALSE;
			}
		}
		else
		{
			restart=FALSE;
		}
		//come back so redraw
		if(switchin==TRUE)
		{
			switchin=FALSE;
			restart=TRUE;
			transmsgbox(screen,SCREEN_W,SCREEN_H,"Welcome back.", "press SPACE to continue",6,1);
			waitF7();
		}

	} while(restart==FALSE); //loop round while no keys are being pressed

 } while(choice!=-1);  //loop round after keys have been pressed

 //exit hands control back to main() which will exit nicely
 //destroy local bitmaps
 if(law1!=NULL) free(law1);
 if(law2!=NULL) free(law2);
 destroy_bitmap(buffer);
}

int position_logo(BITMAP* dest,BITMAP* src)
{
 int xoffset=60;
 blit(src,dest,0,0,xoffset,SCREEN_H/2-(src->h/2),src->w,src->h);
 return xoffset;
}

void gamelevel()
{
   if(game_type==0) game_type=1;
   else game_type=0;
}

void music()
{
 if(musicdat==FALSE) music_level=MUSIC_NONE;
 else {
   switch(music_level) {
     case MUSIC_ALL:
        music_level=MUSIC_MUSIC;
        myGame.noDigi();
        myGame.yesMusic();
        //no need to play music as just come from all
        break;
     case MUSIC_MUSIC:
        music_level=MUSIC_SOUND;
        myGame.stopmidi();
        myGame.noMusic();
        myGame.yesDigi();
        break;
     case MUSIC_SOUND:
        music_level=MUSIC_NONE;
        myGame.stopmidi();
        myGame.noMusic();
        myGame.noDigi();
        break;
     case MUSIC_NONE:
        music_level=MUSIC_ALL;
        myGame.yesDigi();
        myGame.yesMusic();
        MusicPlay();
        break;
   }
 }
}

void music_string(char* ret)
{
 if(music_level==MUSIC_ALL) sprintf(ret,"%s",STR_MUSIC_ALL);
 if(music_level==MUSIC_MUSIC) sprintf(ret,"%s",STR_MUSIC_MUSIC);
 if(music_level==MUSIC_SOUND) sprintf(ret,"%s",STR_MUSIC_SOUND);
 if(music_level==MUSIC_NONE) sprintf(ret,"%s",STR_MUSIC_NONE);
 if(*ret==0) sprintf(ret,"%s","You've found a bug!!!");
}

void game_string(char* ret)
{
 if(game_type==0) sprintf(ret,"%s","Repeat Same Level");
 if(game_type==1) sprintf(ret,"%s","Loop All 5 Levels");
 if(*ret==0) sprintf(ret,"%s","You've found a bug!!!!");
}

void instructions(BITMAP* buf)
{
 char ins1[][75]={  "Game Type:",
					" Loop Same Level - this is as the original",
					"  and repeats the selected level.",
					" Loop All 5 Levels - this goes through all",
					"  5 levels before increasing the difficulty.",
					" ",
					"Game Plan:",
                    " Progress through as many increasingly",
                    " difficult levels as you can.",
                    " ",
                    "Game Features:",
					" Deadly spiders, fish and sea plants kill",
                    " ",
                    " Killing the mother spider stops production and",
                    " killing the cloaked demon finishes the level.",
					" ",
                    " Your laser lasts a total of 5 seconds and",
                    " reduces in strength every second.  Use it ",
                    " wisely, however it is replenished every level.",
                    " ",
                    "Scoring:",
                    " Mother Spider and Demon score highly,",
                    " Fish and spiders score 30.",
                    " ",
                    "Triggers:",
					" Invisible triggers start the spiders and fish.",
					" Visible triggers block routes.",
					" ",
					" Only when fish are active plants kill."
 };
 char ins2[][75]= { "Game Editor:",
                    " Use the keys as shown in the menu to place",
                    " objects.",
                    " ",
                    " Bricks are solid unpassable objects.",
                    " Sea plants kill players, fish and spiders.",
					" Sea plants only kill when fish are activated.",
                    " Sea simply changes the players colour.",
					" ",
                    " You must place a player object on the screen.",
                    " You must place a Demon object on the screen",
                    " in order to finish a level.",
                    " ",
                    " Green traps 1,2 and 3 are shown on the game",
					" as green and when moved over they make visible",
					" any similarly numbered Red traps that stop ",
					" players, fish, spiders.",
					" ",
					" Green/Red 4,5,6 traps start/stop spiders and",
					" fish, they are invisible to the user.  The ",
					" spiders/fish start from the location of the ",
					" Mother spider, Demon or [invisible] fish start.",
					" ",
					" Multiple demons/spiders can be placed to ",
					" multiply the number of spiders for each level.",
					" ",
					" Multiple fish starts increase the location ",
					" where fish start but not the amount (i.e. more",
					" blocks means more random starting places.)"};

 char ins3[][75]= { "Game Keys: Movement",
                    " Up, Down, Left, Right, Fire",
                    " Keys are as set in the main menu",
                    " ",
					"Game Keys: In-Game",
					" F1 : On screen help (keys)",
					" F2 : Frame Rate on screen",
					" F3 : Auto Frame Rate (only shown when too low)",
					" F4 : Save screen as a bitmap",
					" F5 : Pause",
					" F6 : Toggle Gouraud shading (man in water)",
					" F8 : Toggle music",
					" F9 : Toggle sound",
					" F10: Very quick exit of entire game",
					" ESC: Abort current game to main menu",
					" ",
					"Cheat Mode:",
					" Yes"
 };



 do {
    subdisp(ins1,29,buf,FALSE,TRUE);
    if(!key[KEY_ESC]) subdisp(ins2,29,buf,FALSE,TRUE);
    if(!key[KEY_ESC]) subdisp(ins3,18,buf,FALSE,TRUE);
    //clear buffer if not escape
    if(key[KEY_ESC]==FALSE) clear_keybuf();
 } while(!key[KEY_ESC]);

}

void credits(BITMAP* buf)
{
 //display credits
 char credits1[][75]={ "Programming",
                       "Neil Walker",
                       "",
                       "Graphics",
                       "John Blythe",
                       "Bug Byte (Original Logo)",
                       "",
					   "Original Coder",
					   "Matthew Smith",
					   "",
                       "Sound Effects",
                       "www.hollywoodedge.com",
                       "",
                       "Music",
                       "Mozart - Moonlight Sonata in C",
                       "MIDI conversion unknown",
                       "",
                       "Testing",
                       "Retrospec",
                       "",
					   "No Particular Role",
					   "Stephen Walker, Aged 3"
 
 };

 do {
    //clear buffer if not escape
    if(!key[KEY_ESC]) subdisp(credits1,22,buf,TRUE,TRUE);
    //clear buffer if not escape
    if(key[KEY_ESC]==FALSE) clear_keybuf();
 } while(!key[KEY_ESC]);

}


void subdisp(char list[][75],int rows,BITMAP* buf,int hcentreflag,int vcentreflag)
{
 //draw list to buf with horizontal/vertical centreing
 BITMAP* newdisplay;
 int y=0;
 int loop=0;
 newdisplay=create_bitmap(buf->w,buf->h);

 //draw on hidden buffer
 if(vcentreflag==TRUE) y=newdisplay->h/2-(text_height(font)*rows/2);
 clear_to_color(newdisplay,black);
 while(loop<rows) {
   if(hcentreflag) textout_centre(newdisplay,font,list[loop],newdisplay->w/2,y,fg1);
   else textout(newdisplay,font,list[loop],0,y,fg1);
   y+=text_height(font);
   loop++;
 }

 //add bottom caption
 textout(newdisplay,font,"Press A Key. ESC Exits.",0,newdisplay->h-text_height(font),fg2);
 //flash it on screen
 acquire_screen();
 //wipe(newdisplay,buf,wipetype);
 blit(newdisplay,buf,0,0,0,0,buf->w,buf->h);
 release_screen();
 clear_keybuf();
 readkey();
 destroy_bitmap(newdisplay);
}


int quit()
{
 //create faded palette
 //draw translucent box
 //wait for resonse
 return -1; //quit
}


//read screen data into global array
void readscreen(char* fn)
{
	int numread;
	FILE* stream;
	int loop1;
   if( (stream = fopen( fn, "rb" )) != NULL )
   {
      /* Attempt to read in 25 characters */
      numread = fread( screendata, sizeof( int ), 32*24+1, stream );
      if(numread!=(32*24)) 
	  {
		transmsgbox(screen,SCREEN_W,SCREEN_H,"Invalid file size.  Resetting data.", "press SPACE to continue",fg1,fgdark);
		waitF7();
		  for(loop1=0;loop1<(32*24+1);loop1++) *(screendata+loop1)=0;
	  }
	  fclose(stream);
   }
   else
   {
	   for(loop1=0;loop1<(32*24+1);loop1++) *(screendata+loop1)=0;
   }

}


//draw screendata to buffer
void drawscreen(BITMAP* buf,int debugbits=FALSE,int showplayerblock=FALSE,int xoff=XOFF,int yoff=YOFF)
{
	int loop1,loop2;
	int item;
	int gate=0;
	totmammyspiders=totdemonspiders=fishblocks=0;
	
	clear_to_color(buf,black);
	for(loop1=0;loop1<24;loop1++)
	{
		for(loop2=0;loop2<32;loop2++)
		{
			item=screendata[(loop1*32)+loop2];
			//get and remove gate (last 4 binary digits)
			gate=item>>4;
			item&=0x0f;

			switch(item)
			{
			case SEA1_BMP:
			case WALL_BMP:
			case SPIDER1_BMP:
			case MAN_WALK1_BMP:
			case DEMON1_BMP:
			case XMAMMY_BMP:
			case WBFISH_BMP:	//two types, standard is fish start, WBFISH_BMP&0xf is sea water
				//draw block.  game blocks only drawn in edit mode
				if((item!=SEA1_BMP && item!=MAN_WALK1_BMP && item!=WBFISH_BMP && item!=DEMON1_BMP && item!=SPIDER1_BMP && item!=XMAMMY_BMP ) || showplayerblock==TRUE)
					blit((BITMAP*)dat_main[item].dat,buf,0,0,xoff+BLOCKSIZE*loop2,yoff+BLOCKSIZE*loop1,((BITMAP*)dat_main[item].dat)->w,((BITMAP*)dat_main[item].dat)->h);

				if(item==WBFISH_BMP && gate==0) fishblocks++;
				if(item==WBFISH_BMP) blit((BITMAP*)dat_main[item].dat,buf,0,0,xoff+BLOCKSIZE*loop2,yoff+BLOCKSIZE*loop1,((BITMAP*)dat_main[item].dat)->w,((BITMAP*)dat_main[item].dat)->h);
				if(item==XMAMMY_BMP) totmammyspiders++;
				if(item==DEMON1_BMP) totdemonspiders++;
				if(plantswitch==TRUE && item==SEA1_BMP)
				{
					blit((BITMAP*)dat_main[SEA1_BMP].dat,buf,0,0,xoff+BLOCKSIZE*loop2,yoff+BLOCKSIZE*loop1,((BITMAP*)dat_main[SEA1_BMP].dat)->w,((BITMAP*)dat_main[SEA1_BMP].dat)->h);
				}
				if(plantswitch==FALSE && item==SEA1_BMP)
				{
					blit((BITMAP*)dat_main[XSEA2_BMP].dat,buf,0,0,xoff+BLOCKSIZE*loop2,yoff+BLOCKSIZE*loop1,((BITMAP*)dat_main[XSEA2_BMP].dat)->w,((BITMAP*)dat_main[XSEA2_BMP].dat)->h);
				}

				break;
			case TRAP1_BMP:
				//draw start traps only if not triggered
				if((gate==1 && startg1==FALSE) || showplayerblock==TRUE)
					blit((BITMAP*)dat_main[item].dat,buf,0,0,xoff+BLOCKSIZE*loop2,yoff+BLOCKSIZE*loop1,BLOCKSIZE,BLOCKSIZE);
				if((gate==2 && startg2==FALSE) || showplayerblock==TRUE)
					blit((BITMAP*)dat_main[item].dat,buf,0,0,xoff+BLOCKSIZE*loop2,yoff+BLOCKSIZE*loop1,BLOCKSIZE,BLOCKSIZE);
				if((gate==3 && startg3==FALSE) || showplayerblock==TRUE)
					blit((BITMAP*)dat_main[item].dat,buf,0,0,xoff+BLOCKSIZE*loop2,yoff+BLOCKSIZE*loop1,BLOCKSIZE,BLOCKSIZE);
				break;
			case BLOCKED1_BMP:
				//check if need to show locked gates
				if((gate==1 && startg1==TRUE) || showplayerblock==TRUE)
					blit((BITMAP*)dat_main[item].dat,buf,0,0,xoff+BLOCKSIZE*loop2,yoff+BLOCKSIZE*loop1,BLOCKSIZE,BLOCKSIZE);
				if((gate==2 && startg2==TRUE) || showplayerblock==TRUE)
					blit((BITMAP*)dat_main[item].dat,buf,0,0,xoff+BLOCKSIZE*loop2,yoff+BLOCKSIZE*loop1,BLOCKSIZE,BLOCKSIZE);
				if((gate==3 && startg3==TRUE) || showplayerblock==TRUE)
					blit((BITMAP*)dat_main[item].dat,buf,0,0,xoff+BLOCKSIZE*loop2,yoff+BLOCKSIZE*loop1,BLOCKSIZE,BLOCKSIZE);
				break;
			default:
				break;
			}

			//add number
			if((item==TRAP1_BMP || item==BLOCKED1_BMP) && showplayerblock==TRUE)
				textprintf(buf,font,xoff+BLOCKSIZE*loop2,yoff+BLOCKSIZE*loop1,fg1,"%d",gate);
			//add debug
			if(debugbits==TRUE)
				textprintf(buf,font,xoff+BLOCKSIZE*loop2,yoff+BLOCKSIZE*loop1,fgdark,"%d",item|(gate<<4));

		}
	}
}

//edit the game screen
void edit_game()
{
	static int debugbits=FALSE;
	int loop1;
	int item=0;
	int quit;
	int x,y;
	int bDo=FALSE;
	int keypress;
	int numread;
	int rightoffset;
	int topoffset;
	int gap;
	int yy=0;
	int craptimer=0;
	int currentopen=0;
	int currentclose=1;
	FILE* stream;
	BITMAP* curr;
	int toff=text_length(font,"xxxxxxxxxxxx");

	clear_to_color(screen,black);
	//read file every time
	readscreen(files[currentfile]);

	gap=BLOCKSIZE+8;
	rightoffset=513;
	topoffset=20;
    //process system
	quit=FALSE;
	x=y=0;
	while(quit==FALSE)
	{

	   //display it
		bDo=FALSE;
		drawscreen(gbuffer,debugbits,TRUE,0,0);
		//display menu
		yy=0;
		textprintf(gbuffer,font,rightoffset,topoffset+yy,fg1,"1 Sea");
		curr=(BITMAP*)dat_main[SEA1_BMP].dat;
		blit(curr,gbuffer,0,0,rightoffset+toff,topoffset+yy,curr->w,curr->h);
		yy+=gap;
		
		textprintf(gbuffer,font,rightoffset,topoffset+yy,fg1,"2 Wall");
		curr=(BITMAP*)dat_main[WALL_BMP].dat;
		blit(curr,gbuffer,0,0,rightoffset+toff,topoffset+yy,curr->w,curr->h);
		yy+=gap;

		textprintf(gbuffer,font,rightoffset,topoffset+yy,fg1,"3 Player");
		curr=(BITMAP*)dat_main[MAN_WALK1_BMP].dat;
		blit(curr,gbuffer,0,0,rightoffset+toff,topoffset+yy,curr->w,curr->h);
		yy+=gap;
		
		textprintf(gbuffer,font,rightoffset,topoffset+yy,fg1,"4 Demon");
		curr=(BITMAP*)dat_main[DEMON1_BMP].dat;
		blit(curr,gbuffer,0,0,rightoffset+toff,topoffset+yy,curr->w,curr->h);
		yy+=gap;
		yy+=gap;
		
		textprintf(gbuffer,font,rightoffset,topoffset+yy,fg1,"5 Spider");
		curr=(BITMAP*)dat_main[XMAMMY_BMP].dat;
		blit(curr,gbuffer,0,0,rightoffset+toff,topoffset+yy,curr->w,curr->h);
		yy+=gap;
		yy+=gap;
		
		textprintf(gbuffer,font,rightoffset,topoffset+yy,fg1,"6 Open Gate");
		curr=(BITMAP*)dat_main[TRAP1_BMP].dat;
		blit(curr,gbuffer,0,0,rightoffset+toff,topoffset+yy,curr->w,curr->h);
		yy+=(gap/2+gap/4);
		textprintf(gbuffer,font,rightoffset,topoffset+yy,fg1,"7 Shut Gate");
		curr=(BITMAP*)dat_main[BLOCKED1_BMP].dat;
		blit(curr,gbuffer,0,0,rightoffset+toff,topoffset+yy,curr->w,curr->h);
		yy+=gap;

		textprintf(gbuffer,font,rightoffset+gap/2,topoffset+yy,fg3,"-1 Gate(s) #1");
		yy+=(gap/2);
		
		textprintf(gbuffer,font,rightoffset+gap/2,topoffset+yy,fg3,"-2 Gate(s) #2");
		yy+=(gap/2);

		textprintf(gbuffer,font,rightoffset+gap/2,topoffset+yy,fg3,"-3 Gate(s) #3");
		yy+=(gap/2);

		textprintf(gbuffer,font,rightoffset+gap/2,topoffset+yy,fg3,"-4 Spiders");
		yy+=(gap/2);

		textprintf(gbuffer,font,rightoffset+gap/2,topoffset+yy,fg3,"-5 Fish");
		yy+=(gap/2);

		textprintf(gbuffer,font,rightoffset+gap/2,topoffset+yy,fg3,"-6 Demon");
		yy+=(gap);

		textprintf(gbuffer,font,rightoffset,topoffset+yy,fg1,"8 Fish Start");
		curr=(BITMAP*)dat_main[WBFISH_BMP].dat;
		blit(curr,gbuffer,0,0,rightoffset+toff,topoffset+yy,curr->w,curr->h);
		yy+=gap;

		textprintf(gbuffer,font,rightoffset,topoffset+yy,fg1,"9 Sea Water");
		curr=(BITMAP*)dat_main[WBFISH_BMP].dat;
		blit(curr,gbuffer,0,0,rightoffset+toff,topoffset+yy,curr->w,curr->h);
		yy+=gap;

		textprintf(gbuffer,font,rightoffset,topoffset+yy,fg1,"C Clear Block");
		yy+=gap;

		textprintf(gbuffer,font,rightoffset,topoffset+yy,fg1,"Z Clear All");
		yy+=gap;

		textprintf(gbuffer,font,rightoffset,topoffset+yy,fg1,"D Debug bits");
		yy+=gap;

		textprintf(gbuffer,font,rightoffset,topoffset+yy,fg1,"S Save");
		yy+=gap;
		
		textprintf(gbuffer,font,rightoffset,topoffset+yy,fg1,"Q Quit");

		//set position
		craptimer++;
		if(craptimer>40)	rect(gbuffer, x*BLOCKSIZE, y*BLOCKSIZE, x*BLOCKSIZE+(BLOCKSIZE-1), y*BLOCKSIZE+(BLOCKSIZE-1), fg3);
		else				rect(gbuffer, x*BLOCKSIZE, y*BLOCKSIZE, x*BLOCKSIZE+(BLOCKSIZE-1), y*BLOCKSIZE+(BLOCKSIZE-1), fg1);
		if(craptimer>80)	craptimer=0;

		//process keys
		if(keypressed()) 
		{
			keypress=readkey();

			switch(keypress>>8) 
			{
				case KEY_UP:
					y--;
					break;
				case KEY_DOWN:
					y++;
					break;
				case KEY_LEFT:
					x--;
					break;
				case KEY_RIGHT:
					x++;
					break;
				case KEY_Q: //quit
					quit=TRUE;
					break;
				case KEY_S:	//save
					if( (stream = fopen( files[currentfile], "wb" )) == NULL )
					{
						transmsgbox(screen,SCREEN_W,SCREEN_H,"Errors creating file to save!", "press SPACE to continue",fg1,fgdark);
						waitF7();
					}
					else
					{
						numread = fwrite( screendata, sizeof( int ), 32*24, stream );
						if(numread!=(32*24))
						{
							transmsgbox(screen,SCREEN_W,SCREEN_H,"Errors saving!", "press SPACE to continue",fg1,fgdark);
							waitF7();
						}
						else
						{
							transmsgbox(screen,SCREEN_W,SCREEN_H,"Screen Saved.", "press SPACE to continue",fg1,fgdark);
							waitF7();
						}
						fclose(stream);
					}
					break;
				case KEY_1:
					item=SEA1_BMP;
					bDo=TRUE;
					break;
				case KEY_2:
					item=WALL_BMP;
					bDo=TRUE;
					break;
				case KEY_3:
					item=MAN_WALK1_BMP;
					bDo=TRUE;
					break;
				case KEY_4:
					item=DEMON1_BMP;
					bDo=TRUE;
					break;
				case KEY_5:
					item=XMAMMY_BMP;
					bDo=TRUE;
					break;
				case KEY_6:	//open a gate
					//1 to 6
					currentopen++;
					if(currentopen>=7) currentopen=1;
					item=TRAP1_BMP;
					bDo=TRUE;
					break;
				case KEY_7: //close a gate
					//1 to 6
					currentclose++;
					if(currentclose>=7) currentclose=1;
					item=BLOCKED1_BMP;
					bDo=TRUE;
					break;
				case KEY_8: //fish start
					item=WBFISH_BMP;
					bDo=TRUE;
					break;
				case KEY_9:	//sea water
						item=WBFISH_BMP;
						item|=16;
						bDo=TRUE;
						//textprintf(screen,font,100,100,fg1,"%d",item);
					
						break;
				case KEY_C:  //clear block
					item=0;
					bDo=TRUE;
					break;
				case KEY_Z:		//clear all
					for(loop1=0;loop1<(32*24+1);loop1++) *(screendata+loop1)=0;
					break;
				case KEY_D:	//debug
					if(debugbits==TRUE) debugbits=FALSE;
					else debugbits=TRUE;
					break;
			}
			if(x<0) x=31;
			if(x>31)x=0;
			if(y<0) y=23;
			if(y>23)y=0;
			if(bDo==TRUE) 
			{
				//shift gate left 4 to make it easy to identify
				if(item==BLOCKED1_BMP) item |= (currentclose<<4);
				if(item==TRAP1_BMP) item |= (currentopen<<4);

				*(screendata+(y*32+x))=item;
			}
		}

		//draw screen
		acquire_screen();
		vsync();
		textprintf(gbuffer,font,550,0,fg3,"X:%d Y:%d",x,y);
		blit(gbuffer,screen,0,0,0,0,gbuffer->w,gbuffer->h);
		release_screen();
		myGame.purgedigi(TRUE);	//kill all sounds
		
	}
}

void play_game()
{
//game loop proper

  //initialisation
  //set palette and install game timer
	int loop1,loop2;
	playerstartx=playerstarty=-1;
  readscreen(files[currentfile]);
  set_palette((RGB*)dat_main[MAIN_PAL].dat);
  game_time=0;
  int req_fps=FRAME_FPS_TARGET;
  install_int_ex(game_timer,BPS_TO_TIMER(req_fps));

  //reset control flags
  int done=FALSE;
  int draw=TRUE;
  game_time=0;
  noplayersshown=FALSE;

  //start a new game
  clear_to_color(screen,black);
  //validate that the user is there
	for(loop1=0;loop1<32;loop1++) 
	{
		for(loop2=0;loop2<24;loop2++)
		{
			if( screendata[loop2*32+loop1] == MAN_WALK1_BMP) 
			{
				playerstartx=loop1*BLOCKSIZE;
				playerstarty=loop2*BLOCKSIZE;
			}
		}
	}


	if(playerstartx==-1 || playerstarty==-1)
	{
        transmsgbox(screen,SCREEN_W,SCREEN_H,"The player has not been set for this level.  Edit the screen and try again!","press SPACE",fg3,fgdark);
        waitF7();
	}
	else
	{
	  //if there is a hack in the game, the line below is it
	  //for some reason in deatmatch mode effects (nodigi) gets set to TRUE (effects off) after every level
	  if(music_level==MUSIC_SOUND || music_level==MUSIC_ALL) myGame.yesDigi();
	  else myGame.noDigi();

	  flickscreen(TRUE);
	  newlevel(FALSE);    //start the game fresh
	  do {
		//target is 30fps normal (double for smooth)
		//this first loop should take minimal time
		//so if the blit/drawing section below takes one 1/30 of second then target
		//will just be 1 so loop is called just once
		//then if this loop is nothing then we have 30fps
		//if blit/drawing takes 2/30 second then on entry to the loop below
		//target will be 2 and screen will not update fast enough - player will move more

		//therefore can control speed by required speed per second and fps
		//if we want 10 pixels per second and target is 30fps then
		//if we increase x by 10/30 (i.e. 0.3 per frame) then we move 10 pixels in one second
		//if fps is 100 then x is increased by  10/100 (ie. .1 per frame)
		//if achieve 60fps when we require just 30 then
		//loop is ignored twice and same screen left

		//update items if a frame is up
		//if not then system is too fast so don'd do anything
		//this variable is in an interrupt timer every FPS (30 or 60)
		while(game_time) {
		  //first get any game keys
		  done=gameInput(); //read game keys (not player movement) - done set to TRUE if abort by user
		  //if not quit then update player and computer players
		  if(done==FALSE) done=playerUpdate();
		  else break; //exit if quit
		  //if not game over then update the rest
		  if(done==FALSE) {
			hazardUpdate();   //as above for hazards
			//this method updates all CSprite (and subclasses)
			//movements, animations and collisions
			CSprite::NextFrame();
			draw=TRUE;
		  } else {
			//no point redrawing if quitting
			draw=FALSE;
			break; //exit
		  }
		  //decrement game count for next loop
		  game_time--;
		}
		//should exit loop every FRAMERATE/second second if on time
		//update display if frame has been drawn
		//if not then system is too fast so don't do anything
		if(draw) {
		  //function call order is z order
		  gameDraw(); //background and platforms
		  hazardDraw(); //hazards first
		  otherDraw();  //other stuff, i.e. sideline, etc.
		  playerDraw(); //now player last, always top
		  lastDraw(); //on top stuff

		  //now update the screen
		  //this is the simple double buffer technique
		  acquire_screen();
		  //vsync();
		  blit(gbuffer, screen, 0, 0, 0, 0,gbuffer->w,gbuffer->h);
		  release_screen();
		  frame_count++; //update every drawn screen - should match fps which is required FPS or less, cannot be higher
		  draw=FALSE;
		}
	
		//timer event
		//clear up any finished samples.  This may be better inside the processing loop to only clear up when free time.
		if(switchin==TRUE)
		{
			switchin=FALSE;
			transmsgbox(screen,SCREEN_W,SCREEN_H,"Welcome back. I'm afraid I didn't pause it for you.", " You have probably died a few times ;-)  press SPACE to continue",6,1);
			waitF7();
		}
		if(digipurge==TRUE) {
		  digipurge=FALSE;
		  if(plantswitch==TRUE) plantswitch=FALSE;
		  else plantswitch=TRUE;
		  myGame.purgedigi();
		}
	
	  } while(done==FALSE);
	}
  //global buffer bitmap is destroyed on exit
  remove_int(game_timer); //stop timer and remove it
  CSprite::KillAll();      //remove any objects
  myGame.killdigi(); //stop all known samples

  //set music menu to what it is now
  if(myGame.getMusic()==TRUE && myGame.getDigi()==TRUE) music_level=MUSIC_ALL;
  else if(myGame.getMusic()==TRUE && myGame.getDigi()==FALSE) music_level=MUSIC_MUSIC;
  else if(myGame.getMusic()==FALSE && myGame.getDigi()==TRUE) music_level=MUSIC_SOUND;
  else music_level=MUSIC_NONE;

}

int gameInput()
{
 //generic key presses, i.e. not player related
 //show fps
 if(key[KEY_F1]) gamehelp();

 if(key[KEY_F2]) {
   if(showfps==FALSE) showfps=TRUE;
   else showfps=FALSE;
 }
 //auto frame warning on/off
 if(key[KEY_F3]) {
   if(autoshowfps==FALSE) autoshowfps=TRUE;
   else autoshowfps=FALSE;
 }

 //save current screen
 if(key[KEY_F4]) {
   BITMAP* tmp=create_sub_bitmap(screen,0,0,SCREEN_W,SCREEN_H);
   save_bitmap("styxdump.bmp",tmp,(RGB*)dat_main[MAIN_PAL].dat);
   destroy_bitmap(tmp);

 }

 //pause mode
 if(key[KEY_F5]) {
   gamepause();
 }


 if(key[KEY_F6])
 {
	 if(showblue==TRUE) 
	 {
		 showblue=FALSE;
		transmsgbox(screen,SCREEN_W,SCREEN_H,"Gouraud Shading is off","press SPACE.",fg3,fgdark);
	 }
	 else 
	 {
		 showblue=TRUE;
		transmsgbox(screen,SCREEN_W,SCREEN_H,"Gouraud Shading is on","press SPACE.",fg3,fgdark);
	 }
	 waitF7();
   game_time=1; //reset interrupt timer
 }

 //F7 allocated to restore from pause, etc.

 //toggle music
 if(key[KEY_F8]) {
   if(myGame.getMusic()==TRUE)        {
     myGame.noMusic();
     myGame.stopmidi();
     transmsgbox(screen,SCREEN_W,SCREEN_H,"Music Is Off","press SPACE.",fg3,fgdark);
   }
   else {
     myGame.yesMusic();
     MusicPlay();
     transmsgbox(screen,SCREEN_W,SCREEN_H,"Music Is On","press SPACE.",fg3,fgdark);
   }
   waitF7();
   game_time=1; //reset interrupt timer

 }
 //toggle digi
 if(key[KEY_F9]) {
   if(myGame.getDigi()==TRUE) {
     myGame.killdigi();
     myGame.noDigi();
     transmsgbox(screen,SCREEN_W,SCREEN_H,"Effects Are Off","press SPACE.",fg3,fgdark);
   }
   else {
     myGame.yesDigi();
     transmsgbox(screen,SCREEN_W,SCREEN_H,"Effects Are On","press SPACE.",fg3,fgdark);
   }
   waitF7();
   game_time=1; //reset interrupt timer
 }

 if(key[KEY_F10]) myGame.abortsystem("Quick quit selected");

 if(key[KEY_F12]) {
//   debugitems();
 }

 //escape
 if(key[KEY_ESC]) {
   return gamequit();
 }


 //if all else fails return as no quit
 return FALSE;
}


int playerUpdate()
{
 //player only related key presses not part of sprite routine (i.e. not movement)
 //dont call nextframe as called in main loop
 int ret=FALSE;
 int currentdead=FALSE;
 int loop1,loop2;

 playersleft=0;
 //update player counts
 int lives=2;
 int thescore=0;
 CSprite* players=CSprite::first;
 while(players) {
   if(players->type==PLAYER) 
   {
		lives=((CPlayer*)players)->lives;
		thescore=((CPlayer*)players)->score;
		if( ((CPlayer*)players)->isdead()==TRUE) currentdead=TRUE;
   }
   players=players->next;
 }

  //check if game over
 if(lives<1) 
 {
	 clear_keybuf();
   transmsgbox(screen,SCREEN_W,SCREEN_H,"Game over.  press SPACE to continue.","",fg3,fgdark);
   waitF7();
   char code[256];
   int scores[2]={thescore,game_level};
   int sizes[2]={4,2};
   uuencode_generic();
   char message1[50];
   clear(screen);
   clear_keybuf();
   sprintf(message1,"High Score code: %s",code);
   transmsgbox(screen,SCREEN_W,SCREEN_H,message1,"Go to retrospec.sgn.net to enter code",fg3,fgdark);
   waitF7();
	set_config_file("score.txt");
	set_config_string("Code", "Key", code);
	set_config_file(CONFIG_FILE);
	clear_keybuf();
	clear(screen);
   transmsgbox(screen,SCREEN_W,SCREEN_H,"The code has been written to score.txt","Press SPACE",fg3,fgdark);
   waitF7();

   game_level=0;
   currentfile=0;
   waitF7();
	ret=TRUE; //exit
 }
 else
 {
	//check if dead
	if(currentdead==TRUE)
	{
		//transmsgbox(screen,SCREEN_W,SCREEN_H,"Killed.  press SPACE to continue.","",fg3,fgdark);
		//waitF7();
		currentdead=FALSE;
		sfish=FALSE;
		sspider=FALSE;
		newlevel(TRUE);
		return FALSE;
	}
	else
	{
		//check if finished level
		//set leveldone here


		if(leveldone==TRUE) 
		{
			transmsgbox(screen,SCREEN_W,SCREEN_H,"Level Finished.  press SPACE to continue.","",fg3,fgdark);
			waitF7();
			flickscreen(FALSE);
			if(game_type==0) game_level++;
			else
			{
				playerstartx=playerstarty=-1;
				if(game_type==1) 
				{
					currentfile++;
					if(currentfile==5) {currentfile=0; game_level++;}
					readscreen(files[currentfile]);
					for(loop1=0;loop1<32;loop1++) 
					{
						for(loop2=0;loop2<24;loop2++)
						{
							if( screendata[loop2*32+loop1] == MAN_WALK1_BMP) 
							{
								playerstartx=loop1*BLOCKSIZE;
								playerstarty=loop2*BLOCKSIZE;
							}
						}
					}


					if(playerstartx==-1 || playerstarty==-1)
					{
						transmsgbox(screen,SCREEN_W,SCREEN_H,"The player has not been set for this level.  Edit the screen and try again!","press SPACE",fg3,fgdark);
						waitF7();
						ret=TRUE;
					}
				}
			}

			flickscreen(TRUE);
			newlevel(TRUE);   //if finished level start new level
			leveldone=FALSE; //reset level to not being completed
		}
	}
 }

 return ret;

}

void hazardUpdate()
{
 //hazard related update
 //dont call nextframe as called in main loop

 //delete any that need deleting - i.e. fireballs
 //will also delete any other flagged, but is only fireballs in this game
 int loop1,loop2;
 int foundspider1;
 int foundspider2;
 CSprite* item=CSprite::first;
 CSprite* next=item;
 while(item) {
   foundspider1=foundspider2=-1;
   next=item->next;
   if(item->flagdelete) 
   {
	   if(item->type==DEMON) { leveldone=TRUE; sfish=sspider=FALSE; }
	   //if(item->type==BIGSPIDER) { sspider=FALSE; stopspider=TRUE; }
	   if(item->type==BIGSPIDER) { sspider=FALSE; }
	   CSprite::KillItem(item);
   }

	//stop items
   if(stopspider==TRUE && item->type==LITTLESPIDER) {CSprite::KillItem(item);sspider=FALSE;}
   else if(stopfish==TRUE && item->type==FISH) {CSprite::KillItem(item);sfish=FALSE;}
   else if(stopdemon==TRUE && item->type==DEMONSPIDER) {CSprite::KillItem(item);}
   item=next;
 }

	//if starting a spider, add spiders at the spider positions
	//spiders are replenished
	if(startspider==TRUE || sspider==TRUE)
	{
		sspider=TRUE;		//start only called once so use this to keep it on
		startspider=FALSE;
		if(CSprite::gettypecount(LITTLESPIDER)<(totmammyspiders*(2+game_level)))
		{
			//create the spiders
			for(loop1=0;loop1<32;loop1++) 
			{
				foundspider1=loop1;
				for(loop2=0;loop2<24;loop2++)
				{
					if(screendata[loop2*32+loop1] == XMAMMY_BMP)// && (foundspider1!=loop1 && foundspider2!=loop2))
					{
						foundspider2=loop2;
						//addSpiders(loop1*BLOCKSIZE,loop2*BLOCKSIZE,((2+game_level)*totmammyspiders)-(CSprite::gettypecount(LITTLESPIDER)/totmammyspiders),LITTLESPIDER,rand()%6); //spider
						//addSpiders(loop1*BLOCKSIZE,loop2*BLOCKSIZE,2+game_level,LITTLESPIDER,rand()%6); //spider
						addSpiders(loop1*BLOCKSIZE,loop2*BLOCKSIZE,1,LITTLESPIDER,rand()%6); //spider
					}
				}
			}
		}
	}
	//demon spider
	//not replenished
    foundspider1=foundspider2=-1;
	if(startdemon==TRUE)
	{
		startdemon=FALSE;
		if(CSprite::gettypecount(DEMONSPIDER)<(totdemonspiders*(3+game_level)))
		//if(CSprite::gettypecount(DEMONSPIDER)==0)
		{
			//create the spiders
			for(loop1=0;loop1<32;loop1++) 
			{
				for(loop2=0;loop2<24;loop2++)
				{
					if( screendata[loop2*32+loop1] == DEMON1_BMP && (foundspider1!=loop1 && foundspider2!=loop2)) 
					{
						foundspider1=loop1;
						foundspider2=loop2;
						//addSpiders(loop1*BLOCKSIZE,loop2*BLOCKSIZE,(3+game_level)-(CSprite::gettypecount(DEMONSPIDER)/totdemonspiders),DEMONSPIDER,rand()%3); //spider
						addSpiders(loop1*BLOCKSIZE,loop2*BLOCKSIZE,(3+game_level),DEMONSPIDER,rand()%3); //spider
					}
				}
			}
		}
	}
	//fish
	//are replenished
	if(startfish==TRUE || sfish==TRUE)
	{
		startfish=FALSE;
		sfish=TRUE;
		if(CSprite::gettypecount(FISH)<(3+game_level))
		{
			//create the spiders
			for(loop1=0;loop1<32;loop1++) 
			{
				for(loop2=0;loop2<24;loop2++)
				{
					if( screendata[loop2*32+loop1] == WBFISH_BMP && (screendata[loop2*32+loop1]>>4==0)) //change to add randomly
					{
						if(CSprite::gettypecount(FISH)<(3+game_level) && ((rand()%fishblocks)==(fishblocks-3)))
						addSpiders(loop1*BLOCKSIZE,loop2*BLOCKSIZE,1,FISH,rand()%120); //spider
					}
				}
			}
		}
	}
}

void gameDraw()
{
 //do backdrop first
 clear_to_color(gbuffer,black);
 drawscreen(gbuffer);


 //draw score lines, men, etc.

 //draw fps is required
 //if only 70% then show it anyway (i.e. 21 or 42 for 30/60 fps)
 if((fps<(FRAME_FPS_TARGET)*.7) && autoshowfps==TRUE) textprintf(gbuffer,font,0,SCREEN_H-text_height(font),fgdark,"FPS (Max %d): ONLY %d!",FRAME_FPS_TARGET,fps);
 else {
   if(showfps==TRUE) textprintf(gbuffer,font,0,SCREEN_H-text_height(font),fgdark,"FPS (Max %d): %s %d",FRAME_FPS_TARGET,(fps<(FRAME_FPS_TARGET)*.7) ? "ONLY" : "",fps);
 }
 if(cheatmode==TRUE) textout(gbuffer,font,"You cheating bastard!",SCREEN_W-text_length(font,"You cheating bastard!"),SCREEN_H-text_height(font),fgdark);
}

void hazardDraw()
{
 //loop through the hazards and just place them
 //include all sprites, excluding players/computer and gaps
 CSprite* item=CSprite::first;
 BITMAP* ptr;
 while(item) {
   //get x,y co-ordinates and place sprite
   //must be of type hazard or a sub class of hazard
   //gaps are controlled seperately
	 
	 if(item->type==LITTLESPIDER || item->type==FISH || item->type==DEMONSPIDER || item->type==DEMON || item->type==BIGSPIDER) {  // or with all hazard sprites
     ptr=(BITMAP*)dat_main[((CHazard*)item)->animseq[((CHazard*)item)->shownsprite]].dat; //current sprite
     masked_blit(ptr,gbuffer,0,0,((CHazard*)item)->x+XOFF,((CHazard*)item)->y+YOFF,ptr->w,ptr->h);
   }
   item=item->next;
 }

}

void otherDraw()
{

	//draw title
	int xoffset=200;//512+BLOCKSIZE;
	int yoffset;
	int ygap=text_height(font);
	int xoffsetgap=text_length(font,"XXXXXXX");
	int laserlength=0;
	BITMAP* smalllogo=(BITMAP*)dat_main[XXLOGO_BMP].dat;

	yoffset=412-ygap;//SCREEN_H-smalllogo->h+BLOCKSIZE-ygap*5;
	masked_blit(smalllogo,gbuffer,0,0,SCREEN_W/2-smalllogo->w/2,0,smalllogo->w,smalllogo->h);

	//draw lives and score
	CSprite* players=CSprite::first;
	BITMAP* men=(BITMAP*)dat_main[MAN_WALK1_BMP].dat;

	while(players) 
	{
		if(players->type==PLAYER) 
		{
			laserlength=((CPlayer*)players)->firestrength;
			//show scores and lives and level
			textprintf(gbuffer,font,xoffset,yoffset+ygap,fg1,  "Level: %5d",game_level);
			textprintf(gbuffer,font,xoffset,yoffset+ygap*2,fg1,"Score: %5d",((CPlayer*)players)->score);
			textprintf(gbuffer,font,xoffset+xoffsetgap*4,yoffset+ygap*2,fg1,"Lives:");
			
			if( ((CPlayer*)players)->lives<1) 
			{
			  textout(gbuffer,font,"Dead",xoffset+xoffsetgap*4+xoffsetgap,yoffset+ygap*2,fg1);
			}
			else
			{
			  //draw lives, safes, bombs
			  for(int i=1;i<((CPlayer*)players)->lives;i++) draw_sprite(gbuffer,men,xoffset+xoffsetgap*4+text_length(font,"XXXXXXX")+(i-1)*men->w,yoffset+ygap);
			}
			break;
		}
	}

	//draw gun power
	if(laserlength==0) textprintf(gbuffer,font,xoffset,yoffset+ygap*3,fg1,"Laser: Empty");
	else
	{
		textprintf(gbuffer,font,xoffset,yoffset+ygap*3,fg1,"Laser:");
		rectfill(gbuffer, xoffset+text_length(font,"XXXXXXX"), yoffset+ygap*3, xoffset+text_length(font,"XXXXXXX")+laserlength, yoffset+ygap*4, fgdark);
	}
}

void playerDraw()
{
 //loop through the players and just place them
 CSprite* item=CSprite::first;
 BITMAP* ptr;
 int mx=1;
 int xx=0;
 int xtonextblock=0;
 static int firecolour=178;
 static int firedelta=1;
 static int firenum=0;
 while(item) {
   //get x,y co-ordinates and place sprite
   //must be of type hazard or a sub class of hazard
   if(item->type==PLAYER) {
	   //set position
		if(((CPlayer*)item)->facing==PM_LEFT) 
		{
			mx=-1;
			xx=((CPlayer*)item)->x+XOFF;
		}
		else
		{
			xx=((CPlayer*)item)->x+((CPlayer*)item)->width+XOFF;
		}
		//get next block
		xtonextblock=getnextblock(xx-XOFF,((CPlayer*)item)->y+((CPlayer*)item)->height/2-1,((CPlayer*)item)->facing);
		if(xtonextblock>((CPlayer*)item)->firestrength) ((CPlayer*)item)->nextblock=-1;
	   
	   ptr=(BITMAP*)dat_main[((CPlayer*)item)->animseq[((CPlayer*)item)->shownsprite]].dat; //current sprite
	//show player
     if(swimming==TRUE && showblue==TRUE) draw_gouraud_sprite(gbuffer, ptr, XOFF+((CPlayer*)item)->x,YOFF+((CPlayer*)item)->y, 82, 92, 83, 91);
	 else masked_blit(ptr,gbuffer,0,0,XOFF+((CPlayer*)item)->x,YOFF+((CPlayer*)item)->y,ptr->w,ptr->h);

	 //do fire if there
	if(((CPlayer*)item)->firing==TRUE)
	{
		if(firenum<0)
		{
			firenum=0;
			firedelta=1;
		}
		else if(firenum>10)
		{
			firenum=10;
			firedelta=-1;
		}
		firenum+=firedelta;
		if(xtonextblock==-1 || xtonextblock>((CPlayer*)item)->firestrength)
		{
			((CPlayer*)item)->nextblock=-1;			
			hline(gbuffer, xx, \
				YOFF+((CPlayer*)item)->y+(((CPlayer*)item)->height/2-1), \
				xx+((CPlayer*)item)->firestrength*mx, firecolour+firenum);
		}
		else
		{
			((CPlayer*)item)->nextblock=xtonextblock;
			hline(gbuffer, xx, \
				YOFF+((CPlayer*)item)->y+(((CPlayer*)item)->height/2-1), \
				xx+(xtonextblock*mx), firecolour+firenum);
		}


	}
   }
   item=item->next;
 }
}


void lastDraw()
{
 //draw everything that goes on top of everything else
 //sidelines - only drawn if not occupied by player
/*
	BITMAP* ptr=(BITMAP*)dat_main[SK_DUGEM_BMP].dat;
 for(int i=1;i<=7;i++) {
    if(levelsafeavailable[i-1]==TRUE) {
      masked_blit(ptr,gbuffer,0,0,1,(i*60)-ptr->h,ptr->w,ptr->h);
    }
 }
*/
}

int getnextblock(int x,int y,EPLAYER direction)
{
	//get number of pixels to next unpassable block from player
	//bricks and sea plants
	int currblock;
	int loop;
	int yblock=y/BLOCKSIZE;
	int xblock=x/BLOCKSIZE;
	if(direction==PM_RIGHT)
	{
		for(loop=0;loop<32;loop++)
		{
			if(xblock+loop>31) return -1;  //past end
			currblock=screendata[yblock*32+xblock+loop];

			if(currblock==WALL_BMP || currblock==SEA1_BMP) return BLOCKSIZE*loop;
		}
	}
	else
	{
		for(loop=0;loop<32;loop++)
		{
			if(xblock-loop<0) return -1;  //past end
			currblock=screendata[yblock*32+xblock-loop];
			if(currblock==WALL_BMP || currblock==SEA1_BMP) return BLOCKSIZE*loop;
		}
	}

	return -1;
}


void flickscreen(int bOn)
{
 int i;
 BITMAP* tmp=create_bitmap(SCREEN_W,SCREEN_H);
 BITMAP* tmp2=create_bitmap(SCREEN_W,SCREEN_H);
 clear_to_color(tmp,0);
 clear_to_color(tmp2,0);

 drawscreen(tmp);
 if(bOn==TRUE)
 {
	 for(i=90;i>0;i--)
	 {
		 clear_to_color(tmp2,0);
		 acquire_screen();
		 pivot_sprite(tmp2, tmp, 0, 0, 0, 0, itofix(i));
		 blit(tmp2,screen,0,0,0,0,tmp2->w,tmp2->h);
		 release_screen();
		 while(game_time%4) ;
	 }
 }
 else
 {
	 for(i=0;i<90;i++)
	 {
		 clear_to_color(tmp2,0);
		 acquire_screen();
		 pivot_sprite(tmp2, tmp, 0, 0, 0, 0, itofix(i));
		 blit(tmp2,screen,0,0,0,0,tmp2->w,tmp2->h);
		 release_screen();
		 while(game_time%4) ;
	 }
 }
 destroy_bitmap(tmp);
 destroy_bitmap(tmp2);

}

int newlevel(int flag)
{

 //start a new level
 //set level variables
 //set game variables
 //set new sprites
 //if flag TRUE then a level has been completed
 //if flag FALSE then game start
 //multi-player end of level is trapped prior to this call

 //start a new level
plantskill=FALSE;
demonawake=FALSE;
demonsleep=TRUE;
startspider=FALSE;
stopspider=FALSE;
startfish=FALSE;
stopfish=FALSE;
startg1=FALSE;	//no stops as gates 1 to 3 are latches
startg2=FALSE;
startg3=FALSE;
startdemon=FALSE;
stopdemon=FALSE;

 CSprite* player;
 player=CSprite::first;

 myGame.purgedigi(TRUE);	//kill all sounds
 if(flag==FALSE) {
   //new game so reset score and lives
   //as each new level deletes everything, this resets it
	carryscore=0;
	carrylives=3;
	game_level=1;
	sspider=sfish=startfish=startspider=startdemon=FALSE;
 } else {

   //new level so carry over the scores/lives
	while(player!=NULL)
	{
		if(player->type==PLAYER)
		{
			carryscore=((CPlayer*)player)->score;
			carrylives=((CPlayer*)player)->lives;
		}
		player=player->next;
	}
}

 //start a new level
 //delete class objects
 CSprite::KillAll();

 //create players
 addPlayers(0);
 addDemonMammy();

 //new hazard object is deleted by the KillAll static class method
 game_time=1; //reset timer

 return TRUE;
}

void addPlayers(int i)
{

 //create players
 CHazard* newhaz;
	newhaz=new CPlayer(playerkeys[currentkey][0],playerkeys[currentkey][1],playerkeys[currentkey][2],playerkeys[currentkey][3],playerkeys[currentkey][4]);    //player 1
	CHazard::Add(newhaz, playerstartx, playerstarty, FRAME_MAN_PIXELS_FRAME, \
				player[PM_STILL][0], \
              player[PM_STILL], \
              ((BITMAP*)dat_main[player[PM_LEFT][1]].dat)->w,\
              ((BITMAP*)dat_main[player[PM_LEFT][1]].dat)->h,\
              i,PLAYER,0);
 //set players scores/lives now that the players are created
 CSprite* player;
 player=CSprite::first;
 if(player==NULL) {
	    myGame.abortsystem("Could not create players!");
 }else {
	((CPlayer*)player)->lives=carrylives;
	((CPlayer*)player)->score=carryscore;
 }
}

void addDemonMammy()
{
	int loop1,loop2;
	int x,y;

	//validate that the user is there
	for(loop1=0;loop1<32;loop1++)
	{
		for(loop2=0;loop2<24;loop2++)
		{
			if( screendata[loop2*32+loop1] == DEMON1_BMP) 
			{
				x=loop1*BLOCKSIZE;
				y=loop2*BLOCKSIZE;
				addSpiders(x,y,1,DEMON,0);
			}
			if( screendata[loop2*32+loop1] == XMAMMY_BMP) 
			{
				x=loop1*BLOCKSIZE;
				y=loop2*BLOCKSIZE;
				addSpiders(x,y,1,BIGSPIDER,0);
			}

		}
	}

}

void addSpiders(int x,int y,int count,ESPRITE sprtype,int waittime)
{	

 //create new spiders at position x,y
 CHazard* newhaz;
 int loop;
	for(loop=0;loop<count;loop++)
	{
		switch(sprtype)
		{
		case FISH:
			newhaz=new CSpider();
			CHazard::Add(newhaz, x, y, FRAME_FISH_PIXELS_FRAME, \
					fish[0][0], \
				  fish[0], \
				  ((BITMAP*)dat_main[fish[0][1]].dat)->w,\
				  ((BITMAP*)dat_main[fish[0][1]].dat)->h,\
				  0,sprtype,waittime);
			break;
		case LITTLESPIDER:
		case DEMONSPIDER:
			newhaz=new CSpider();
			CHazard::Add(newhaz, x, y, FRAME_SPIDER_PIXELS_FRAME, \
					spiderlittle[0][0], \
				  spiderlittle[0], \
				  ((BITMAP*)dat_main[spiderlittle[0][1]].dat)->w,\
				  ((BITMAP*)dat_main[spiderlittle[0][1]].dat)->h,\
				  0,sprtype,waittime);
				 if(sprtype==LITTLESPIDER) playsample(SPIDERMAMMY_WAV,LOW,QUIET,FALSE);
				 if(sprtype==DEMONSPIDER) playsample(SPIDERDEMON_WAV,LOW,QUIET,FALSE);
			break;
		case DEMON:
			newhaz=new CEnemy();
			CHazard::Add(newhaz, x, y, FRAME_SPIDER_PIXELS_FRAME, \
					demon [0][0], \
				  demon[0], \
				  ((BITMAP*)dat_main[demon[0][1]].dat)->w,\
				  ((BITMAP*)dat_main[demon[0][1]].dat)->h,\
				  0,sprtype,waittime);
			break;
		case BIGSPIDER:
			newhaz=new CEnemy();
			CHazard::Add(newhaz, x, y, FRAME_SPIDER_PIXELS_FRAME, \
					spiderbig[0], \
				  spiderbig, \
				  ((BITMAP*)dat_main[spiderbig[1]].dat)->w,\
				  ((BITMAP*)dat_main[spiderbig[1]].dat)->h,\
				  0,sprtype,waittime);
			break;
		default:
			break;
		}
	}
}

int gamequit()
{
 int validkey=FALSE;
 int userkey=0;

 clear_keybuf();
 transmsgbox(screen,SCREEN_W,SCREEN_H,"","Are you sure you wish to quit [y/n]?",fg3,fgdark);
 while(validkey==FALSE) {
   userkey=readkey() & 0xff;
   if(userkey=='y' || userkey=='n') validkey=TRUE;
 }
 clear_keybuf();
 game_time=1; //reset interrrupt
 return (userkey=='y');
}

void gamepause()
{
 clear_keybuf();
 transmsgbox(screen,SCREEN_W,SCREEN_H,"Paused","press SPACE to continue",fg3,fgdark);
 waitF7();
 game_time=1; //reset interrupt timer
}

void gamehelp()
{
 clear_keybuf();
 transmsgbox(screen,SCREEN_W,SCREEN_H,"F2 FPS; F3 Auto FPS; F4 Save Screen as BMP; F5 Pause; F6 Toggle Gouraud","press SPACE to continue",fg3,fgdark);
 waitF7();
 transmsgbox(screen,SCREEN_W,SCREEN_H,"F8 Music On/Off;  F9 Sound On/Off;  F10 Boss Key  (quick exit with no checks)","press SPACE to continue",fg3,fgdark);
 waitF7();

 game_time=1; //reset interrupt timer
}

void endsequence()
{
}

void transmsgbox(BITMAP* bmp, int w,int h,char* line1,char* line2, int transcolour, int forecolour)
{
 //passing in width/height in case we are passing in screen
 //and we need screen not virtual size
 int tlen1, tlen2,tlen3;
 tlen1=strlen(line1)*text_length(font,"A");  //fixed point font always
 tlen2=strlen(line2)*text_length(font,"A");
 tlen3=(tlen1>tlen2) ? tlen1 : tlen2;

 BITMAP* tmp=create_bitmap(tlen3,text_height(font)*6);

 clear_to_color(tmp,transcolour);
 textout_centre(tmp,font,line1,tmp->w/2,text_height(font),forecolour);
 textout_centre(tmp,font,line2,tmp->w/2,text_height(font)*3,forecolour);
 draw_trans_sprite(bmp,tmp,w/2-(tmp->w/2),h/2-(tmp->h/2));
 destroy_bitmap(tmp);
}

void waitF7()
{
 clear_keybuf();
 while(readkey()>>8 !=KEY_SPACE);
}

void MusicPlay()
{
 //moonlight sonata in C by Mozart
 if(dat_music!=NULL) myGame.playmidi((MIDI*)dat_music[MOZART_MIDI].dat,TRUE,MEDIUM);
}

//play a sample
//only play if dat file available
//extra layer as the allegro class should not know about such things
int playsample(int samplenum,EDIGIPRIORITY prior,EVOLUME vol,int flag)
{
 if(dat_music!=NULL) return myGame.playdigi((SAMPLE*)dat_music[samplenum].dat,vol,prior,flag);
 else return -1;
}


