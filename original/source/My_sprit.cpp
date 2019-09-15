//sprite functions
//all shite now as I've rewritten everything to find the bug
//which turns out to be nothing wrong with the sprite routines
//but I can't be bothered to return them to normal as they work ok
#include <allegro.h>
#include "styx.h"
#include <stdio.h>
#include <string.h>
#include "styx_mus.h"
#include "styx_mn.h"

extern int player[][7];
extern int spiderlittle[][7];
extern int levelsafeavailable[8];
extern int fish[][7];
extern int demon[][7];
extern int spiderbig[7];

extern DATAFILE dat_music;
extern CAllegro myGame;
extern BITMAP* gbuffer;
extern int cheatmode;
extern int game_level;
extern int leftmostpos;
extern int playersleft;
extern int screendata[32*24+1];
extern int gtl,gtr,gbl,gbr;
extern int startspider;
extern int stopspider;
extern int startdemon;
extern int stopdemon;
extern int startfish;
extern int stopfish;
extern int startg1;
extern int startg2;
extern int startg3;
extern int demonawake;
extern int demonsleep;
extern int plantskill;
extern int swimming;

////
//hazard sprite
CHazard::CHazard()
{
 //not really needed as set by add
 type=HAZARD;
 dying=FALSE;
 //constructor for sprite sets up rest first
}

void CHazard::Add(CHazard* haz, int x,int y, int movei, int anif, int*animseq, int width,int height,int num, ESPRITE type,int startwait)
{

 haz->x=x;
 haz->y=y;
 CSprite::Add(haz,movei,anif,animseq,width,height,num,type,startwait);
}


void CHazard::Move(int notwaiting,int flag)
{
}

void CHazard::MoveLeft(int min, int max)
{
 //called only by nextframe or as an override to move hazard straight away
	x-=moveinc;
}

void CHazard::MoveRight(int min, int max)
{
 //called only by nextframe or as an override to move hazard straight away
	x+=moveinc;
}

void CHazard::MoveUp(int min, int max)
{
 //called only by nextframe or as an override to move hazard straight away
	y+=moveinc;
}

void CHazard::MoveDown(int min, int max)
{
 //called only by nextframe or as an override to move hazard straight away
	y-=moveinc;
}

////
//player override
//

//human
CPlayer::CPlayer(int left,int right, int up, int down, int fire)
{
 //constructor
 kleft=left;
 kright=right;
 kup=up;
 kdown=down;
 kfire=fire;
 direction=PM_STILL;
 score=0;
 lives=MAX_LIVES_NORMAL;
 deadasageordie=FALSE;
 movementoverride=PM_STILL;
 defaultdirection=PM_STILL;
 singlevoice=-1;
 //blocktemp=0;
 blockmove=-1;
 lastdirection=PM_STILL;
 facing=PM_RIGHT;
 firestrength=BLOCKSIZE*7;		//size of initial laser
 firecountdown=FRAME_FPS_TARGET*2;	// seconds between reducing laser strength
 firing=FALSE;
 nextblock=-1;
}


void CPlayer::Move(int notwaiting,int forcedirection)
{
 //player can't move or do anything if waiting
 //except drop down a hole - extra collisions are ignored
 //if keyforce they move as instructed if allowed
 static EPLAYER lastdirection=direction;
 ESPRITE sp;

 firing=FALSE;
 if(notwaiting==TRUE && deadasageordie==FALSE) 
 {
  direction=PM_STILL;    //default to standing still looking cool
  //process user movement only if not waiting
  //rotate keys so that appears to allow multiple key presses

  if(type==PLAYER) 
  {
     //if forced move (e.g. for demo or computer move)
     if(movementoverride!=PM_STILL) 
	 {
       direction=movementoverride;
	   doMove();
     }
	 else
	 {
      if(key[kleft])
	  {
		  direction=PM_LEFT;
		  doMove();
	  }
      if(key[kright])
	  {
		  direction=PM_RIGHT;
		  doMove();
	  }
      if(key[kup])
	  {
		  direction=PM_UP;
		  doMove();
	  }
      if(key[kdown])
	  {
		  direction=PM_DOWN;
		  doMove();
	  }
      if(key[kfire])
	  {
		  //only play when finished
		  if(singlevoice==-1 && firestrength>0 )
		  {
			myGame.stopdigi(singlevoice);
			if(dying==FALSE)			singlevoice=playsample(GUN_WAV,NORMAL,MEDIUM,TRUE);
		  }
		  direction=PM_FIRE;
		  doMove();
	  }
	  if(direction==PM_STILL) doMove();
	 }
  }
  if(dying==TRUE) {
	  dead();		//only get here when finished wait period
  }
 }

 if(direction!=PM_FIRE) 
 {
	 myGame.stopdigi(singlevoice,TRUE); //stop if not firing
	 singlevoice=-1;
 }

 //check for collision
 //only check if any kind of collision first
 if(Collision(&sp)==TRUE) 
 {
   //if hit poppins or a hazard then fall
   if(sp!=PLAYER)
   {
     if(cheatmode==FALSE) 
	 {
		 if(dying==FALSE)	//only kill player once
		 {
			 myGame.stopdigi(singlevoice,TRUE);
			 playsample(SCREAMMAN_WAV,NORMAL,MEDIUM);
			waiting=FRAME_HAZ_WAIT_DEAD;
			dying=TRUE;
		 }
     }
   }
 }

 //to reset dead if changed by above
 if(deadasageordie==TRUE || dying==TRUE) direction=PM_DEAD;

 if(firing==TRUE)
 {
	 currentframe=0; //reset straight away
     shownsprite=1;            //set starting sprite
 }
  //set animation back to start if new direction
 if(notwaiting==TRUE) 
 {
   if(lastdirection!=direction) 
   {
	   if(direction==PM_LEFT || direction==PM_UPLEFT || direction==PM_DOWNLEFT) animseq=player[PM_LEFT]; //set sequence pointer
	   else if(direction==PM_RIGHT || direction==PM_UPRIGHT || direction==PM_DOWNRIGHT) animseq=player[PM_RIGHT]; //set sequence pointer
	   else if(direction==PM_STILL) animseq=player[facing];
	   else if(direction==PM_UP || direction==PM_DOWN || direction==PM_FIRE) animseq=player[facing];
	   else animseq=player[direction];

     //adjust for FPS
     animateframes=player[direction][0];

     currentframe=0; //reset straight away
     shownsprite=1;            //set starting sprite
   }
   if(direction==PM_STILL) {shownsprite=0;currentframe=0;}	//always show first one for still
 }
  lastdirection=direction;

}


void CPlayer::doMove()
{
 //not firing unless firing
 firing=FALSE;

	 switch(direction) {
   //for each direction
   //set animation pointer if new type, otherwise just move
   //move if required (newframe controls movement)
   case PM_STILL:
	    processblock();
        break;
   case PM_LEFT:
        MoveLeft();
		facing=PM_LEFT;
        break;
   case PM_RIGHT:
        MoveRight();
		facing=PM_RIGHT;
        break;
   case PM_UP:
        MoveUp();
        break;
   case PM_DOWN:
	   MoveDown();
	   break;
   case PM_FIRE:
	   //every frame firing, count down
	   //when zero reduce strength
	   //if none left then not firing
	   firecountdown--;
	   if(firecountdown<=0)
	   {
		   firestrength-=BLOCKSIZE;		//size of initial laser
		   firecountdown=FRAME_FPS_TARGET*1;	//1 second between reducing laser strength
	   }
	   //if fire power then firing
	   if(firestrength<=0)
	   {
		   firestrength=0;
		   firing=FALSE;
	   }
	   else
	   {
		   firing=TRUE;
	   }

	   break;
   default:
        break;
 }
}

void CPlayer::MoveLeft()
{
 //called only by nextframe or as an override to move hazard straight away
 x-=moveinc;
 
 blockmove=-1;
 processblock();

 if(blockmove!=-1 || x<0) x+=moveinc;
 //when working, move player automatically around block
}

void CPlayer::MoveRight()
{
	x+=moveinc;
 blockmove=-1;
 processblock();
 if(blockmove!=-1 || x>LEVELWIDTH-width) x-=moveinc;

	 //when working, move player automatically around block
}

void CPlayer::MoveUp()
{
 y-=moveinc;
 blockmove=-1;
 processblock();
	 if(blockmove!=-1 || y<0) y+=moveinc;
	 //when working, move player automatically around block
}

void CPlayer::MoveDown()
{
 y+=moveinc;
 blockmove=-1;
 processblock();
 if(blockmove!=-1 || y>LEVELHEIGHT-height) y-=moveinc;
	 //when working, move player automatically around block
}


//check collision of player with all hazards
int CPlayer::Collision(ESPRITE* sp)
{
 //return value is TRUE for collision other than blank
 //sp is sprite type collided with

 CSprite* item=first;	//first sprite is always player
 int cx,cy;
 int midh=height/2;
 int hheight;
 int hit=FALSE;
 int offset;
 int xtonextblock=-1;

 //loop all hazards, i.e. moving baddies, not blocks
 while(item) {
  //dont bother checking yourself
  //collision with other players is ignored too
  if(item->type!=PLAYER) 
  {
   *sp=item->type;  //only valid and checked if a collision
   cx=((CHazard*)item)->x;
   cy=((CHazard*)item)->y;
   hheight=((CHazard*)item)->height;

   //match if within bounds of hazard, simple bounding box as sprites are clipped in bitmap and will do
   if( ((cx>=x && cx<x+width) || (cx<x && cx+item->width>x)) && ((cy>=y && cy<y+height) || (cy<y && cy+item->height>y)))
   {
     //collision
     return TRUE;
   }

   //check firing
   if(firing==TRUE && item->type!=BASESPRITE && ((CHazard*)item)->dying==FALSE)
   {
	   if(facing==PM_RIGHT)
	   {
		   xtonextblock=getnextblock(x+width,y+height/2-1,facing);
		   if(xtonextblock>firestrength) nextblock=-1;
		    if (nextblock==-1) offset=firestrength;
	        else offset=nextblock;
	

		   if(x<cx && x+width+offset>=cx && (cy<=y+midh && cy+hheight>=y+midh)) 
		   {
				 //play sound
				if(((CHazard*)item)->type==DEMON) ((CHazard*)item)->flagdelete=TRUE;
				else
				{
				  ((CHazard*)item)->dying=TRUE;
				  ((CHazard*)item)->waiting=FRAME_HAZ_WAIT_DEAD;
				}
				hit=TRUE;
		   }
	   }
	   else		//facing left
	   {
		   xtonextblock=getnextblock(x,y+height/2-1,facing);
		   if(xtonextblock>firestrength) nextblock=-1;
		  if (nextblock==-1) offset=firestrength;
		  else offset=nextblock;
	

		   if(x>cx && x-offset<=cx && (cy<=y+midh && cy+hheight>=y+midh)) 
		   {
				 //play sound
				if(((CHazard*)item)->type==DEMON) ((CHazard*)item)->flagdelete=TRUE;
				else
				{
				  ((CHazard*)item)->dying=TRUE;
				  ((CHazard*)item)->waiting=FRAME_HAZ_WAIT_DEAD;
				  hit=TRUE;
				}
		   }
	   }
	   if(hit==TRUE)
	   {
		   switch(item->type)
		   {
		   case BIGSPIDER:
				::playsample(SPLAT_WAV,NORMAL,MEDIUM);
			   score+=500;
			   break;
		   case DEMON:
			   myGame.purgedigi(TRUE);
				::playsample(SCREAMDEMON_WAV,NORMAL,MEDIUM);
			   score+=800;
			   break;
		   case LITTLESPIDER:
				::playsample(SPLAT_WAV,NORMAL,QUIET);
			   score+=30;
			   break;
		   case FISH:
 			   score+=30;
			   break;
		   case DEMONSPIDER:
				::playsample(SPLAT_WAV,NORMAL,QUIET);
			   score+=30;
			   break;
		   default:
			   break;
		   }
	   }
	   hit=FALSE;
   }

  }
  item=item->next; //get next one
 }

 return FALSE;
}

void CPlayer::processblock()
{
 int tl,tr,bl,br,l,r,t,b;
 int ptl,ptr,pbl,pbr,pl,pr,pt,pb;
 int block=0;
 int item=0;
 
 //set as a block if past end
 if(x<0 || x>LEVELWIDTH|| y<0|| y>LEVELHEIGHT) block=999;

 BlockCollision(&tl,&t,&tr,&bl,&b,&br,&l,&r,&ptl,&pt,&ptr,&pbl,&pb,&pbr,&pl,&pr);
 //get the highest priority block
 if(tl>0) {blockmove=direction;block=tl;}
 if(tr>0 && ptl==0) {blockmove=direction;block=tr;}
 if(bl>0 && ptl+ptr==0) {blockmove=direction;block=bl;}
 if(br>0 && ptl+ptr+pbl==0) {blockmove=direction;block=br;}
 if(t>0 && ptl+ptr+pbl+pbr==0) {blockmove=direction;block=t;}
 if(b>0 && ptl+ptr+pbl+pbr+t==0) {blockmove=direction;block=b;}
 if(l>0 && ptl+ptr+pbl+pbr+t+b==0) {blockmove=direction;block=l;}
 if(r>0 && ptl+ptr+pbl+pbr+t+b+l==0) {blockmove=direction;block=r;}

 //start/stop spiders, verify blocks, etc
 if((block&0x0f) == TRAP1_BMP)	//start trap off
 {
	 item=block>>4;
	 if(item==1 && startg1==FALSE) playsample(TRAP_WAV,LOW,MEDIUM,FALSE);
	 if(item==2 && startg2==FALSE) playsample(TRAP_WAV,LOW,MEDIUM,FALSE);
	 if(item==3 && startg3==FALSE) playsample(TRAP_WAV,LOW,MEDIUM,FALSE);

	 if(item==1) {startg1=TRUE;}
	 if(item==2) {startg2=TRUE;}
	 if(item==3) {startg3=TRUE;}
	 if(item==4 && CSprite::gettypecount(BIGSPIDER)>0) {startspider=TRUE;stopspider=FALSE;}
	 if(item==5) {startfish=TRUE;stopfish=FALSE;plantskill=TRUE;}
	 if(item==6) {startdemon=TRUE;stopdemon=FALSE;demonawake=TRUE;demonsleep=FALSE; }
	blockmove=-1;	//let user pass, everything else cannot
 }
 //stop items if finished, allow to pass if not closed, can always pass 4,5,6 as not really gates
 if((block&0x0f) == BLOCKED1_BMP)
 {
	 item=block>>4;
	 if(item==1 && startg1==FALSE) {blockmove=-1;}
	 if(item==2 && startg2==FALSE) {blockmove=-1;}
	 if(item==3 && startg3==FALSE) {blockmove=-1;}
	 if(item==4) 
	 {
		 stopspider=TRUE; 
		 startspider=FALSE;
		 blockmove=-1;
	 }
	 if(item==5) 
	 {
		 stopfish=TRUE;
		 startfish=FALSE;
		 //demonawake=FALSE;
		 //demonsleep=TRUE;
		 plantskill=FALSE;
		 blockmove=-1;
	 }
	 if(item==6) 
	 {
		 stopdemon=TRUE;
		 startdemon=FALSE;
		 demonawake=FALSE;
		 demonsleep=TRUE;
		 blockmove=-1;
	 }
 }

 //ignore if over any other blocks, e.g. starting block, background blocks, fish start
 if((block&0x0f) == MAN_WALK1_BMP || (block&0x0f) == WBFISH_BMP || (block&0x0f) == SPIDER1_BMP)
 {
	 blockmove=-1;	
 }

 //if(type==PLAYER && block==SEA1_BMP && plantskill==TRUE) ((CPlayer*)this)->dead();
 if(block==SEA1_BMP && plantskill==TRUE) 
 {
	 waiting=FRAME_HAZ_WAIT_DEAD;
	 dying=TRUE;
 }

  if((block&0x0f) == WBFISH_BMP) swimming=TRUE;
  else swimming=FALSE;
}

//check collision of player with map blocks
void CPlayer::BlockCollision(int* tl,int* t,int* tr,int* bl,int* b,int* br,int* l,int* r,int* ptl,int* pt,int* ptr,int* pbl,int* pb,int* pbr,int* pl,int* pr)
{

 *ptl=*pt=*ptr=*pbl=*pb=*pbr=*pl=*pr=0;

 *tl=GetBlock(x,y);
 *tr=GetBlock(x+width-1,y);
 *t=GetBlock(x+((width-1)/2),y);
 *bl=GetBlock(x,y+height-1);
 *br=GetBlock(x+((width-1)/2),y+height-1);
 *b=GetBlock(x+width-1,y+height-1);
 *l=GetBlock(x,y+((height-1)/2));
 *r=GetBlock(x+width-1,y+((height-1)/2));

 gtl=*tl;gtr=*tr;gbl=*bl;gbr=*br;

 //priority - 1 for wall or plants in order to set before others to allow items to not go over solid blocks
 if(*tl==SEA1_BMP || *tl==WALL_BMP) *ptl=1;
 if(*tr==SEA1_BMP || *tr==WALL_BMP) *ptr=1;
 if(*t==SEA1_BMP  || *t==WALL_BMP)  *pt=1;
 if(*bl==SEA1_BMP || *bl==WALL_BMP) *pbl=1;
 if(*br==SEA1_BMP || *br==WALL_BMP) *pbr=1;
 if(*b==SEA1_BMP  || *b==WALL_BMP)  *pb=1;
 if(*l==SEA1_BMP  || *l==WALL_BMP)  *pl=1;
 if(*r==SEA1_BMP  || *r==WALL_BMP)  *pr=1;
}

int CPlayer::GetBlock(int x, int y)
{
	int cx,cy;
	int cxr,cyr;
	int ret;
	//get the offset from the start of a block
	cxr=x%16;
	cyr=y%16;
	//cut it down
	x-=cxr;
	y-=cyr;
	//divide by the block size
	cx=x/16;
	cy=y/16;

	ret=cy*32+cx;
	if(ret<0 || ret>(32*24) ) ret=-2;
	else ret=screendata[ret];

	return ret;
}

void CPlayer::dead()
{
 deadasageordie=TRUE;
 stopspider=TRUE;
 stopdemon=TRUE;
 stopfish=TRUE;
 lives--;
}

int CPlayer::isdead()
{
 return deadasageordie;
}



//spider
CSpider::CSpider()
{
 //constructor
 singlevoice=-1;
 //blocktemp=0;
 blockmove=FALSE;
 movementoverride=PM_STILL;
 waiting=0;
}


void CSpider::Move(int notwaiting,int forcedirection)
{
 //player can't move or do anything if waiting
 //except drop down a hole - extra collisions are ignored
 //if keyforce they move as instructed if allowed
 EPLAYER olddirection=direction;
 //int colpos;

 if(notwaiting==TRUE) {
  //set direction movement
	direction=PM_LEFT;
     //if forced move (e.g. for demo or computer move)
     if(movementoverride!=PM_STILL) {
       direction=movementoverride;
     }

   if(dying==TRUE) 
   {
	   flagdelete=TRUE;	//get here with dying flag set only when not waiting any more
	   return;
   }

 }

 switch(type)
 {
 case FISH:
	 if(dying==TRUE) 
	 {
		 direction=PM_DOWN;
		 animseq=fish[F_DEAD];
		 animateframes=fish[F_DEAD][0];
	 }

	 break;
 case LITTLESPIDER:
 case DEMONSPIDER:
	 if(dying==TRUE)
	 {
		 animseq=spiderlittle[F_DEAD];
		 animateframes=spiderlittle[F_DEAD][0];
	     currentframe=0; //reset straight away
		 shownsprite=1;            //set starting sprite
		 direction=PM_STILL;
	 }

 default:
	 break;
 }


 switch(direction) {
   //for each direction
   //set animation pointer if new type, otherwise just move
   //move if required (newframe controls movement)
   case PM_STILL:
	   processblock();
        break;
   case PM_LEFT:
        MoveLeft();
        break;
   case PM_RIGHT:
        MoveRight();
        break;
   case PM_UP:
        MoveUp();
        break;
   case PM_DOWN:
	   MoveDown();
	   break;
   case PM_UPRIGHT:
	   MoveRight();
	   MoveUp();
	   break;
   case PM_UPLEFT:
	   MoveLeft();
	   MoveUp();
	   break;
   case PM_DOWNRIGHT:
	   MoveRight();
	   MoveDown();
	   break;
   case PM_DOWNLEFT:
	   MoveLeft();
	   MoveDown();
	   break;
   default:

        break;
 }

 //set animation back to start if new direction
 if(notwaiting==TRUE) {
   if(olddirection!=direction) {
	   switch(type)
	   {
	   case FISH:
		   
		   if(dying==TRUE)
		   {
			   animseq=fish[F_DEAD];
			   animateframes=fish[F_DEAD][0];
		   }
		   else
		   {
			   animseq=fish[0];
			   animateframes=fish[0][0];
		   }
		   
		   break;
	   case LITTLESPIDER:
		   
		   if(dying==TRUE)
		   {
			   animseq=spiderlittle[S_DEAD];
			   animateframes=spiderlittle[S_DEAD][0];
		   }
		   else
		   {
			   animseq=spiderlittle[0];
			   animateframes=spiderlittle[0][0];
		   }
		   break;
	   default:
		   animseq=spiderlittle[0];
		   animateframes=spiderlittle[0][0];
		   break;
	   }

	if((type!=FISH) || (type==FISH && dying==TRUE))
	{
     currentframe=0; //reset straight away
     shownsprite=1;            //set starting sprite
	}
   }
 }
}

void CSpider::MoveLeft()
{
 //called only by nextframe or as an override to move hazard straight away
 x-=moveinc;
 
 blockmove=-1;
 processblock();
 if(blockmove!=-1 || x<0) x+=moveinc;
 //when working, move spider automatically around block*/
}

void CSpider::MoveRight()
{
	x+=moveinc;
 blockmove=-1;
 processblock();
 if(blockmove!=-1 || x>LEVELWIDTH-height) x-=moveinc;

	 //when working, move spider automatically around block
}

void CSpider::MoveUp()
{
 y-=moveinc;
 blockmove=-1;
 processblock();
	 if(blockmove!=-1 || y<0) y+=moveinc;
	 //when working, move spider automatically around block
}

void CSpider::MoveDown()
{
 y+=moveinc;
 blockmove=-1;
 processblock();
 if(blockmove!=-1 || y>LEVELHEIGHT-height) y-=moveinc;
	 //when working, move spider automatically around block
}

void CSpider::processblock()
{
 int tl,tr,bl,br,l,r,t,b;
 int ptl,ptr,pbl,pbr,pl,pr,pt,pb;
 int block=0;
 int item=0;
 int spiderdir=0;
 
 //set as a block if past end
 if(x<0 || x>LEVELWIDTH|| y<0|| y>LEVELHEIGHT) block=999;

 BlockCollision(&tl,&t,&tr,&bl,&b,&br,&l,&r,&ptl,&pt,&ptr,&pbl,&pb,&pbr,&pl,&pr);
 //get the highest priority block
 if(tl>0) {blockmove=direction;block=tl;}
 if(tr>0 && ptl==0) {blockmove=direction;block=tr;}
 if(bl>0 && ptl+ptr==0) {blockmove=direction;block=bl;}
 if(br>0 && ptl+ptr+pbl==0) {blockmove=direction;block=br;}
 if(t>0 && ptl+ptr+pbl+pbr==0) {blockmove=direction;block=t;}
 if(b>0 && ptl+ptr+pbl+pbr+t==0) {blockmove=direction;block=b;}
 if(l>0 && ptl+ptr+pbl+pbr+t+b==0) {blockmove=direction;block=l;}
 if(r>0 && ptl+ptr+pbl+pbr+t+b+l==0) {blockmove=direction;block=r;}

 //stop items if finished, allow to pass if not closed, can always pass 4,5,6 as not really gates
 if((block&0x0f) == BLOCKED1_BMP)
 {
	 item=block>>4;
	 if(item==1 && startg1==FALSE) {blockmove=-1;}
	 if(item==2 && startg2==FALSE) {blockmove=-1;}
	 if(item==3 && startg3==FALSE) {blockmove=-1;}
	 if(item==4) blockmove=-1;
	 if(item==5) blockmove=-1;
	 if(item==6) blockmove=-1;
 }

 //ignore if over any other blocks, e.g. starting block, background blocks, fish start
 if((block&0x0f) == MAN_WALK1_BMP || (block&0x0f) == WBFISH_BMP || (block&0x0f) == SPIDER1_BMP)
 {
	 blockmove=-1;	
 }

 //ignore spiders hitting spiders and set movement
 if(type==LITTLESPIDER || type==DEMONSPIDER || type==FISH)
 {
	 if(block==XMAMMY_BMP|| block==DEMON1_BMP) 
	 {
		 blockmove=-1;
	 }
	 else 
	 { 
		 switch(type)
		 {
		 case LITTLESPIDER:
			 if(blockmove!=-1 || (rand()%100)==56)	//move different direction if blocked or small random chance
			 //if((rand()%100)==56)	//move different direction if blocked or small random chance
			 {
				 spiderdir=rand()%100;
				 if(spiderdir<25) movementoverride=PM_LEFT;
				 else if(spiderdir<50) movementoverride=PM_RIGHT;
				 else if(spiderdir<75) movementoverride=PM_UP;
				 else movementoverride=PM_DOWN;
			 }
			 break;
		 case DEMONSPIDER:
			 //if(block!=0 || (rand()%100)<10)	//move different direction if blocked or small random chance
			 if((rand()%100)<10)	//move different direction if blocked or small random chance
			 {
				 spiderdir=rand()%100;
				 if(spiderdir<15) movementoverride=PM_LEFT;
				 else if(spiderdir<30) movementoverride=PM_UPLEFT;
				 else if(spiderdir<45) movementoverride=PM_DOWNLEFT;
				 else if(spiderdir<55) movementoverride=PM_UP;
				 else if(spiderdir<65) movementoverride=PM_RIGHT;
				 else if(spiderdir<75) movementoverride=PM_UPRIGHT;
				 else if(spiderdir<85) movementoverride=PM_DOWNRIGHT;
				 else movementoverride=PM_DOWN;
			 }
			 break;
		 case FISH:
			 
			 //fish always move left to right
			 //if(block!=0 || (rand()%100<10))	//move different direction if blocked, randomly via up or down
			 if((rand()%100<10))	//move different direction if blocked, randomly via up or down
			 {
				 spiderdir=rand()%100;
				 if(spiderdir<20) movementoverride=PM_DOWNRIGHT;
				 else if(spiderdir<40) movementoverride=PM_UPRIGHT;
				 else movementoverride=PM_RIGHT;
			 }
			 //fish are dead if hit a block - they fall to the floor
			 if(x>=481 || block==SEA1_BMP) flagdelete=TRUE;
			 //if(x>=(32*BLOCKSIZE-width-1) || block==SEA1_BMP) flagdelete=TRUE;
			 
			 break;
		 default:
			 break;
		 }
	 }
 }

}

//check collision of player with map blocks
void CSpider::BlockCollision(int* tl,int* t,int* tr,int* bl,int* b,int* br,int* l,int* r,int* ptl,int* pt,int* ptr,int* pbl,int* pb,int* pbr,int* pl,int* pr)
{

 *ptl=*pt=*ptr=*pbl=*pb=*pbr=*pl=*pr=0;

 *tl=GetBlock(x,y);
 *tr=GetBlock(x+width-1,y);
 *t=GetBlock(x+((width-1)/2),y);
 *bl=GetBlock(x,y+height-1);
 *br=GetBlock(x+((width-1)/2),y+height-1);
 *b=GetBlock(x+width-1,y+height-1);
 *l=GetBlock(x,y+((height-1)/2));
 *r=GetBlock(x+width-1,y+((height-1)/2));

 gtl=*tl;gtr=*tr;gbl=*bl;gbr=*br;

 //priority - 1 for wall or plants in order to set before others to allow items to not go over solid blocks
 if(*tl==SEA1_BMP || *tl==WALL_BMP) *ptl=1;
 if(*tr==SEA1_BMP || *tr==WALL_BMP) *ptr=1;
 if(*t==SEA1_BMP  || *t==WALL_BMP)  *pt=1;
 if(*bl==SEA1_BMP || *bl==WALL_BMP) *pbl=1;
 if(*br==SEA1_BMP || *br==WALL_BMP) *pbr=1;
 if(*b==SEA1_BMP  || *b==WALL_BMP)  *pb=1;
 if(*l==SEA1_BMP  || *l==WALL_BMP)  *pl=1;
 if(*r==SEA1_BMP  || *r==WALL_BMP)  *pr=1;
}

int CSpider::GetBlock(int x, int y)
{
	int cx,cy;
	int cxr,cyr;
	int ret;
	//get the offset from the start of a block
	cxr=x%16;
	cyr=y%16;
	//cut it down
	x-=cxr;
	y-=cyr;
	//divide by the block size
	cx=x/16;
	cy=y/16;

	ret=cy*32+cx;
	if(ret<0 || ret>(32*24) ) ret=-2;
	else ret=screendata[ret];

	return ret;
}


//
//spider
CEnemy::CEnemy()
{
 //constructor
 singlevoice=-1;
 //blocktemp=0;
 movementoverride=PM_STILL;
 waiting=0;
}


void CEnemy::Move(int notwaiting,int forcedirection)
{
 //player can't move or do anything if waiting
 //except drop down a hole - extra collisions are ignored
 //if keyforce they move as instructed if allowed
 EPLAYER olddirection=direction;
 //int colpos;

 if(notwaiting==TRUE) 
 {
  direction=PM_STILL;    //default to standing still looking cool
  //process user movement only if not waiting
  //only allow movement if not in safety zone - right moves out
  //priority is highest for last check, in theory

  //set direction movement
  //temp below
     //if forced move (e.g. for demo or computer move)
     if(movementoverride!=PM_STILL) direction=movementoverride;

   if(dying==TRUE) flagdelete=TRUE;	//get here with dying flag set only when not waiting any more
 }

	 if(dying==TRUE) direction=PM_STILL;

 //set animation back to start if new direction

 if(notwaiting==TRUE) 
 {
   if(olddirection!=direction || type==DEMON) 
   {
	   switch(type)
	   {
	   case BIGSPIDER:
		   animseq=spiderbig;
		   animateframes=spiderbig[0];
	     currentframe=0; //reset straight away
		 shownsprite=1;            //set starting sprite
		   break;
	   case DEMON:
		   if(demonawake==TRUE) { animseq=demon[D_AWAKE]; }//demonawake=FALSE;	
			if(demonsleep==TRUE) {animseq=demon[D_SLEEP]; }//demonsleep=FALSE;
		   animateframes=demon[0][0];
		   break;
	   default:
		   break;
	   }
	}
 }
}

void CEnemy::MoveLeft()
{
 //called only by nextframe or as an override to move hazard straight away
}
void CEnemy::MoveRight()
{
 //called only by nextframe or as an override to move hazard straight away
}
void CEnemy::MoveUp()
{
 //called only by nextframe or as an override to move hazard straight away
}
void CEnemy::MoveDown()
{
 //called only by nextframe or as an override to move hazard straight away
}



