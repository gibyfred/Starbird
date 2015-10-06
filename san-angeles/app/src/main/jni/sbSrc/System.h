
/*=====================================

 file: System.h

 Owner: Hui Tin Yuen, Fred

 Description:

  - contains main() and interface to GLUT
  - control refresh rate and keyboard/mouse callback


 Hui Tin Yuen
 92057645
 cs_fredh@ug.cs.ust.hk

 comp341 project

=====================================*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
typedef unsigned char bool;
#endif

//extern vOpenWindow(char *name, int w, int h, int i);
extern void mySysSwapBuffers();
extern void reportError(const char * _Format, ...);
extern void displayOnScreenMessage(const char* message);

// main call to StarBird system
extern void SBInitApp();
extern void SBInit();
extern void SBDeinit();
extern bool SBOnKeyEvent(int key, unsigned short state );
extern bool SBOnVirtualGameKeyEvent(int key, unsigned short state );

extern bool SBOnTouchEvent( unsigned short x, unsigned short y, unsigned short state );
extern void SBOnPause( bool isPause );
extern void SBOnTogglePause();
extern void SBDrawMain(long tick);//, int width, int height);
extern void SBOnResizeViewport( int width, int height );

#ifdef __cplusplus
}
#endif

