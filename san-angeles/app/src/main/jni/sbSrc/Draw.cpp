
/*=====================================

 file: Draw.cpp

 Owner: Hui Tin Yuen, Fred

 Description:
 
 Includes scene drawing functions for aircraft, background elements (light, wall, 
 tunnel, etc) and many game-state variables.

=====================================*/

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
#include "Dream.h"


//#define DEBUG_USING_SPACEBALL

//---------------------------------------//
// legacy constants
#if 0
#define  DIR_NONE      0x00
#define  DIR_X         0x01
#define  DIR_Y         0x02
#define  DIR_Z         0x04

#define  SENS_PANX     30.0
#define  SENS_PANY     23.0
#define  SENS_ZOOM     35.0
#endif

//---------------------------------------//
#ifdef DEBUG_USING_SPACEBALL

typedef int Vector[4];
typedef Vector Matrix[4];


// few matrices to support trackball rotation

GLMatrix mbaseRotations  = {1.0, 0.0, 0.0, 0.0,
                        0.0, 0.91, 0.42, 0.0,
                        0.0, -0.42, 0.91, 0.0,
                        0.0, 0.0, 0.0, 1.0},
       mbaseTranslations = {1.0, 0.0, 0.0, 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0,
                        0.0, 0.0, -25.0, 1.0},
       mbaseIdentity     = {1.0, 0.0, 0.0, 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0,
                        0.0, 0.0, 0.0, 1.0};

GLMatrix mRotations    = {1.0, 0.0, 0.0, 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0,
                        0.0, 0.0, 0.0, 1.0},
       mTranslations = {1.0, 0.0, 0.0, 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0,
                        0.0, 0.0, 0.0, 1.0},
       mIdentity     = {1.0, 0.0, 0.0, 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0,
                        0.0, 0.0, 0.0, 1.0};

//short sXReference, sYReference;

//long XValDiff = 0, YValDiff = 0;
int  MouseOn[3] = { 0, 0, 0 };

Matrix lastMat = {{1.0, 0.0, 0.0, 0.0},
   {0.0, 1.0, 0.0, 0.0},
   {0.0, 0.0, 1.0, 0.0},
   {0.0, 0.0, 0.0, 1.0}};

#endif



//---------------------------------------//
// our dirty global objects

DrawEngine Draw_Engine;
DreamState Dream_State;

//---------------------------------------//
// game state variables

// message string holders
char Main_Mesg[128] = "\0";				// the message is centered on the screen
char Help_Mesg[128] = "\0";				// the message is shown on the bottom screen
const char Warning_Mesg[] = "WARNING";
const char GameOver_Mesg[] = "GAME OVER";
const char Ready_Mesg[] = "Get Ready!!";


// game variables for Dream
int Game_Time = 0;
int Energy = FULL_ENG;
int Passes = 0;						// # of rings (checkpoints) passed
int Passes_Count = 0;				// total number of rings of the current stage
float Speed  = NOR_SPEED;			//DIRTY: delta speed(distance) per frame!!
float Speed_Bkup = NOR_SPEED;
int Sp_Eng  = FULL_SPENG;

// game variables for configuration (file/menu, etc)
int Light_On = TRUE;
float Diffi = BABY;					// difficulty level

// variables for input
char Last_Actions[DR_NUM_BUT];		// last captured keyboard states
int Screen_W = WIN_WIDTH;
int Screen_H = WIN_HEIGHT;

// variables for turning corner
int Turn_Mode = FALSE;					// true if dream is turning corner
float Ob_Yang;							// dream's rotatoin around y-axis
float Obz[TURN_NUM] =	{ 0, 0 };		// Dream's z coordinate
float Obx[TURN_NUM] =	{ 0, 0 };		// Dream's x coordinate
int Act_Turn = -1;						// current turn/corner id



//---------------------------------------//
// static int mainmenu, sets, lights, diff;

bool Is_GameOver = FALSE;			// if game is over
bool Is_GameCleared = FALSE;		// if game is cleared
//int Input_win = -1;
int ch_lplane = FALSE;				// on/off for bound checking: left side
int ch_rplane = FALSE;				// on/off for bound checking: right side
unsigned int T_w=0, T_h=0;			// texture width and height
GLubyte Pixel_Arr[1000*1000][3];	// texture data
float Eval_Angle;					// this is the angle of Dream to x-z plane


//-------------------------
//

int Scene_State = SCENE_END;		// SCENE_TITLE, SCENE_END
int Sub_State = SUB_STATE1;			// SUB_STATE0, SUB_STATE1
unsigned int Switch_Back_2_Title_Counter = 0;

/*
//TODOF2 put this in C++ class
int getGameState()
{
	return Scene_State == SCENE_TITLE; //2000 for set_diff
}
*/



//---------------------------------------------------------
// set message to be displayed later on screen
//---------------------------------------------------------
void setMainMessage(const char* msg)
{
	assert(strlen(msg) < 127);

	if (strlen(msg) < 127)
	{
		Dstrcpy(Main_Mesg, msg);
	}
}

void setHelpMessage(const char* msg)
{
	assert(strlen(msg) < 127);

	if (strlen(msg) < 127)
	{
		Dstrcpy(Help_Mesg, msg);
	}
}

//---------------------------------------------------------
// clear last user input
//---------------------------------------------------------
void clearLastActions()
{
	//for ( int i=0; i<DR_NUM_BUT; i++)
	//	Last_Actions[i] = FALSE;

	memset(Last_Actions, 0, DR_NUM_BUT);
}

void clearLastActionsAfterUpdate()
{
#ifdef SB_ANDROID
	Last_Actions[SPUP] = FALSE;
	Last_Actions[SPDOWN] = FALSE;
#endif
}


//---------------------------------------------------------
// Draw Light at (x,y,z) with light id be ligId
// i is not used in windows version.
//---------------------------------------------------------
void vDrawLight(int , int ligId, float x, float y, float z)
{
//	debugONLY
//	if(ligId > GL_LIGHT1 )
//		return;

	glPushMatrix();

	GLfloat lightZeroDir[] = {0.0, -1.0, 0.0, 1.0};
	GLfloat pos[] = {x,y,z,1};

	if ( Light_On == TRUE )
	{
		//lmbind(i, j);
		//lmcolor(LMC_AD);
		
		glEnable(ligId);
		
		glLightfv(ligId, GL_POSITION, pos);
		glLightfv(ligId, GL_SPOT_DIRECTION, lightZeroDir);
		
		glColor3fv(fYellowVec);

#ifdef  SB_GLES
		//TODO A1??
#else
		glColorMaterial(GL_FRONT, GL_AMBIENT);
#endif


		glDisable(GL_LIGHTING);
//		glEnable(GL_COLOR_MATERIAL);

		glTranslatef(x, y, z);
		glScalef(0.2, 0.2, 0.2);
		vDrawObj(OBJ_CUBE4);	// glCallList

		//		glDisable(GL_COLOR_MATERIAL);
		glEnable(GL_LIGHTING);

		//lmcolor(LMC_COLOR);
	}
	else
	{
		//lmbind(i,0);
		//glDisable(GL_LIGHTING);		// or glDisable(ligId);
	}
	glPopMatrix();
}


//---------------------------------------------------------
// draw all lights along with the corridor
//---------------------------------------------------------
void vDrawLights()
{
	if ( Light_On != TRUE)
		return;

	static float s_offset=0.0f;
	int lig[LIGHT_COUNT];
	int lig_id[LIGHT_COUNT];
	float i;
	int j;	

	for (j=0; j<LIGHT_COUNT; j++)
	{
		lig[j] = lig_id[j] = j+GL_LIGHT0;
	}

	i = s_offset;
	for ( j = 0; (j < LIGHT_COUNT) && (i > -CHAN_Z*0.3); j++ )
	{
		if ( i < 0 ) // skip light which is passed away
			vDrawLight(lig[j], lig_id[j], 0.0, CHAN_Y/2, i-0.1);
		i -= LIG_DIST;
	}

//	if ( !game_paused )
		s_offset += Speed;

	if ( s_offset >= LIG_DIST )
		s_offset = 0.0;
}

//---------------------------------------------------------
// Draw basic scene elements: wall
//---------------------------------------------------------
void vDrawWall(float x, float y, float z)
{
	glPushMatrix();
	glTranslatef(0.0, 0.0, z);
	glScalef(CHAN_X, CHAN_Y, WALL_Z);
	vDrawObj(OBJ_CHANNEL);		//glCallList(OBJ_CHANNEL);  which is a unit-size object
	glPopMatrix();
}

//---------------------------------------------------------
// Draw basic scene elements: draw tunnel
//   int ligsOn - specifies whether the lights on the roof should be applied.
//---------------------------------------------------------
void vDrawTunnel(int ligsOn)
{
	static float s_offset = 0.0;
	int lig[LIGHT_COUNT];
	int lig_id[LIGHT_COUNT];
	float i,z,z2;
	int j;

//	if ( ligsOn == TRUE )
//		setup_lig();

	for (j=0; j<LIGHT_COUNT; j++)
	{
		lig[j] = lig_id[j] = j+GL_LIGHT0;
	}

	for ( i=s_offset, j=0; i<CHAN_Z/2 + 10.0; i += WALL_Z, j++)
	{
		z = i-CHAN_Z/2.0+10.0;
		z2 = z+CHAN_Z*0.25;
		//lmdef(DEFMATERIAL, OBJ_DREAM, 5, wall_mat);
		vDrawWall(0.0, 0.0, z );
/*
printf("  z = %f, z2 = %f\n", z, z2);
*/

//		if ( (j < LIGHT_COUNT && z2 < 8.0 ) && ligsOn == TRUE )
//			vDrawLight(lig[j], lig_id[j], 0.0, CHAN_Y/2, z2);		//BUGHERE: find during w95 ver
    }

//	if ( !game_paused )
//debugonly?
//		s_offset += Speed;

	if ( s_offset >= WALL_Z )
        s_offset = 0.0;
}

//---------------------------------------------------------
// Draw basic scene element: a black square (line loop) surrounded the tunnel
//---------------------------------------------------------
void vDrawSq(float x, float y, float z)
{
	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(CHAN_X, CHAN_Y, 0.0);
	vDrawObj(OBJ_SQUARE);
	glPopMatrix();
}

//---------------------------------------------------------
// Draw basic scene elements: a list of black squares (line loop) surrounded the tunnel
//---------------------------------------------------------
void vDrawSqs( bool isUpdate )
{
    float i;
    static float offset = 0.0;

	glDisable(GL_LIGHTING);
    for ( i=offset; i<CHAN_Z/2; i += SQ_DIST )
    {
        vDrawSq(0.0, 0.0, i-CHAN_Z/2.0);
    }
	glEnable(GL_LIGHTING);

	if ( isUpdate )
		offset += Speed;

	if ( offset >= SQ_DIST )
        offset = 0.0;
}

//---------------------------------------------------------
// Draw basic scene elements: 
//    yellow rectangle which looks a rectanglar light on floor of tunnel
//---------------------------------------------------------
void vDrawRect(float x, float y, float z)
{
	//TODOA8 speedup without push/pop
	glPushMatrix();
	glTranslatef(x, y, z);
	vDrawObj(OBJ_RECT);
	glPopMatrix();
}

//---------------------------------------------------------
// Draw basic scene elements: 
//    a list of yellow rectangles on floors
//---------------------------------------------------------
void vDrawRects( bool isUpdate ) {
	if (Light_On == TRUE) {
		glDisable(GL_LIGHTING);
	}

	float i;
	static float offset = 0.0;

	for (i = offset; i < CHAN_Z / 2; i += RECT_LEN * 1.5) {
		vDrawRect(CHAN_X / 2.0 - 0.5, -CHAN_Y / 2.0 + OFFSET_FROM_WALL, i - CHAN_Z / 2.0 + 2.0);
		vDrawRect(-CHAN_X / 2.0 + 0.5, -CHAN_Y / 2.0 + OFFSET_FROM_WALL, i - CHAN_Z / 2.0 + 2.0);
	}

	if (isUpdate)
	{
		offset += Speed;
		if ( offset >= 1.5 * RECT_LEN )
			offset = 0.0;
	}

	if (Light_On == TRUE )
	{
		glEnable(GL_LIGHTING);
	}
}


//---------------------------------------------------------
// Draw basic scene elements: bullet
//---------------------------------------------------------
void vDrawFire(float x, float y, float z)
{
	//TODOA8 speedup without push/pop
	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(2.0, 0.0, 2.0);
	vDrawObj(OBJ_FIRE);
	glPopMatrix();
}

//---------------------------------------------------------
// Draw basic scene elements: a list of bullets
//    the number of these depends on the way player pushes the fire button
//---------------------------------------------------------
void vDrawFires(int new_fire)
{
	if ( Speed <= 0.0f )
		return;

	int i;
	static int active = 0;
	static float z[FIRE_NUM] =	{ .0,.0,.0,.0,.0,.0,.0 };
	static int next = 0;

	if (Light_On == TRUE)
	{
		glDisable(GL_LIGHTING);
	}

	if ( new_fire == TRUE )
	{
		z[next] = NORM_Z_POS - 0.3;
		next++;
		if ( next == FIRE_NUM )
			next = 0;
	}

	for ( i=0; i<FIRE_NUM; i++)
	{
		if ( z[i] > -CHAN_Z/2.0 && z[i] < NORM_Z_POS )
		{
			vDrawFire(z[i], 0.0, 0.6);
			z[i] -= 3.0;
		}
		else if ( active == i && active < next )
			active++;
	}

	if (Light_On == TRUE)
	{
		glEnable(GL_LIGHTING);
	}
}

//---------------------------------------------------------
// Draw Dream's Shadow.
// First draw body and then the four wings
//---------------------------------------------------------
void vDrawDreamShadow(float x, float y, float z, float xang, float yang, float zang, float wang, float wang2)
{
	float sc;

	if ( Light_On == FALSE )
		return;

	if ( Game_Time >= END_TIME )
		return;

	sc = DR_SCALE * 1.44/( y + CHAN_X/2.0 );

	glPushMatrix();
	glTranslatef(x, 0.011-CHAN_Y*0.5, z);
	glRotatef(-90.0, 0,1,0);
	glRotatef(yang, 0,1,0);
	glScalef(sc, sc, sc*(0.2 + 1.0 - fabs(xang) * 0.011111));
	//lmdef(DEFMATERIAL, OBJ_DREAM, 5, sha_mat);
	dream::drawShaPart(OBJ_DRSHA);

	glPushMatrix();
	glRotatef(wang, 0,1,0);
	dream::drawShaPart(OBJ_DRSHA_LL);
	glRotatef(-2*wang, 0,1,0);
	dream::drawShaPart(OBJ_DRSHA_LR);
	glPopMatrix();

	glRotatef(wang2, 0,1,0);
	dream::drawShaPart(OBJ_DRSHA_TL);
	glRotatef(-2*wang2, 0,1,0);
	dream::drawShaPart(OBJ_DRSHA_TR);
	glPopMatrix();
}

//---------------------------------------------------------
// Draw Dream Body.
// First draw body and then the four wings
//---------------------------------------------------------
void vDrawBody(float v_z, float r_x, float r_y, float r_z, 
					float x, float y, float z, float xang, float yang, float zang, float wang)
{
#if 0
	float pt[3];

	pt[0] = x;
	pt[1] = y;
	pt[2] = z;

	/*if ( Is_GameOver == TRUE )
	{
		if ( Energy <= 0.0 )
			vDrawSparks(pt, Game_Time);
		else
			vDrawSparks(pt, 0);
		return;
	}*/
#endif

	glPushMatrix();
	vDrawDreamShadow(x,y,z,xang,yang,zang,wang,wang);
	glTranslatef(x, y, z);
	glRotatef(-90.0, 0,1,0);		// since Dream was made in 90 degree around y-axis

	glRotatef(yang, 0,1,0);
	glRotatef(xang, 1,0,0);
	glRotatef(zang, 0,0,1);

	glScalef(DR_SCALE,DR_SCALE,DR_SCALE);
	dream::drawPart(OBJ_DREAM);
	dream::drawPart(OBJ_DREAMh);

	glPushMatrix();
	glRotatef(wang, 0,1,0);
	dream::drawPart(OBJ_DREAMll);
	glRotatef(-2*wang, 0,1,0);
	dream::drawPart(OBJ_DREAMlr);
	glPopMatrix();

	dream::drawPart(OBJ_DREAMe);

	glPushMatrix();
	glRotatef(wang, 0,1,0);
	dream::drawPart(OBJ_DREAMtl);
	glRotatef(-2*wang, 0,1,0);
	dream::drawPart(OBJ_DREAMtr);
	glPopMatrix();
	if ( Last_Actions[FIRE] == TRUE )
		vDrawFires(TRUE);
	else
		vDrawFires(FALSE);
	glPopMatrix();
}
