
/*=====================================

 file: DrawEnd.cpp

 Owner: Hui Tin Yuen, Fred

 Description:

 For DrawEngine::drawEndScene() and its helper methods including game-over and game-clear

=====================================*/

#include <string.h>		// scanf, sprintf
#include <math.h>
#include <assert.h>

#include "DLib/DGL.h"
#include "DLib/DStr.h"

#include "System.h"
#include "Defines.h"
#include "ObjMake.h"	// fWhiteVec, etc
#include "Bound.h"
#include "Draw.h"


//---------------------------------------
// local static vars


//---------------------------------------
// helpers methods

static void vDrawGameOverScene();
static void vDrawGameClearScene();
static void vDrawFloor();


//---------------------------------------------------------
// draw end scene which depends on how player performed
//---------------------------------------------------------
bool DrawEngine::drawEndScene(int state)
{
	if ( state == SUB_STATE0 )
		vDrawGameOverScene();
	else if ( state == SUB_STATE1 )
		vDrawGameClearScene();

	return false;
}

void DrawEngine::setUpdateSpeed(float spd)
{
	Speed = spd;
}

//---------------------------------------------------------
// draw game over scene
//---------------------------------------------------------
void vDrawGameOverScene()
{
   setMainMessage(GameOver_Mesg);
	
   char m2[100] = "";

//	if ( Game_Time > END_TIME + 100 )
	{
//		setMainMessage("touch the screen or push 'q' to quit");
		Dstrcpy( m2, "please touch the MENU button to proceed");
		Speed = 0;	// w95 ver
	}

	//
	vDrawScene();

	//
	Draw_Engine.beginDraw2D();

	glColor3fv(fBlueVec);
	//len = strlen(m2);
	Draw_Engine.drawMessage(-90, -90, -1, 5, m2);

	Draw_Engine.endDraw2D();
}


//---------------------------------------------------------
// draw a long rectangle strip
//TOSPEEDUP: don't push/pop
//---------------------------------------------------------
void vDrawEarth(float z)
{
	glPushMatrix();
	glRotatef(6.3f, 1,0,0);
	glTranslatef(0.0, -0.5*CHAN_Y2, z);
	glScalef(CHAN_X*50.0, 0.0, 2.0);
	vDrawObj(OBJ_SMALL_EARTH);
	glPopMatrix();
}

//---------------------------------------------------------
// draw many rectangle strips above a very big rectangle as a base.
//---------------------------------------------------------
void vDrawEarths()
{
	float i;
	static float offset = 0.0;

	// draw bar
	for ( i=offset; i<CHAN_Z/2; i += RECT_LEN * 1.5 )
	{
		vDrawEarth(i-CHAN_Z/2.0+2.0);
	}
	
	offset += Speed;

	if ( offset >= 1.5 * RECT_LEN )
		offset = 0.0;

	// draw base
	glPushMatrix();
	glRotatef(6.3f, 1,0,0);
	glTranslatef(0.0, -0.5*CHAN_Y2-1, CHAN_Z);
	glScalef(CHAN_X*150.0, 0.0, CHAN_Z*2.0);
	// lmdef(DEFMATERIAL, OBJ_DREAM, 5, e_mat2);
	vDrawObj(OBJ_RECT_EARTH);
	glPopMatrix();
}


//---------------------------------------------------------
// draw 2D image to buffer
//---------------------------------------------------------
void vDraw2DBackground()
{
	if ( T_w == 0 )
		return;

	//----------------
	// disable states
	//
	glDisable(GL_DEPTH_TEST);	// z-buffering set above
	glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);		// ON by default
	glDisable(GL_DITHER);

	glDisable (GL_ALPHA_TEST);
	glDisable (GL_CULL_FACE);
#ifndef SB_GLES
	glDisable (GL_POLYGON_STIPPLE);
#elif __ANDROID_API__ > 9
	//use nativeActivity!?
#elif __ANDROID_API__ > 8
	//TODOA1?
	//try jnigraphics file:///O:/app/android-ndk/docs/STABLE-APIS.html
#endif
	glDisable (GL_STENCIL_TEST);
	glDisable (GL_NORMALIZE);

	//--------------
	int disx = WIN_WIDTH - T_w, disy=0;
	int offset;

	offset = (int) (sign(Eval_Angle)*tan(deg2r(fabs(Eval_Angle))) * CHAN_Z*0.5 );
	disy = 173+offset;

	//--------------
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	dglOrtho(0.0, T_w-1, 0.0f, T_h-1, -1.0f, 1.0f );
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	#if 0//def SB_DEBUG
	GLboolean valid;
	glGetBooleanv(GL_CURRENT_RASTER_POSITION_VALID, &valid);
	if ( valid == GL_FALSE )
		reportError("drawText: 2nd invalid pos\n");
	#endif
	
	//--------------
#ifdef SB_GLES
	//TODOA1
	//use  OES draw texture extensio??>
#else
	glRasterPos2i(0, disy);

	float zoomFactor = float(WIN_WIDTH) / T_w;
	glPixelZoom (zoomFactor, zoomFactor);
	glDrawPixels(T_w, T_h, GL_RGB, GL_UNSIGNED_BYTE, Pixel_Arr);		// 1008 refer to porting guide about glDrawPixel
	glPixelZoom (1, 1);
	glClear(GL_DEPTH_BUFFER_BIT);		// make the image as background of the scene
   

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
#endif

	//--------------
	// restore states
	glEnable(GL_DEPTH_TEST);		// z-buffering set above
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);		// ON by default
	glEnable(GL_DITHER);
}

//---------------------------------------------------------
// Draw a floor (possibly textured).
// THIS IS A TEMPOARY FUNCTION!
//---------------------------------------------------------
void vDrawFloor()
{
	const int textCoor = 64;
	const int textSizeZ = 16;			//temp hard-coded
	const float animateStep = 0.5;		// for animation; 1 unit = 1 non-repeated texture copy
										// in our case (sysmetric/repeated texture), 05 is the fastest
	const int w = CHAN_X*64;
	const int l = CHAN_Z/2;

	static float textOffset = 0;

	GLint floorVertices[4][3] =
	{
		{ 1, 0, 1 },
		{ 1, 0, -1 },
		{ -1, 0, -1 },
		{ -1, 0, 1 },	  
	};

	float textureCoors[4][2] =
	{
		{ 1, 1 },
		{ 1, 0 },
		{ 0, 0 },
		{ 0, 1 },
	};
	GLubyte indices[] = {
		0, 1, 2,  1, 2, 0,
	};

	glPushMatrix();
	glTranslatef(0, -0.5*CHAN_Y2, -l/2);
	glScalef(w, 1.0f, 1.0f );

	if ( Draw_Engine.isTexture )
	{


#ifdef SB_GLES

#if 0
		//TODOA1
GLuint textureID = 1;

glEnable(GL_TEXTURE_2D);
glEnableClientState(GL_VERTEX_ARRAY);
glEnableClientState(GL_TEXTURE_COORD_ARRAY);
glBindTexture(GL_TEXTURE_2D, textureID);

glVertexPointer(3, GL_INT, 0, floorVertices);
glTexCoordPointer(2, GL_FLOAT, 0, textureCoors);
//glClientActiveTexture
glDrawElements(GL_TRIANGLES, SIZEOFARR(indices), GL_UNSIGNED_BYTE, indices);

glDisableClientState(GL_VERTEX_ARRAY);
glDisableClientState(GL_TEXTURE_COORD_ARRAY);
glDisable(GL_TEXTURE_2D);
#endif

#else
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);

		 glTexCoord2f(textOffset + textCoor, 0);
		 glVertex3iv(floorVertices[0]);

		 glTexCoord2f(textOffset, 0 );		 
		 glVertex3iv(floorVertices[1]);

		 glTexCoord2f(textOffset , textCoor*4);				// *2: because width of floor != height
		 glVertex3iv(floorVertices[2]);
		 
		 glTexCoord2f(textOffset + textCoor, textCoor*4);
		 glVertex3iv(floorVertices[3]);

		glEnd();
		glDisable(GL_TEXTURE_2D);
#endif		
	}
	else
	{
		;
	}

  glPopMatrix();

  //TODOA9 use texture matrix to move it!!!
  textOffset = (animateStep+textOffset);

  if ( textOffset > textCoor )
	  textOffset = 0;
}

//---------------------------------------------------------
// draw game clear scene
//-------------------------------------------------------0--
void vDrawGameClearScene()
{
	char title[] = "STAR BIRD";
	char m1[60], m2[60], m3[60];
	int len;


	// select message according to game states
	
	Dstrcpy(m3, " by Giby Studio");

	if ( Diffi - BABY < 0.1 || Passes <= Passes_Count*0.3  )
	{
			Dstrcpy(m1, "you're not a baby... Right?");
			Dstrcpy(m2, "please try next difficulty level");
	}
	else if ( Diffi - DIFF_NORMAL < 0.1 && Passes > Passes_Count*0.3 )
	{
			Dstrcpy(m1, "Congratulations!");
//			Dstrcpy(m2, "hint: use left mouse button to change view point...");
/*
			Dstrcpy(m2, "hint: during pressing pad enter, press 'r' and use mouse...");
			Dstrcpy(m2, "hint: to 'get' a virtual trackball\, press \'r\' during pressing pad enter");
*/
	}
	else if ( Diffi - EXPERT <0.1 && Passes > Passes_Count*0.5 )
	{
			Dstrcpy(m1, "Excellent!!!");
//			Dstrcpy(m2, "hint: press \'arrows\' and pad enter in the title scene...\n");
	}

	if ( Game_Time > END_TIME + 700 )
	{
			Dstrcpy(m1, "Thanks for playing the alpha version!");
	//		Dstrcpy(m2, "push 'q' to quit");
			Dstrcpy(m2, "please touch the MENU button to proceed");
    }
	else if ( Game_Time > END_TIME + 1400 )
			Dstrcpy(m1, "THE END");

	//---------------------------------
	// init env
	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glLoadIdentity();
	//glLoadMatrixf(mTranslations);
	//glMultMatrixf(mRotations);

	//---------------------------------
	// draw our scene
	
	vDraw2DBackground();
	update_walls(CHAN_X, -CHAN_X, CHAN_Y2*0.5, -CHAN_Y2*0.5);

	// use only one light  // this setup BYPASSES/IGNORES the user's setting: Light_On
#if 0
	//TODOA7 speed up this! since we don't need to do this every frame!
	// possibly make a function call initGameClearScene()
	setup_lig2();
	vDrawLight(GL_LIGHT0, GL_LIGHT0, 0, 50, -11.5);
	float spot_ang = 180;
	glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, &spot_ang);
#endif

	// dont ligt up DREAM by ONLY turning off all lights
	setup_lig();
	glEnable(GL_LIGHTING);
	vDrawDREAM();			/* view trans is set here */

	// draw moving earth
	if ( !Draw_Engine.isTexture )
	{
		glDisable(GL_LIGHTING);
		vDrawEarths();			// two layers without texture
	}
	else
	{
		vDrawFloor();			// one layer with texture
	}

	glPopMatrix();

	//---------------------------------
	// setup 2D and draw message

#if 0
	glPushMatrix();
	glLoadIdentity();
	glGetIntegerv(GL_MATRIX_MODE, &prev_mode);
	glMatrixMode(GL_PROJECTION);
	glGetFloatv(GL_PROJECTION_MATRIX, mat);

	glOrtho(-100, 100, -100, 100, 1, 5);
#endif

//#ifdef SB_WIN		// #x500 glError inside
#if 1
	Draw_Engine.beginDraw2D();

	glColor3fv(fBlueVec);

	Draw_Engine.drawMessage(50, 60, -1, 7, title);

	Draw_Engine.drawMessage(50, 40, -1, 7, m3);

	glColor3fv(fRedVec);
	len = strlen(m1);
	Draw_Engine.drawMessage(-len*2.6f, -20, -1, 6, m1);

	//len = strlen(m2);
	Draw_Engine.drawMessage(-90, -90, -1, 5, m2);

	Draw_Engine.endDraw2D();
#endif

#if 0
	// deinit
   glLoadMatrixf(mat);
   glMatrixMode(prev_mode);
   glPopMatrix();	
#endif
//	mySysSwapBuffers();
}
