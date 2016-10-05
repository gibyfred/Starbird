/*=====================================

 file: Systm.cpp - ui system

 Owner: Hui Tin Yuen, Fred

 Description:

  - contains main() and interface to GLUT
  - control refresh rate and keyboard/mouse callback

=====================================*/

/*
development note:

	In glut's spec:
	Definitely, definitely, use the -gldebug option to look for OpenGL errors 
	when OpenGL rendering does not appear to be operating properly. OpenGL 
	errors are only reported if you explicitly look for them! 


//TODOF1  add this compiler setting in release build to avoid 
the Console window appearing: /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup
*/

#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "DLib/DGL.h"
#include "DLib/DError.h"

#ifdef SB_WIN
#include <windows.h>
#include <gl/glut.h>			// don't include "System.h" if we include this!
#endif

#include "Defines.h"
#include "ObjMake.h"
#include "Draw.h"
#include "System.h"


//#define DEBUG_MSG



//---------------------------------------------------------------//
// Section: global variables for this file
//---------------------------------------------------------------//

typedef int Device;


#ifdef SB_IRIS
// setting #1
Device Device_Ids[DR_NUM_BUT] =
{
	GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_LEFT, GLUT_KEY_RIGHT,
	GLUT_KEY_HOME, GLUT_KEY_PAGE_UP, GLUT_KEY_END, GLUT_KEY_PAGE_DOWN, GLUT_KEY_INSERT, 
	GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12
};

values for glutKeyCB
  	50, 51, 48, 46, 13,
	53, 54, 56, 57
*/
#elif defined(SB_WIN)
// setting #2; same as setting 1 of iris version	// win32 dependency
Device Device_Ids[DR_NUM_BUT] =
{
	VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT,
	VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD0, VK_DECIMAL, VK_NUMPAD1,
	VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD8, VK_NUMPAD9
};


enum
{
	TIMER0,
};

#endif



#ifdef SB_BUILD_VIRTUAL_PAD
//-------------------------------------------------
// variables for virtual touch pad

int s_touchPadMode=2;
bool s_isTouchDown=0;

#endif


#ifdef SB_BUILD_TRACKBALL
//-------------------------------------------------
// variables for trackball

GLfloat angle = 0;   /* in degrees */
int moving, begin;
//GLdouble bodyWidth = 2.0;
char isInitTrackball = 0;		// the reverse of "isTrackballDirty"
#endif



//-------------------------------------------------
// variables for multiple windows
long win_ids[2];		//TODOA remove it?


// variables to control animation
static int frame_rate = 40;			// 40msec = 25 frames/second
static bool game_paused = false;
static int last_redraw_time = 0;


//---------------------------------------------------------------//
// Section: functions for other modules
//---------------------------------------------------------------//

/* -------------------------
swap back and front buffer
------------------------- */
void mySysSwapBuffers()
{
#ifdef SB_WIN
	glutSwapBuffers();		//note: glFlush is called inside
#else
	//TODO A1AA
	;
#endif
}

void reportError(const char * _Format, ...)
{
	;
}

// draw text message on screen
void displayOnScreenMessage(const char* message)
{
#if 1 //def SB_WIN
	glLineWidth(1);
	while (*message)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *message);
		message++;
	}
#endif
}

bool isGamePaused()
{
	return game_paused;
}

float getMainMessageF()
{
	return Game_Time;
//	return Energy;
}

char* getMainMessage()
{
	return Help_Mesg;
}

void setPopup( int puID, bool isShow )
{
	Engine_SetPopup( puID, isShow );
}

//---------------------------------------------------------------//
// Section: glut callback functions and helper functions
//---------------------------------------------------------------//
static void vGlutTimer(int timerId);


/* -------------------------
This is a special design to speed up redraw!
We should do something like this for many other stuffs during game play  //TODOA8 SPEEDUP
------------------------- */
static void recalcModelView(void)
{
#ifdef SB_BUILD_TRACKBALL
	if (isInitTrackball)
		return;

	// only update the base view matrix if trackball/simple rotation is changed by player

	glPopMatrix();
	glPushMatrix();

	glRotatef(angle, 0.0, 1.0, 0.0);			// for simple trackball
	//glTranslatef(-8, -8, -bodyWidth / 2);
	isInitTrackball = 1;
#endif
}



/* -------------------------
glut callback function
------------------------- */
void vGlutReshape(int w, int h)
{
  SBOnResizeViewport(w,h);
}

void SBOnResizeViewport(int w, int h)
{
  glViewport(0, 0, w, h);
  Screen_W = w;
  Screen_H = h;
}


#define ABS(__i) ((__i)>0 ? (__i) : -(__i) )
unsigned short	LastTouchX=0;
unsigned short	LastTouchY=0;

/// change touch input to stick input (Last_Actions)
bool SBOnTouchEvent( unsigned short x, unsigned short y, unsigned short state )
{
#ifdef SB_BUILD_VIRTUAL_PAD
//	dbg_msg("SBOnTouchEvent(): %d  %d, %d \n", state, x,y);

	if (state == 2) //UP
	switch ( Scene_State )
	{
	case SCENE_TITLE:
		if ( !game_paused )
		{
			Sub_State++;
			return true;
		}
		break;

	case SCENE_PLAY:
		if ( game_paused )
		{
//			SBOnTogglePause();
		}
		break;

	case SCENE_END:
//		if ( sIsDebugStartScene(key) )
		{
//			if( Sub_State==SUB_STATE0 )
//			dbg_msg("SBOnTouchEvent() SCENE END: counter:%d \n", s_CountDownBeforeGoingToTitle);
			return true;
		}
#if 0
//ifdef DEBUG
		if ( sIsDebugNextScene(key) )
		{
			Is_GameOver = FALSE;
			Is_GameCleared = TRUE;
		}
#endif
		break;
	}

	if ( game_paused || Game_Time < 10 )        //dirty: don't accept input when time < 10
	{
		return true;
	}

	//---// update virtual gamePad
	if ( s_touchPadMode == 0 )
	{
		float TOUCH_OFFSET = 3.0f;
		unsigned short TOUCH_THRESHOLD = 0;

		if ( state == 1 )	// MOVE
		{
			if ( ABS(x-LastTouchX) >= TOUCH_THRESHOLD )
			{
				float angleX = (x-LastTouchX) * TOUCH_OFFSET;
				Last_Actions[RIGHT] = angleX>0;
				Last_Actions[LEFT] = angleX<0;
			}
			if ( ABS(y-LastTouchY) >= TOUCH_THRESHOLD )
			{
				float angleY = (y-LastTouchY) * TOUCH_OFFSET;
				Last_Actions[DOWN] = angleY<0;
				Last_Actions[UP] = angleY>0;
			}
			s_isTouchDown = true;
		}
		else if ( state == 2 ) //UP
		{
			for ( int i=UP; i <= RIGHT; i++ )
			{
			//	Last_Actions[i] = false;
			}

  			s_isTouchDown = false;
			return true;
		}

		// DOWN
		LastTouchX = x;
		LastTouchY = y;
	}
	else if (s_touchPadMode == 1 )
	{
		if ( state == 2 ) //UP
		{
			for ( int i=UP; i <= RIGHT; i++ )
			{
				Last_Actions[i] = false;
			}

  			s_isTouchDown = false;
			return true;
		}
		else
		{
			s_isTouchDown = true;
		}

		//convert x,y to DPAD game keys
		const S16 diffx = S16(x) - DPAD_X;
		const S16 diffy = Screen_H - S16(y) - DPAD_Y;

		//dbg_msg("SBOnTouchEvent(): %d  %d, %d \n", state, diffx, diffy);

		if ( ABS(diffx) > DPAD_SIZE_OUT 
			||  ( ABS(diffy) > DPAD_SIZE_OUT ) )
		{
			Last_Actions[LEFT] = Last_Actions[RIGHT] = false;
			Last_Actions[UP] = Last_Actions[DOWN] = false;
		}
		else
		{
			if (DPAD_SIZE_IN <= ABS(diffx))
			{
				const char i = diffx > 0 ? RIGHT:LEFT;
				const char j = i==LEFT? RIGHT:LEFT;
				Last_Actions[i] = true;
				Last_Actions[j] = false;
				//dbg_msg("SBOnTouchEvent() DIFF: %d  %d, %d \n", state, diffx, x);
			}

			if ( DPAD_SIZE_IN <= ABS(diffy) )
			{
				const char i = diffy > 0 ? DOWN:UP;
				const char j = i==UP? DOWN:UP;
				Last_Actions[i] = true;
				Last_Actions[j] = false;
			}
		}
	}
	else if ( s_touchPadMode==2 )
	{
		for ( int i=UP; i <= RIGHT; i++ )
		{
			Last_Actions[i] = false;
		}

		if ( state == 1 )	// MOVE
		{
			float p[2];
			// map screen coor to our coor
			p[0] = CHAN_X * x / Screen_W + (-CHAN_X*0.5);
			p[1] = - CHAN_Y * y / Screen_H + (CHAN_Y*0.5);
			//dbg_msg("SBOnTouchEvent(): res:  %.2f  %.2f  \n", p[0], p[1] );

			//calculate vectors and then map it to Last_Actions[]
			for ( char i=0; i<2; ++i )
			{
				const float diff = p[i] - Dream_State.pos[i];
				if ( fabs(diff) > 0.2 )
				{
					const char dir = i==0 ? LEFT : UP;
					Last_Actions[dir] = diff < 0;
					Last_Actions[dir+1] = !Last_Actions[dir];
				}
			}

			s_isTouchDown = true;
		}
		else //if ( state == 2 ) //UP
		{
			s_isTouchDown = false;
		}

//		dbg_msg("onTouch():  input:%d%d \n", Last_Actions[LTURN], Last_Actions[RTURN] );
	}
#endif
	return true;
}



#ifdef SB_ANDROID

// map key input to game key
#define _CASE_ON_KEY(_key, _action) \
case _key: \
	Last_Actions[_action] = (state != KEY_UP); \
	break;

#endif


#ifdef SB_DEBUG

//TOREFACTOR input module
const unsigned int SB_KEY_Y = 53;

#ifdef SB_WIN
const char sDebugKeyArr[] = { 't', 'y', 'v' };
#else
const char sDebugKeyArr[] = { 48, SB_KEY_Y, 50 };
#endif

#define sIsDebugNextScene(_k_) (_k_ == sDebugKeyArr[1])
#define sIsDebugStartScene(_k_) (_k_ == sDebugKeyArr[0])

#endif

enum
{
	KEY_DOWN,
	KEY_UP,
	KEY_MULTIPLE,
};


/*
 * key      code of the keyboard key
 * state    up or down
 */
bool SBOnKeyEvent( int key, unsigned short state )
{
	dbg_msg("SBOnKeyEvent(): key:%d  %d \n", key, state);

	if (state == KEY_UP)
	switch ( Scene_State )
	{
	case SCENE_TITLE:
		if ( key == ' ' || key == 62 )
		{
			Sub_State++;
			return true;
		}
		break;

#if 1
//#ifdef SB_DEBUG
	case SCENE_PLAY:
		if ( sIsDebugStartScene(key) )
		{
			Draw_Engine.init();
			return true;
		}
		if ( sIsDebugNextScene(key) )
		{
			Scene_State++;
			Sub_State = SUB_STATE0;
			Is_GameOver = TRUE;
			Is_GameCleared = FALSE;
			return true;
		}
		break;

	case SCENE_END:
		if ( sIsDebugStartScene(key) )
		{
			Draw_Engine.init();
			return true;
		}
		if ( sIsDebugNextScene(key) )
		{
			Is_GameOver = FALSE;
			Is_GameCleared = TRUE;
		}
		break;
#endif
	}

	//----// Dream's action
#ifdef SB_ANDROID

	//WSAD
	if ( !game_paused )
	{
		switch (key)
		{
		_CASE_ON_KEY(51,UP);		// android.view.KeyEvent.KEYCODE_W
		_CASE_ON_KEY(33,UP);		// KEYCODE_E
		_CASE_ON_KEY(47,DOWN);
		_CASE_ON_KEY(52,DOWN);
		_CASE_ON_KEY(54,DOWN);
		_CASE_ON_KEY(29,LEFT);
		_CASE_ON_KEY(32,RIGHT);
		_CASE_ON_KEY(38,SPUP);
		_CASE_ON_KEY(41,SPDOWN);
		_CASE_ON_KEY(42,LTURN);
		_CASE_ON_KEY(105,RTURN);
		_CASE_ON_KEY(66,FIRE);		// KEYCODE_ENTER
		}
	}
#endif

	//
	if (state == KEY_UP)
	if ( key == 'p' || key == 44 )
	{
		SBOnTogglePause();
#ifdef SB_WIN
		if ( !game_paused )
		{
			glutTimerFunc(frame_rate, vGlutTimer, TIMER0);
			last_redraw_time = glutGet(GLUT_ELAPSED_TIME);
			glutAttachMenu(GLUT_RIGHT_BUTTON);
		}
		else
			glutDetachMenu(GLUT_RIGHT_BUTTON);
	}
#ifdef SB_DEBUG
	else if ( key == 'q' )
		exit(0);
#endif
#else
		return true;
	}
#endif

	return false;
}

/*
 * key      key code of the VirtualGameKey
 * state    up or down
 */

//dirty5: this enum is also on the Java side :(
enum VirtualGameKey
{
	VirtualGameKey_MIN,
	ROTATE_LEFT=0,
	ROTATE_RIGHT,
	GO_TITLE,
	TOGGLE_LIGHT,
	SPEED_DOWN,
	DEBUG_SETTINGS,
	VirtualGameKey_MAX,
};

bool SBOnVirtualGameKeyEvent( int key, unsigned short state )
{
//	dbg_msg("SBOnGameKeyEvent(): key:%d  %d \n", key, state);

	assert(key >= VirtualGameKey_MIN && VirtualGameKey_MIN < VirtualGameKey_MAX);

	//----// Dream's action
//#ifdef SB_ANDROID
#ifdef SB_DEBUG
	bool oldActionVal = 2;	// DEBUGonly!

	// map virtual key input to game key (and toggle the other game key)
#define _CASE_ON_KEY2(_key, _action, _toResetAction) \
	case _key: \
		oldActionVal = Last_Actions[_action]; \
		Last_Actions[_action] = (state != KEY_UP); \
		if ( Last_Actions[_action] && Last_Actions[_toResetAction] )  \
			Last_Actions[_action] = Last_Actions[_toResetAction] = false; /*turn back to normal*/  \
		else if ( Last_Actions[_action] )  \
			Last_Actions[_toResetAction] = false;  \
	dbg_msg("SBOnVirtualGameKeyEvent():    _action:%d  %d -> %d   \n", _action, oldActionVal, Last_Actions[_action] );   \
		break;

#define _CASE_ON_KEY3(_key,_CBFUNC) \
	case _key: \
	_CBFUNC; \
	break;

	// special key
	switch (key)
	{
	case GO_TITLE:
//		if ( Scene_State == SCENE_END )
		{
			Draw_Engine.init();
		}
		break;

	//debug
	case TOGGLE_LIGHT:
		Light_On = !Light_On;
		break;
	}

	// key on controller for dream
	if ( !game_paused )
	{
		switch (key)
		{
		_CASE_ON_KEY2(ROTATE_LEFT, LTURN, RTURN );
		_CASE_ON_KEY2(ROTATE_RIGHT, RTURN, LTURN );

		_CASE_ON_KEY(SPEED_DOWN, SPDOWN);

		_CASE_ON_KEY3( DEBUG_SETTINGS, SBOnKeyEvent( SB_KEY_Y, 1 ) );    //debug only
		}
	}
#endif
	return false;
}

//---------------------------------------------------------------//
// Section: timer control
//---------------------------------------------------------------//

#ifdef SB_WIN

/* -------------------------
glut callback function
------------------------- */
void vGlutIdle()
{
	assert(0);
}


/* -------------------------
glut callback function - call redisplay() for animation and set timer for next frame
------------------------- */
void vGlutTimer(int timerId)
{
	// In glut's spec: "There is no support for canceling a registered callback. 
	// Instead, ignore a callback based on its value parameter when it is triggered."

	if ( timerId == TIMER0 && !game_paused )
	{
		glutPostRedisplay();

		int cur = glutGet(GLUT_ELAPSED_TIME);
		int next = 2*frame_rate-cur+last_redraw_time;		// next = frame_rate - delayed time

		if ( next > 0 )
		{
			glutTimerFunc(next, vGlutTimer, TIMER0);
		}
		else
		{
			static int slow_sys_detected_count = 0;

			if ( Scene_State == SCENE_PLAY && slow_sys_detected_count <= 25 )	// warn at most five times
			{
				slow_sys_detected_count++;

				if ( slow_sys_detected_count > 20 )
					printf("WARNING: It was found that your display card or system is not\n fast enough to run this game in a normal frame rate.\n\n");
			}
			glutTimerFunc(10, vGlutTimer, TIMER0);
		}

		last_redraw_time = cur;
	}
}



//---------------------------------------------------------------//
// Section: user input: keyboard, mouse, menu, etc
//---------------------------------------------------------------//

/* -------------------------
glut callback function
------------------------- */
void vGlutKeyb(unsigned char key, int x, int y)
{
	SBOnKeyEvent(key,1);
}


/* -------------------------
// glut callback function
// for accepting function keys, arrow keys and home/end keys, etc
------------------------- */
void vGlutSpKeyb(int key, int x, int y)
{
	//updateKeyStates();
	return;
}


/* -------------------------
glut callback function
------------------------- */
void vGlutMouse(int button, int state, int x, int y)
{
#if defined(SB_BUILD_VIRTUAL_PAD) && defined(SB_WIN)
	SBOnTouchEvent( x, y, state == GLUT_DOWN ? 0: 2 );
#else // SB_BUILD_TRACKBALL
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		moving = 1;
		begin = x;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		moving = 0;
	}
#endif
}


/* -------------------------
glut callback function
------------------------- */
void vGlutMotion(int x, int y)
{
#if defined(SB_BUILD_VIRTUAL_PAD) && defined(SB_WIN)
	SBOnTouchEvent( x, y, 1 );
#else // SB_BUILD_TRACKBALL
	if (moving)
	{
		angle = angle + (x - begin)*2;		// *2: multiplier for control of sensitivity
		begin = x;
		isInitTrackball = 0;
		glutPostRedisplay();		//TODOA we should remove it!?
	}
#endif
}

/*-------------------------
glut callback function
-------------------------*/
void vGlutMainMenu(int value)
{
	static GLboolean lightZeroSwitch = GL_TRUE, lightOneSwitch = GL_TRUE;

	switch (value)
	{
	case 4:
		//2000-1011 no support for this:
		assert(0);
		glutFullScreen();
		break;
	case 5:
		exit(0);
		break;
	}

	glutPostRedisplay();
}

/*-------------------------
glut callback function
-------------------------*/
void vGlutSetDiffiMenu(int value)
{
	set_diffi(value);
}

/*-------------------------
glut callback function
-------------------------*/
void vGlutSetLightMenu(int value)
{
	set_light(value==1);
}
#endif


/* -------------------------
glut callback function
------------------------- */
void updateKeyStates()
{
//#ifdef
	if ( s_isTouchDown )	// ignore key input during touching
	{
		return;
	}


#ifdef SB_ANDROID
  /* Starting from API level 9, use 
  <android/input.h>
  <android/keycodes.h>
  <android/sensor.h>
  */
#elif defined(SB_WIN)
	BYTE key_state[256];

	if ( GetKeyboardState(key_state) )				/** Win32 dependency!! **/
	{
		for ( int i=0; i < DR_NUM_BUT; i++ )
		{
			BYTE c = (key_state[Device_Ids[i]] )>>7;	// get highest bit

			// NOTE: perhaps we should clearLastActions so that the display we can respond
			// the user input
			Last_Actions[i] = (c == 1);

			#ifdef DEBUG_MSG
				if (c==1)
					dbg_msg("Last_Actions %d\n", i);
			#endif
		}
		//dbg_msg("-----------\n");
	}
	else
		assert(0);
#endif
}



//---------------------------------------------------------------//
// Section: main and redraw callback functions
//---------------------------------------------------------------//

//void SBInit();
void SBDrawMainGlut(void);
//void SBDeinit(void);

#ifdef SB_WIN
/*-------------------------
C main function!!!
-------------------------*/
int main(int argc, char **argv)
{
	printf("Initializing system...\n");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );	//| GLUT_MULTISAMPLEb
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	glutInitWindowPosition(40, 40);
	//trackball(curquat, 0.0, 0.0, 0.0, 0.0);
	win_ids[0] = glutCreateWindow("Starbird w98&nt");	// // !!! this line slows things down in debug mode // mem leak was reported in Purify!

	//--------- call backs
	glutDisplayFunc(SBDrawMainGlut);
	glutReshapeFunc(vGlutReshape);
	glutKeyboardFunc(vGlutKeyb);
	glutSpecialFunc(vGlutSpKeyb);
	//glutIdleFunc(vGlutIdle);

	//glutVisibilityFunc(vis);
	glutMotionFunc(vGlutMotion);
	glutMouseFunc(vGlutMouse);
	glutReshapeWindow(WIN_WIDTH, WIN_HEIGHT);

	//--------- sub Menu
	int menuId = glutCreateMenu(vGlutSetDiffiMenu);
	
	glutAddMenuEntry("Baby", 1);
	glutAddMenuEntry("Normal", 2);
	glutAddMenuEntry("Expert", 3);

	//--------- sub Menu
	int menuId2 = glutCreateMenu(vGlutSetLightMenu);
	
	glutAddMenuEntry("On", 1);
	glutAddMenuEntry("Off", 2);

	//--------- main Menu
	glutCreateMenu(vGlutMainMenu);

	// glutAddMenuEntry("Full screen", 4);
	glutAddSubMenu("Difficulty", menuId);
	glutAddSubMenu("Lighting", menuId2);
	glutAddMenuEntry("Quit", 5);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	SBInit();

	//--------------// set timer and call mainloop

	// set timer before main loop.
	glutTimerFunc(frame_rate, vGlutTimer, TIMER0);
	last_redraw_time = glutGet(GLUT_ELAPSED_TIME);

	printf("Opening window...\n");
	glutMainLoop();
	return 0;             /* ANSI C requires main to return int. */
}
#endif

void SBInitApp()
{
	dbg_msg("SBInit:InitApp() ===============================");

	Draw_Engine.init();			// init game states

#ifdef SB_DEBUG
//	set_light( FALSE );
#endif
}

void SBDeinitApp()
{
	//TODOF delete and deallocate al stuff?
}

void SBInit()
{
//	static bool isInitialized = false;
//	if (isInitialized)
	{
//		return;
	}

	//REF: use: http://www.khronos.org/opengles/sdk/1.1/docs/man/glGetString.xml
	const GLubyte* verStr = glGetString(GL_VERSION);	
	dbg_msg("SBInit: OGL info: %s, %s, %s \n", glGetString(GL_VENDOR), glGetString(GL_RENDERER), verStr );

	//----------- set view mode
	#ifdef BACK_FACE
		glEnable(GL_CULL_FACE);		// back face culling
	#endif

	glEnable(GL_DEPTH_TEST);		// z-buffering set above

	if (Light_On)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	glEnable(GL_DITHER);			// ON by default
	glDisable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);		// ON by default	

    //glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_COLOR_ARRAY);

  //--------------// initialize matrices
	glMatrixMode(GL_PROJECTION);
	//	gluPerspective( /* field of view in degree */ 40.0,
	// /* aspect ratio */ 1.0,
	// /* Z near */ 1.0, /* Z far */ 40.0);
	gluPerspective( /* field of view in degree */ 100.0,
	/* aspect ratio */ 1.33,
	/* Z near */ 1.0, /* Z far */ 500.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();                // Load unity on current matrix
	
	/*
	gluLookAt(0.0, 0.0, 10.0,  //* eye is at (0,0,30) 
	0.0, 0.0, -1.0,      //* center is at (0,0,0) 
	0.0, 1.0, 0.0);      //* up is in positive Y direction 
	*/
	glPushMatrix();      /* dummy push so we can pop on model recalc */  //2013 BUGHERE???

	//--------------// init game state

	printf("Loading resources...\n");

	vMakeAll();					// make objs and lights; depends on init's result
	GetOpenGLError();
}

void SBDeinit()
{
}

static void onPauseChanged()
{
	if ( game_paused )
	{
		// if we have backup, don't backup again...
		if ( Speed != 0 )  //buggy check
		{
			Speed_Bkup = Speed;
		}

//		setMainMessage("push 'p' to continue");
		setMainMessage("Paused");
	}
	else
	{
		setMainMessage("");

		//resetInput
		for ( int i=UP; i <= RIGHT; i++ )
		{
			Last_Actions[i] = false;
		}

	}

	{
		Draw_Engine.setUpdateSpeed( game_paused ? 0.0f : Speed_Bkup );
	}
}

void SBOnPause( bool isPause )
{
	if ( game_paused == isPause )
	{
		return;
	}

	game_paused = isPause;
	dbg_msg("SBOnPause(): game_paused1:%d \n", game_paused );

	onPauseChanged();
}

void SBOnTogglePause()
{
	game_paused = !game_paused;
	dbg_msg("SBOnPause(): game_paused2:%d \n", game_paused );

	onPauseChanged();
}

/* -------------------------
It is the main callback of glut: glutDisplayFunc(vGlutRedraw).
------------------------- */
void SBDrawMainGlut(void)
{
	SBDrawMain(-1 );	//,WIN_WIDTH,WIN_HEIGHT);
}

void SBDrawMain(long tick) //, int width, int height)
{
#ifdef SB_ANDROID //TEMP to do it here
//	if ( game_paused )
	{
//		return;
	}
#else

#endif

//	vGlutReshape(width,height);
	recalcModelView();

	assert(!GetOpenGLError());

	//------------//
	// because glut don't provide keyUpCB, otherwise we should call this at keyUp and keyDown CBs
	updateKeyStates();

	//------------//
	switch ( Scene_State )
	{
	case SCENE_TITLE:
		if ( ! Draw_Engine.drawTitleScene(Sub_State) )
		{
			Scene_State = SCENE_PLAY;
			Sub_State = SUB_STATE0;
		}
		break;

	case SCENE_PLAY:
		if ( ! Draw_Engine.drawPlayScene(Sub_State) )
		{
			Scene_State = SCENE_END;
			Switch_Back_2_Title_Counter = 300;
			dbg_msg("SceneEnd: %d \n", Switch_Back_2_Title_Counter );
		}

		// The old design that this time depends on redraw rate is VERY bad!
		// Now, decreasing frame rate also make the object move slower.
		// Also, it's hard to tune Dream's min/max speed
		//
		if (!game_paused)
		{
			Game_Time++;
		}

	    clearLastActionsAfterUpdate();      // reset those input after we referenced it
		break;

	case SCENE_END:
		//TODOA2 CLEAN: remove these two variables
		if ( Is_GameOver )			// Game over
		{
			Sub_State = SUB_STATE0;
		}
		else if ( Is_GameCleared == TRUE )	// Game Clear
		{
			Sub_State = SUB_STATE1;
		}

		Draw_Engine.drawEndScene(Sub_State);

		// need this for Android for showing sequence of on-screen message
		// #ifdef SB_ANDROID?
		if (!game_paused && Is_GameCleared == TRUE )
		{
			Game_Time++;
		}

        if ( --Switch_Back_2_Title_Counter == 0 )
        {
	        dbg_msg("SceneEnd: %d \n", Switch_Back_2_Title_Counter );
	        SBOnVirtualGameKeyEvent( GO_TITLE, 1 );
        }
		break;
	}

	//dbg_msg("DrawMain(): end");
	assert(!GetOpenGLError());

	//------------//
	mySysSwapBuffers();
}
