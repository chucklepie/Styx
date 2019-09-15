//sprite functions
#include <allegro.h>
#include "styx.h"
#include <stdio.h>

CSprite* CSprite::first=NULL;

//default sprite
//derive from here but don't use it
CSprite::CSprite()
{
 //all new sprites have no next one (i.e. they are the last)
 next=NULL;
 width=height=moveinc=currentframe=waiting=0;
 shownsprite=num=animitem=-1;
 type=BASESPRITE;
 animseq=NULL;
 flagdelete=FALSE;
 flagany=FALSE;
}

int CSprite::gettypecount(ESPRITE type)
{
 int count=0;
 CSprite* item=first;
 while(item) {
   if(item->type==type) count++;
   item=item->next;
 }
 return count;
}

void CSprite::KillItem(CSprite* i)
{
 //remove current item from the list
 //set first if none
 if(i->next==NULL && i->prev==NULL) CSprite::first=NULL;
 else {

	 //if no next then set previous next to null
	if(i->next==NULL) (i->prev)->next=NULL;
	//if no previous then set next previous to null and to first
	if(i->prev==NULL) {
		(i->next)->prev=NULL;
		CSprite::first=i->next;
	}
	else {
		//has previous and next so join them
		if(i->prev!=NULL) (i->prev)->next=i->next;
		if(i->next!=NULL) (i->next)->prev=i->prev;
	}
 }

 delete i;     //delete me*/
}

void CSprite::Add(CSprite* haz, int movei, int anif, int*animseq, int width,int height,int num, ESPRITE type,int startwait)
{

 if(movei<1) movei=1;	//stop increment from being 0 as can happen when division by animationlevel.  If require sprite to stop, either use 'waiting' or don't have movement overrides
 haz->moveinc=movei;    //how many pixels to move sprite by when required
 haz->animateframes=anif;     //how many frames before movement/animation
 haz->currentframe=anif;      //starting point
 haz->width=width;            //width of sprite (assumes all are the same)
 haz->height=height;          //height of sprite (assumes all are the same)
 haz->shownsprite=1;         //default first sprite - 0 is reserved for creator (e.g. animation rate)
 haz->animseq=animseq;
 haz->type=type;            //type of sprite for any purpose
 //find last link and add new hazard
 
 CSprite* previous=first;
 CSprite* item=first;
 haz->num=num;
 haz->waiting=startwait;

 //set prev to the last hazard
 //first=haz;
 if(first==NULL) {first=haz; haz->prev=NULL;}  //this is the first one
 else {
   //loop through all sprites
   while(item) {
     item=previous->next;       //get next one
     if(item!=NULL) previous=item;    //if still more point to next
   }
   previous->next=haz;  //last ones next pointer is new one
   haz->prev=previous;  //current ones prev is previous
 }
 haz->next=NULL;    //new ones next is null, i.e. the end

}


void CSprite::KillAll()
{
 //static
 CSprite* item=first;
 CSprite* next=first;
 //loop all stored hazards
 while(item) {
   next=item->next; //point to next item
   delete item;     //delete current
   item=next;       //set current as next
 }
 first=NULL;        //set first to null, i.e. no items
}



void CSprite::NextFrame()
{
 //static function
 //this is called every new frame
 //decrements the items frame count
 //when zero is reached then a new animation is done
 //every frame the movement is made
 CSprite* item=first;
 CSprite* next=first;
 //loop all hazards and update
 while(item) {
   //for each frame decrement the frame
   //if zero then time to move/animate
   //only if wait is zero - non-zero when freezing is required, e.g. hazard at bottomright, player hit, etc
   //when freezing, animation still takes place
   if(item->waiting==0) item->Move(TRUE,NULL); //move every frame, except when waiting
   else 
   {
     item->Move(FALSE,NULL); //otherwise wait but allow movement if move() allows this
     item->waiting--;
     if(item->waiting<0) item->waiting=0;
   }

   //change animation if animated
   //item 0 is always reserved for creators use
   //e.g. setting animation speed
   //last item is always negative and represents what animation sequence
   // to reset to, e.g. -1 starts from beginning which is usual
   // to stop after the last one, e.g. dead and wish to show last just set the last one
   //set aninateframes to 0 to stop at the current frame
  if(item->animateframes>0) {
     item->currentframe--;
     if(item->currentframe<=0) { //change animation if needed
      //set frame count back to start and change position/sprite
      item->currentframe=item->animateframes;
      item->shownsprite++;

      if(item->animseq[item->shownsprite]<0) item->shownsprite=abs(item->animseq[item->shownsprite]);
     }
   }
   //go to next sprite
   next=item->next;
   item=next;
 }
}

