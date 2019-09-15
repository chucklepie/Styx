
//simple sprite routine
//to change to a different type

//subclass for hazard,man, poppins, gap
//need to get a book I think
//don't usually derive from csprite as co-ords are not stored here

//this is the main game sprite
//all others e.g. player, gap are derived from this
//to use the static functions
#ifndef STYX_SPR
#define STYX_SPR
class CSprite
{
 //public
 public:
        //methods
        CSprite();  //constructor
        virtual ~CSprite() {}
        static void KillAll(); //delete all sprites
        static void Add(CSprite*, int movei,int anif, int*animseq,int width, int height, int num, enum ESPRITE type,int startwait);
        static void NextFrame();           //new frame, update animations, etc.
        virtual void Move(int,int) {}               //normally called by nextframe only, only useful to bypass synchronisation
        static void KillItem(CSprite*); //delete a sprite
        //data
        static CSprite* first;  //first item, same for all
        CSprite* next;          //this hazards next in list
        CSprite* prev;          //this hazards previos in list
        //anim sequences are array[][]
        int animitem;   //entry in the first level
        int* animseq;    //array of animation sequences - -1 is the last - array in the last level

        int width;              //of sprite
        int height;             //of sprite
        int shownsprite;        //which sprite is showing/to show
        int num;                //unique reference for any purpose
        ESPRITE type;           //sprite type
        int moveinc;     //how many to move by every frame (left is negative)
		int framewait;	//how many frames to wait between each moveinc, allows finer speed control
        int waiting;     //time to wait every full screen move
        int animateframes; //how many frames between changing animation
        int currentframe;       //current frame
        int flagdelete;
        static int gettypecount(ESPRITE);
	private:
		int flagany;

};



class CHazard : public CSprite
{
 public:
        CHazard();
        ~CHazard() {}
        static void Add(CHazard*, int x,int y, int m, int anif, int*animseq,int width, \
			int height, int num, ESPRITE type,int startwait);
        virtual void Move(int,int);
        virtual void MoveLeft(int,int);            //these are normally called by nextframe via Move(), but used as an override
        virtual void MoveRight(int,int);
        virtual void MoveUp(int,int);
        virtual void MoveDown(int,int);

        int x,y;   
        EPLAYER movementoverride; //used to automove player (i.e. computer move)
        EPLAYER defaultdirection; //when not in danger direction player moves (i.e. computer)
        int defaultposition;      //where computer player goes if PM_FIXED type
        EPLAYER direction;
        int singlevoice;   //to allow a single sprite a single voice to play at any one time (useful if one action cancels another out, e.g. jumping then hitting)
		int dying;

	private:
};

//player sprite
class CPlayer: public CHazard
{
 public:
        //add function sets player/computer type
        CPlayer(int left,int right,int up, int down, int fire); //human player
        //CPlayer(char*);      
        ~CPlayer() {}
        void Move(int,int);               //normally called by nextframe only, only useful to bypass synchronisation
        void MoveLeft();            //these are normally called by nextframe via Move(), but used as an override
        void MoveRight();
		void MoveUp();
		void MoveDown();
		EPLAYER lastdirection;

        int Collision(ESPRITE*); //check collision of players with sprites
		void doMove();

        char* username() {return playername;}
        void dead();
        int isdead();

		void BlockCollision(int*,int*,int*,int*,int*,int*,int*,int*,int*,int*,int*,int*,int*,int*,int*,int*); //check player collision with blocks
		int GetBlock(int,int);
		int blockmove;

        int score;
        int lives;
		EPLAYER facing;
		int firestrength;		//strength of laser in pixels (defaults to 5 blocks)
		int firecountdown;		//how long (in total) firing before reducing strength, defaults to 5 seconds
		int firing;				//if currently firing
		int nextblock;			//pixels position to next solid block
		void processblock(void);
 protected:
        char playername[20]; //name
        int insafe;
 private:
        int kleft,kright,kup,kdown,kfire; //key presses for movement
        int deadasageordie;
};

class CSpider: public CHazard
{
 public:
        //add function sets player/computer type
        CSpider(); 
        //CPlayer(char*);      
        ~CSpider() {}
        void Move(int,int);               //normally called by nextframe only, only useful to bypass synchronisation
        void MoveLeft();            //these are normally called by nextframe via Move(), but used as an override
        void MoveRight();
		void MoveUp();
		void MoveDown();

		void BlockCollision(int*,int*,int*,int*,int*,int*,int*,int*,int*,int*,int*,int*,int*,int*,int*,int*); //check player collision with blocks
		int GetBlock(int,int);
		int blockmove;
		void processblock(void);

};

class CEnemy: public CHazard
{
 public:
        //add function sets player/computer type
        CEnemy(); 
        ~CEnemy() {}
        void Move(int,int);               //normally called by nextframe only, only useful to bypass synchronisation
        void MoveLeft();            //these are normally called by nextframe via Move(), but used as an override
        void MoveRight();
		void MoveUp();
		void MoveDown();
};

#endif