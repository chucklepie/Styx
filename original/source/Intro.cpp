#include <allegro.h>
#include "jj_logo.h"
#include "styx.h"

extern CAllegro myGame;


//globals
int logo_shown;

void intro()
{
 //const int front=0;
 //const int back=4;
 DATAFILE* dat=NULL;
 int stopmeplease=FALSE;

 myGame.datload(&dat,DAT_LOGO,FALSE);

 if(dat==NULL) {
   logo_shown=FALSE;
 } else {
   //do logo
   logo_shown=TRUE;
   BITMAP* buffer=create_bitmap(SVGA_W,SVGA_H);
   BITMAP* ret=(BITMAP*)dat[LOGO_RET_BMP].dat;
   BITMAP* chu=(BITMAP*)dat[LOGO_CHU_BMP].dat;


     //clear_to_color(buffer,0);                   //black
     //do our stuff
     clear_keybuf();
     //set_gfx_mode(GFX_AUTODETECT,VGA_W,VGA_H,0,0);
     //draw line scanning in logo
     set_palette((RGB*)(dat[COMPANY_BMP].dat));
     int x1;
     for(x1=1;x1<SVGA_W;x1+=4) {
        clear_to_color(buffer,0);
        blit(ret,buffer,0,0,SCREEN_W/2-(ret->w/2),SCREEN_H/4-5-ret->h,ret->w,ret->h);
        blit(chu,buffer,0,0,SCREEN_W/2-(chu->w/2),SCREEN_H/4+5,chu->w,chu->h);
        rectfill(buffer,x1,0,SVGA_W-1,SVGA_H-1,0);
        line(buffer,x1-1,0,x1-1,SVGA_H-1,255);
        vsync();
        blit(buffer,screen,0,0,0,0,SVGA_W,SVGA_H);
        if(keypressed()) {stopmeplease=TRUE; break;}
     }
     line(screen,x1-5,0,x1-5,SVGA_H-1,0);  //get rid of last line
     if(stopmeplease==FALSE) rest(3000);

   destroy_bitmap(buffer);
   unload_datafile(dat);
 }
}


