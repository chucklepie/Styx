/* Jumping Jack 2 - Skunk McGeordie
   Neil Walker 12/Sept/1999
   www.retrospec.co.uk
*/

#include <iostream.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <allegro.h>
#include "styx.h"


//-----------------------------------------------------------/

////////////////////////////
//init_allegro
//start up
///////////////////////
void CAllegro::init_allegro()
{
 //graphics mode not to be set here, called by init_game after this

 //standard Allegro initialise
 allegro_init();
 install_timer();
 install_keyboard();
 install_joystick(JOY_TYPE_AUTODETECT);
 install_mouse();

 //set configuration file
 //reads from here if available
 set_config_file(CONFIG_FILE);

 //setup sound
 digi_voices=detect_digi_driver(DIGI_AUTODETECT);
 midi_voices=detect_midi_driver(MIDI_AUTODETECT);

 if(digi_voices<=0) nodigi=TRUE; else nodigi=FALSE;
 if(midi_voices<=0) nomidi=TRUE; else nomidi=FALSE;

 if(nodigi==TRUE) allegro_message("No digital sound found (sound effects)");
 if(nomidi==TRUE) allegro_message("No MIDI sound found (music).");

 reserve_voices(-1,-1);
 if(install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,NULL)==0) {
   set_volume(255,255); //maximum to whatever card is, volume controlled in game
 }

}


////////////////////////////
//deinit_allegro
//stop allegro
///////////////////////
void CAllegro::deinit_allegro()
{
 //allegro_exit removes all initialised stuff for us
 allegro_exit();
 m_mode=MODE_DOS;
}

///////////////////////
//shutdown
//
///////////////////////
void CAllegro::shutdown()
{
 deinit_game();
 deinit_allegro();
 deinit_system();
}

/////////
//abort system
//forced shutdown
///////////
void CAllegro::abortsystem(char* err)
{
 //usually called by itself if terminal or after shutdown
 char umsg[255];

 if((*err!=NULL)) sprintf(umsg,"Aborting system because:\n%s",err);
 else sprintf(umsg,"Aborting system (no reason given");
 set_gfx_mode(GFX_TEXT,0,0,0,0);
 allegro_message(umsg);
 allegro_exit();
 exit(0);
}


/////////////////
//load dat file
//load/reload dat file - may be needed when changing graphic modes
/////////////////
void CAllegro::datload(DATAFILE** dat, char* file, int stop) {
//dat must be set to NULL when it is initialised
   	//if dat is pointing at something then
    //the dat file must be open so close it
    //useful if change modes
	if(*dat!=NULL) {
      unload_datafile(*dat);
    }

    //try to open it
    *dat=load_datafile(file);

    //abort if in error
    if(stop && (*dat)==NULL) {
        char err[100];
        sprintf(err,"The data file %s could not be loaded.",file);
        if(*dat==NULL) abortsystem(err);
    }
}

CAllegro::CAllegro()
{
 //clear the voice array
 for(int i=0;i<MAXVOICEARRAY;i++) voicearray[i]=-1;
}

CAllegro::~CAllegro()
{
 //don't call shutdown
}

void CAllegro::startup()
{
 init_system();
 init_allegro();
 init_game();
}

void CAllegro::init_system()
{
}


////////////////////////////
//deinit_system
//stop system
///////////////////////
void CAllegro::deinit_system()
{
}

void CAllegro::textmsg(char* msg)
{
	//only message if not in graphics mode
	if(m_mode==MODE_DOS) allegro_message(msg);
}

////////////////
// music/sound stuff
/////////////

//volume stuff is quiet, mediumquiet, medium, mediumloud, loud

//midi stuff
void CAllegro::playmidi(MIDI* tune, int loopflag, EVOLUME vol)
{
 //play midi music if available
 if(nomidi==FALSE) {
   //set volume specified - leaving digi volume
   set_volume(-1,vol);
   play_midi(tune,loopflag);
 }
}

void CAllegro::stopmidi()
{
 //stop current midi
 play_midi(NULL,0);
}

int CAllegro::getMusic()
{
 if(nomidi==TRUE) return FALSE;
 else return TRUE;
}

//digital sounds - wav or voc stuff
//priority is low, medium, high

int CAllegro::getDigi()
{
 if(nodigi==TRUE) return FALSE;
 else return TRUE;
}

int CAllegro::playdigi(SAMPLE* samp, EVOLUME vol, EDIGIPRIORITY prior,int flagloop)
{
 //defaults available for pan and frequency
 return playdigi(samp,vol,prior,127,1000,flagloop);
}


int CAllegro::playdigi(SAMPLE* samp, EVOLUME vol, EDIGIPRIORITY prior,int pan, int freq, int loopflag)
{
 int ret=-1; //default which is could not play
 if(pan<0 || pan>255) pan=127;
 if(freq<0) freq=1000;

 if(nodigi==FALSE) {
   //set sample priority
   voice_set_priority(ret,prior);
   //allocate a voice if possible
   ret=allocate_voice(samp);
   if(ret!=-1) {
     //if available add to list and play - see purgedigi/stopdigi
     //may fail to add if system removes a lower priority voice
     //until the purgedigi is called
     //hopefully
	   if(loopflag==TRUE) voice_set_playmode(ret, PLAYMODE_LOOP);
     voice_start(ret);

     //go through the voice array and deallocate voice if being stored
     for(int i=0;i<MAXVOICEARRAY;i++) {
        if(voicearray[i]==-1) {
          voicearray[i]=ret;
          break;
        }
     }
   }
 }
 return ret;
}

void CAllegro::pansweep(int voice, int time, int endpan)
{
 if(nodigi==FALSE && voice!=-1) {
   voice_set_position(voice,0); //set at start
   voice_sweep_pan(voice,time,endpan);
 }
}

void CAllegro::stopdigi(int voice,int force)
{
 //stop a digi playing, for example if in a loop
 if(nodigi==FALSE && voice!=-1) {
   if(force==TRUE) deallocate_voice(voice);  
   else release_voice(voice); //allow it to finish first
   
   //go through the voice array and deallocate voice if being stored
   for(int i=0;i<MAXVOICEARRAY;i++) {
    if(voicearray[i]==voice) {
      voicearray[i]=-1;
      break;
    }
   }
 }
}

void CAllegro::purgedigi(int killit)
{
 //go through the voice array and deallocate voices
 //this is used to stop the user from having to do stopdigi
 // to speed up call it on a timer rather than, say, every frame
 int pos;
 if(nodigi==FALSE) {
   //loop through array
   for(int i=0;i<MAXVOICEARRAY;i++) {
      //if it contains a voice get its position
      if(voicearray[i]!=-1) pos=voice_get_position(voicearray[i]); else pos=-2;

      //if it is at the end then release it
      if(pos==-1 || killit==TRUE) {
        deallocate_voice(voicearray[i]);
        //release_voice(voicearray[i]);
        voicearray[i]=-1;
      }
   }
 }
}

void CAllegro::killdigi()
{
 //stop all samples in their tracks and clear the play buffer
 if(nodigi==FALSE) {
   //loop through array
   for(int i=0;i<MAXVOICEARRAY;i++) {
      //if it contains a voice get its position
      if(voicearray[i]!=-1) deallocate_voice(voicearray[i]);
      voicearray[i]=-1;
   }
 }
}

int CAllegro::digipos(int voice)
{
 if(nodigi==FALSE) return voice_get_position(voice);
 else return -1;
}


//game flags, not hardware settings
void CAllegro::noMusic()
{
 nomidi=TRUE;
}

void CAllegro::noDigi()
{
 nodigi=TRUE;
}

void CAllegro::yesDigi()
{
 nodigi=FALSE;
}

void CAllegro::yesMusic()
{
 nomidi=FALSE;
}

