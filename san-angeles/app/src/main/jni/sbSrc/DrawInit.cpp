
/*=====================================

 file: draw.cpp

 Owner: Hui Tin Yuen, Fred

 Description:

 Includes DrawEngine::init(), DrawEngine::drawTitleScene(int state), message printing
 and their helper methods

=====================================*/

#include <stdio.h>		// scanf, sprintf
#include <string.h>
#include <math.h>
#include <assert.h>

#include "DLib/DGL.h"
#include "DLib/DStr.h"

#include "System.h"
#include "Defines.h"
#include "ObjMake.h"
#include "Bound.h"
#include "Draw.h"


//---------------------------------------------------------
// Draw Title Scene in which there is a "game start" state and "a ready to go" state
//---------------------------------------------------------
void vDrawTitle(int state)
{
#ifdef SB_ANDROID
	char mesg[] = "TOUCH TO START";
	char mesg2[] = "use MENU button to change settings";
#else
	char mesg[] = "PUSH SPACE BAR TO START";
	char mesg2[] = "use right mouse button to change settings";
#endif
	char title[] = "STAR BIRD";

	static float x = 0.0, y = 0.0, z = -10.0;
	static float r_x = 0.0, r_y = 0.0, r_z = -10.0;
	static float xang = 0.0, yang = 90.0, zang = 0.0;
	static float wang = 0.0, bgang = 0.0;
	static float lx = 0.0, ly = CHAN_Y/2.0 + 2, lz = -14.0;	// light
	static int st_time, titxang = 0;

	int len = strlen(title);

	//----------------------------
	// clear buffer
	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	#ifdef DEBUG_USING_SPACEBALLs
	// allow trackball rotation for debugging
	glLoadMatrixf(mTranslations);
	glMultMatrixf(mRotations);
	#endif

#if 0
//#ifdef SB_DEBUG
	//----------------------------
	// a trick to move the light
//	if ( Last_Actions[FIRE] == TRUE )
	{
		if ( Last_Actions[UP] == TRUE )
			ly++;
		if ( Last_Actions[DOWN] == TRUE )
			ly--;
		if ( Last_Actions[LEFT] == TRUE )
			lz--;
		if ( Last_Actions[RIGHT] == TRUE )
			lz++;
	}
#endif

	//----------------------------
	// setup and draw scene
	gluLookAt(0.7f, 1.0f, -8.2f, r_x, r_y, r_z, 0.0f, 1.0f, 0.0f );

	setup_lig();
	vDrawLight(GL_LIGHT0, GL_LIGHT0, lx, ly, lz);

	//----------------------------
	// draw Dream and rotate it
	vDrawBody(-8.2f, r_x, r_y, r_z, x,y,z,xang,yang,zang,wang);
	yang+=2;

	//----------------------------
	// draw corridor(Channel)
	glPushMatrix();
	Speed = 0.0;
	glRotatef(bgang * 0.1f, 0,1,0);
	vDrawTunnel(FALSE);			// FALSE: only one light is needed and vDrawLight is called above
	vDrawSqs();
	vDrawRects();
	Speed = NOR_SPEED;
	glPopMatrix();

	//----------------------------
	// print message in 2D space
	if ( state == SUB_STATE0 )
	{
		//GLMatrix mat;

#if 0
		// init 2D draw
		glPushMatrix();
		glLoadIdentity();

		//2000-1017 glGetIntegerv(GL_MATRIX_MODE, &prev_mode);
		//2000-1017 assert(prev_mode == GL_MODELVIEW);

		glMatrixMode(GL_PROJECTION);
		glGetFloatv(GL_PROJECTION_MATRIX, mat);		//or pushmatrix()

		glLoadIdentity();		//2000-1013
		glOrtho(-100, 100, -100, 100, 1, 5);
#endif

		Draw_Engine.beginDraw2D();

		// draw
		glRotatef(titxang * 0.1f, 1,0,0);

		glColor3fv(fYellowVec);
		Draw_Engine.drawMessage(-95,35,-1, 27, title);

		glColor3fv(fBlackVec);
		Draw_Engine.drawMessage(-85,-85,-1, 6, mesg2);

		// deinit
		//glLoadMatrixf(mat);
		//glMatrixMode(GL_MODELVIEW);
		//glPopMatrix();

		Draw_Engine.endDraw2D();

		//TODOF4 speedup: 2D draw below again!
		setMainMessage(mesg);
		vDrawPrtMesg();			// which will set up 2D draw
		setMainMessage("");
	}
	else if ( state == SUB_STATE1 )
	{
		vDrawPrtMesg();	// which will set up 2D draw
	}

	glPopMatrix();

//	mySysSwapBuffers();	// which will call glFlush();
}


//---------------------------------------------------------
// setup 2D drawing and then draw mainMessage on the center of the screen
//---------------------------------------------------------
void vDrawPrtMesg()
{
	char str[30];
	int len = strlen(Main_Mesg);
	static int i = 0;

#ifdef SB_DEBUG
	int prev_mode;
	glGetIntegerv(GL_MATRIX_MODE, &prev_mode);
	assert(prev_mode == GL_MODELVIEW);
#endif

	if ( len >= 1 )
	{
		// print main message
//#ifdef SB_WIN
		Draw_Engine.beginDraw2D();
		glColor3fv(fCyanVec);
		Draw_Engine.drawMessage(-len*2.6f, -55, -1, 6, Main_Mesg);		// 2.6: half of estimated font width
		Draw_Engine.endDraw2D();
//#endif
	}
	else if ( !Is_GameOver )
	{
#ifdef SB_BUILD_VIRTUAL_PAD
		if (s_touchPadMode == 1 && !s_isTouchDown)
		{
			//----// draw 2D virtual on-screen controller
			Draw_Engine.beginDraw2D(1);
			//glEnable(GL_LINE_SMOOTH);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

			const int x = DPAD_X;
			const int y = DPAD_Y;

			//1. draw the DPAD frame
			glLineWidth(5.f);
			{
			float v[]     = {0.5,0.6,0.5,0.1};
			glColor4f(v[0], v[1], v[2], v[3] );
			}

			glPushMatrix();
			glTranslatef(x, y, -2);
			glScalef(DPAD_SIZE_OUT,DPAD_SIZE_OUT,0);

			float v[4][3] =	{
				{ -1, -1, 1 },
				{ 1, -1, 1 },
				{ 1, 1, 1 },
				{ -1, 1, 1 } };

			glVertexPointer( 3, GL_FLOAT, 0, v );
			glEnableClientState( GL_VERTEX_ARRAY );
			glDrawArrays( GL_LINE_LOOP, 0, SIZEOFARR(v) );
			glDisableClientState( GL_VERTEX_ARRAY );
			glPopMatrix();

			//2. draw the cross
			glLineWidth(1);
			glColor3fv(fGreenVec);
			Draw_Engine.drawMessage(x, y, -2, 6, "+");

			//
	//		glDisable(GL_LINE_SMOOTH);
			glDisable (GL_BLEND);
			Draw_Engine.endDraw2D();
		}
#endif

		//----// draw game status on screen
		Draw_Engine.beginDraw2D();

		if ( Energy > 20 )
   			glColor3fv(fWhiteVec);
		else if ( i != i/25*25 )
		{
			i++;
   			glColor3fv(fRedVec);
		}
		else
		{
			i++;
			glColor3fv(fYellowVec);
		}

		Dsprintf(str, "Passes: %d/%d   Energy: %d \n", Passes, Passes_Count, Energy);
		Draw_Engine.drawMessage(-60, -80, -1, 6, str);

		Dsprintf(str, "Speed: %.3d    SpEng: %d/%d\n", int(Speed*100), Sp_Eng/FULL_SPENG*10, 10);
		Draw_Engine.drawMessage(-60, -87, -1, 6, str);

#if 0
//#ifdef SB_DEBUG
		Dsprintf(str, "t: %.5d  p:%d  input:%d%d \n", Game_Time, isGamePaused(), Last_Actions[LTURN], Last_Actions[RTURN] );
		Draw_Engine.drawMessage(-60, -94, -1, 6, str);
#endif

		Draw_Engine.endDraw2D();
	}	

#if 0
	glPopMatrix();

   glLoadMatrixf(mat);
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
#endif
}




//==============================================================
// functions of class DrawEngine
//==============================================================

//---------------------------------------------------------
// Draw a text message on buffer
// Assume that 2D drawing, i.e.: beginDraw2D() was called
//---------------------------------------------------------
void DrawEngine::drawMessage(GLfloat x, GLfloat y, GLfloat z, float zoom, char *message)
{
	zoom *= 0.01f;

	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(zoom,zoom,zoom);
	::displayOnScreenMessage(message);
	glPopMatrix();
}

//---------------------------------------------------------
// draw title scene
// it's just a C++ method for the C function
//---------------------------------------------------------
bool DrawEngine::drawTitleScene(int state)
{
#ifndef DEBUG_TITLE
	
	vDrawTitle(state);
	if ( state >= SUB_STATE2 )
		return false;
	else
		return true;
#else

	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	dream::drawPart(OBJ_DREAM_ALL);

// begin2D()
//	drawMessage(2, 4, -4, "Spin me.");
// end2D()

	mySysSwapBuffers();
#endif
}

//---------------------------------------------------------
// For set up drawing in orthogonal space.
// only in windows version
//---------------------------------------------------------
void DrawEngine::beginDraw2D( const char mode )
{
	//
	glDisable(GL_LIGHTING);

	//
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();			//or: glGetFloatv(GL_PROJECTION_MATRIX, lastProjMat);
	glLoadIdentity();		//2000-1013
	if (mode==0)
		dglOrtho(-100, 100, -100, 100, 1.0f, 5.0f);		// for message
	else
		dglOrtho(0, Screen_W, 0, Screen_H, 1.0f, 5.0f);	// for input (virtual pad)

	//
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	assert(!GetOpenGLError());
}

//---------------------------------------------------------
// For undo the  set up above.
// only in windows version
//---------------------------------------------------------
void DrawEngine::endDraw2D()
{
	//
	glEnable(GL_LIGHTING);

	//
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();		//or: glLoadMatrixf(lastProjMat);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	assert(!GetOpenGLError());
}


//==============================================================
// initialization section
//==============================================================


//---------------------------------------------------------
// called by ui indirectly to switch on/off lighting
//---------------------------------------------------------
void set_light(int on)
{
	if (on == TRUE)
		Light_On = TRUE;
	else
		Light_On = FALSE;
}

//---------------------------------------------------------
// called by ui indirectly to change difficulty
//---------------------------------------------------------
void set_diffi(int deg)
{
	char s[] = "Can't change difficulty after the game is started";

	if ( Scene_State != SCENE_TITLE )
	{
		//TODOA8 this doesn't work in Windows ver:
		setMainMessage(s);
		//vDrawScene();
		//setMainMessage("");
		//return;
	}

	switch (deg)
	{
		case 1:
			Diffi = BABY;
			Eng_Deduct_Rate = INIT_ENG_RATE * Diffi;
			break;

		case 2:
			Diffi = DIFF_NORMAL;
			Eng_Deduct_Rate = INIT_ENG_RATE * Diffi;
			break;

		case 3:
			Diffi = EXPERT;
			Eng_Deduct_Rate = INIT_ENG_RATE * Diffi;
			break;
	}
}

//---------------------------------------------------------
// read in texture image
//---------------------------------------------------------
void read_textures()
{
	unsigned int i, x,r,g,b;

//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	FILE *fptr = fopen("cloud.ppm", "r");

	if (fptr == NULL)
	{
		reportError("Error: Texture file \"Cloud\" not found.\n");
		return;
	}

	Dfscanf(fptr, "%d", &T_w);
	Dfscanf(fptr, "%d", &T_h);

    for ( i=0;i<T_h;i++ )
    {
        for ( x=0;x<T_w;x++ )
        {
				r=g=b=0;
            Dfscanf(fptr, "%d %d %d", &r, &g, &b);

				Pixel_Arr[x+(T_h-1-i)*T_w][0] = r;
				Pixel_Arr[x+(T_h-1-i)*T_w][1] = g;
				Pixel_Arr[x+(T_h-1-i)*T_w][2] = b;
        }
    }

	 fclose(fptr);


    //for ( i=0;i<T_h;i++)
        //for ( x=0;x<T_w;x++)
				;//Pixel_Arr[x+i*T_w] = arr[x][i];
}

//---------------------------------------------------------
// initialize game state variables for each stage
// iris version: called make_all()
//---------------------------------------------------------
void DrawEngine::init()
{
#ifdef SB_GLES
	isTexture = false;
	//isTexture = true;
#else
	isTexture = true;
#endif

	// this is designed for future: if we could we start a new game section 
	// without running the exe...
	Scene_State = SCENE_TITLE;		// SCENE_TITLE, SCENE_END
	Sub_State = SUB_STATE0;			// SUB_STATE0, SUB_STATE1

	set_diffi(2);					// 2: 2nd entry to difficulty menu

	//memset(Last_Actions, 0, DR_NUM_BUT);
	clearLastActions();

	//
	Game_Time = 0;
	Energy = FULL_ENG;
	Passes = 0;					// # of rings (checkpoints) passed
	Passes_Count = NUM_CHK_PTS;				// total number of rings of the current stage
	Speed  = NOR_SPEED;
	Sp_Eng  = FULL_SPENG;
	Light_On = TRUE;
	Diffi = BABY;
	setHelpMessage("");

	for ( int i=UP; i <= SPDOWN; i++ )
	{
		Last_Actions[i] = false;
	}

	//
	Turn_Mode = FALSE;					// true if dream is turning corner
	Ob_Yang=0.0f;							// dream's rotatoin around y-axis
	//ZERO_MEMORY
	memset(Obz,0,sizeof(Obz));			// Dream's z coordinate
	memset(Obx,0,sizeof(Obx));			// Dream's x coordinate
	Act_Turn = -1;						// current turn/corner id

	//
	Is_GameOver = FALSE;		// if game is over
	Is_GameCleared = FALSE;		// if game is cleared
	ch_lplane = FALSE;			// on/off for bound checking: left side
	ch_rplane = FALSE;
	Eval_Angle=0.0f;

	// iris version:
	//strcpy(mainMesg, "Please Wait");
	//vDrawTitle(SUB_STATE0);
	//strcpy(mainMesg, "");

	//
	initInternalStates();
	bound_resetInternalStates();

	// read image files, which could be very slow.
	read_textures();
}
