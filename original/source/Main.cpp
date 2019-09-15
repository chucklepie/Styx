/* Jumping Jack 2 - Skunk McGeordie
   Neil Walker 12/Sept/1999
   www.retrospec.co.uk
*/

#include <string.h>
#include <stdio.h>
#include <time.h>

#include <allegro.h>
#include "styx.h"
#include "styx_mn.h"

//externals
extern int logo_shown;
extern DATAFILE* dat_main;
extern BITMAP* newplayers[][3];
extern int switchin;

//global variables
CAllegro myGame;
BITMAP* gbuffer; //double buffer if required
int gbufw,gbufh,gbufw_c,gbufh_c; //size of gbuffer (full and centre) for quickness
COLOR_MAP trans_map;
int musicdat;           //as above for music/sound effects

//fps stuff
volatile int frame_count;
volatile int fps;
volatile int digipurge=FALSE;

///////
//standard fps timer
//fps_handler
////////////
void fps_handler()
{
 //called every 1000 milliseconds
 fps=frame_count; //set fps to display to user
 frame_count=0;   //reset the count for the next loop
}
END_OF_FUNCTION(fps_handler);


//music handler for purging stuff
void digipurgenow()
{
 //called every 5 seconds, which seems reasonable to me
 digipurge=TRUE;
}
END_OF_FUNCTION(digipurgenow);

////////////////////////
//main
//the starting point
////////////////////////
int main(int argc, char* argv[])
{
 //this calls all initialisation stuff
 //left in GFX_TEXT MODE
 //srandom(time(0)); //one and only seed
 srand(time(0)); //one and only seed - change for windows
 myGame.startup();

 //main game loop
 logo_shown=FALSE;
 if(argc<=1 || stricmp(argv[1],"-nologo")!=0) {
   logo_shown=TRUE;
   intro();
 }
 game_loop();

 //tidy up - not really needed as destructor does this
 myGame.shutdown();

 return 0;
 //main console startup to determine available modes, etc.
}
END_OF_MAIN()
//-----------------------------------------------------------/




//virtual overrides
void CAllegro::init_game()
{
 //any extra console stuff here
 //set up translucency table
 //load up data file (temporarily)

 myGame.datload(&dat_main,MAINDAT_FILE,TRUE);
 if(dat_main!=NULL) {
   create_trans_table(&trans_map,(RGB*)dat_main[MAIN_PAL].dat,200,200,200,trans_callback);
   color_map=&trans_map; //set it
   unload_datafile(dat_main);
 }

 //check if extra data file is there
 if(exists(MUSICDAT_FILE)) musicdat=TRUE; else musicdat=FALSE;

 //must set graphics mode here
 set_color_depth(8);
 if(set_gfx_mode(GFX_AUTODETECT,SVGA_W,SVGA_H,0,0)<0) abortsystem("Cannot set/reset 320x240 8 bit mode!");
 m_mode=MODE_GFX;      //needed for text stuf
 //create global double buffer
 gbuffer=create_bitmap(SVGA_W,SVGA_H);
 if (gbuffer==NULL) myGame.abortsystem("error allocating screen space");
 gbufw=gbuffer->w;
 gbufh=gbuffer->h;
 gbufw_c=gbuffer->w/2;
 gbufh_c=gbuffer->h/2;


 set_display_switch_callback(SWITCH_IN, welcomeback);

 //fps timer
 fps=frame_count=0;
 LOCK_VARIABLE(fps);
 LOCK_VARIABLE(frame_count);
 LOCK_VARIABLE(digipurge);
 LOCK_FUNCTION(fps_handler);
 LOCK_FUNCTION(digipurgenow);
 install_int(fps_handler,1000);
 install_int(digipurgenow,5000);
}

void CAllegro::deinit_game()
{
 if(dat_main!=NULL) { unload_datafile(dat_main);}
 if(gbuffer!=NULL) {destroy_bitmap(gbuffer);}
}

void trans_callback(int pos)
{
 //called 256 times when setting up translucency
 if(pos%8==0) {
  //cout<<".";
  //fflush(stdout);
 }
}

void welcomeback()
{
	//called when display switched back in
	switchin=TRUE;
}


int strpos(char * string, unsigned char chr)
{
	int len = strlen(string);
	for (int i=0; i < len; i++)
	{
		if (string[i] == chr)
		{
			return i;
		}
	}
	return -1;
}

void uuencode_generic()
{
}
