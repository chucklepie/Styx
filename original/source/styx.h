/* styx
   Neil Walker 30/06/00
   www.retrospec.co.uk
*/

#ifndef STYX_HEAD
	#define STYX_HEAD
//compiler stuff, yes consts are better...
	#define VERSION "1.0.0"
	#define SYSTEM "Styx"
	#define AUTHOR "Neil Walker"
//the resolutions we represent as VGA and SVGA
	//#define VGA_H 240
	//#define VGA_W 320
	#define SVGA_H 480
	#define SVGA_W 640
	#define CONFIG_FILE "styx.cfg"
	#define HI_COLOUR 16
	#define MODE_GFX 1        //set_gfx_mode set to graphics
	#define MODE_DOS 3        //allegro is not running
	#define XOFF 64
	#define YOFF 10

//music and sound
	enum EVOLUME {QUIET=50, MEDIUMQUIET=110, MEDIUM=150, MEDIUMLOUD=200, LOUD=255};
	enum EDIGIPRIORITY {LOW=90, NORMAL=128, HIGH=255};
	#define MAXVOICEARRAY 256 //maximum sounds we allow to play at once
	//music levels
	#define MUSIC_ALL 0
	#define MUSIC_MUSIC 1
	#define MUSIC_SOUND 2
	#define MUSIC_NONE 3
	#define STR_MUSIC_ALL   "Music & Effects"
	#define STR_MUSIC_SOUND "Effects"
	#define STR_MUSIC_MUSIC "Music"
	#define STR_MUSIC_NONE  "None"



//data files
	#define MAINDAT_FILE "styx_mn.dat"
	#define MUSICDAT_FILE "styx_mus.dat"
	#define DAT_LOGO "jj_logo.dat"

//menu options
//-1 is defined as no option
	#define GAME1_PLAY 0
	#define GAME1_PLAYERS 1
	#define GAME1_INSTRUCTIONS 2
	#define GAME1_CREDITS 3
	#define GAME1_DIFFICULTY 4   //controls dificulty by basically adding pixels to gap movement
	#define GAME1_PASSWORD 5     //level selector for multi-player
	#define GAME1_HISCORE 6
	#define GAME1_SETUP 7
	#define GAME1_QUIT 8

	#define GAME2_RETURN 0
	#define GAME2_SPEC   1
	#define GAME2_MUSIC  2
	#define GAME2_SETUP  3

//game defaults
	#define PLATFORM_WIDTH 10   //thing you walk on in pixels
	#define PLATFORM_SPACE 50   //gap between two platforms in pixels
	#define MAXGAPS 20          //maximum number of gaps allowed
	#define LAST_LEVEL 9
	#define MAX_HAZARDS 20
	#define MAX_LIVES_NORMAL 7
	#define MAX_LIVES_DEATH 2
	#define BLOCKSIZE 16
	#define LEVELWIDTH (32*BLOCKSIZE)
	#define LEVELHEIGHT (24*BLOCKSIZE)

//sprites
	enum ESPRITE {BASESPRITE=0,HAZARD,PLAYER,BIGSPIDER,LITTLESPIDER,FISH,DEMON,DEMONSPIDER};
	//eplayer is linked to the sprite array
	//enum EPLAYER {PM_LEFT=0,PM_RIGHT,PM_STILL,PM_JUMP,PM_HIT,PM_UP,PM_DOWN,PM_DEAD,PM_FIXED,PM_FIRE};
	enum EPLAYER {PM_LEFT=0,PM_RIGHT,PM_STILL,PM_UP,PM_DOWN,PM_DEAD,PM_FIRE,PM_UPLEFT,PM_UPRIGHT,PM_DOWNLEFT,PM_DOWNRIGHT};
	enum EDEMON {D_SLEEP=0,D_AWAKE};
	enum EFISH {F_LIVE=0,F_DEAD};
	enum ESPIDER {S_LIVE=0,S_DEAD};

/*
Width is 640 pixels
Frames per second is 40
Required time to span width of screen 6 seconds
1 screen is 240 frames (6*40)
1 frame is 3 pixels (640/240) - actually 3
which gives 120 pixels per second (40*3)

*/
//if want smoother then double all the below where used
//except gap_pixels_frame which should be 2
	#define FRAME_FPS_TARGET 40       //required frame rate

	#define FRAME_SCREEN_FRAMES 240
	#define FRAME_MAN_PIXELS_FRAME 3      //pixels gap/baddies/player moves per frame -- add difficulty level to gap movement (see above) - bad name really
	#define FRAME_SPIDER_PIXELS_FRAME 2
	#define FRAME_FISH_PIXELS_FRAME 1
	#define FRAME_HAZ_WAIT_DEAD 80  //hazards wait when dead before removing

/////////////////////////////
//function declarations

//game functions
	//system
	void fps_handler();
	void trans_callback(int);
	//game
	void uuencode_generic();
	int strpos(char * string, unsigned char chr);
	void edit_game();
	void readscreen(char* file);
	void drawscreen(BITMAP* buf,int,int,int,int);
	void flickscreen(int);

	void game_loop();
	int showoptions(int);
	void play_game();
	void instructions(BITMAP*);
	int quit();
	void credits(BITMAP*);
	int position_logo(BITMAP*,BITMAP*);
	void subdisp(char [][75],int,BITMAP*,int,int);
	void music();
	void music_string(char*);
	void hiscore(BITMAP*);
	void computer_string(char*);
	void MusicPlay();
	int playsample(int,EDIGIPRIORITY,EVOLUME,int flag=FALSE);
	void playfireball(int);
	void welcomeback();
	int getnextblock(int,int,EPLAYER);
	void gamelevel();
	void game_string(char*);


	int newlevel(int);
	void hazardDraw();
	int gameInput();
	int playerUpdate();
	void hazardUpdate();
	void gameDraw();
	void playerDraw();
	void otherDraw();
	void lastDraw();
	void drawbackdrop();
	void drawplatforms();
	void addPlayers(int);
	void addSpiders(int,int,int,ESPRITE,int);
	void addFish(int,int,int);
	void addDemonMammy();
	void gamepause();
	int gamequit();
	void gamehelp();
	void transmsgbox(BITMAP*,int,int,char*,char*,int,int);
	void endsequence();
	void waitF7();
	void moveComputers();

//intro
	void intro();


//classes for game system
class CAllegro
{
 public:
//core methods
  CAllegro();
 virtual ~CAllegro();
 void shutdown();
 void abortsystem(char*);
 void datload(struct DATAFILE**,char*,int);
 void startup();
 void console(char*,char*,char*);

protected:
 //this is the order they are called
 virtual void init_system(); //main console startup to determine available modes, etc.
 void init_allegro(); //initialise Allegro
 virtual void init_game(); //initialise game

 virtual void deinit_game(); //stop all the stuff we have started
 void deinit_allegro();
 virtual void deinit_system();

//core data
private:
 int m_mode;

//music methods
public:
 //midi
 void noMusic();
 void yesMusic();
 int getMusic();
 void playmidi(MIDI*,int,EVOLUME);
 void stopmidi();

 //digi
 void noDigi();
 void yesDigi();
 int getDigi();
 int playdigi(SAMPLE*,EVOLUME,EDIGIPRIORITY,int);
 int playdigi(SAMPLE*,EVOLUME,EDIGIPRIORITY,int,int,int);
 void pansweep(int,int,int);
 void stopdigi(int,int force=FALSE);
 void purgedigi(int killit=FALSE);
 void killdigi();
 int digipos(int);
 void textmsg(char*);

//music data
protected:
 int digi_voices;
 int midi_voices;

 int nomidi; //in game midi
 volatile int nodigi; //in game digi

private:
 int voicearray[MAXVOICEARRAY]; //maximum allowed at any one time
};

#include "sprite.h"

#endif