
/*=====================================

 file: draw.h

 Owner: Hui Tin Yuen, Fred

 Description:

 All ogl rendering stuff.  A half C/C++ version.

=====================================*/
//#pragma once


//--------------------
// define simple types

typedef float GLMatrix[16];


//-------------------------------------------------------------
//
// wapper class for drawing scene
//
//
//-------------------------------------------------------------
class DrawEngine
{
public:
	bool	isTexture;		//0310 texture on the floor appeared in game clear scene

public:
	void	init();

	void	drawMessage(GLfloat x, GLfloat y, GLfloat z, float zoom, char *message);

	bool	drawTitleScene(int state);
	bool	drawPlayScene(int state);
	bool	drawEndScene(int state);

	void beginDraw2D( const char mode = 0 );
	void endDraw2D();

	void setUpdateSpeed(float spd);

private:
	GLMatrix lastProjMat;
};

extern DrawEngine Draw_Engine;


struct DreamState
{
	void update(float *x, float *y, float *z, float *angle)
	{
		pos[0] = *x;
		pos[1] = *y;
		pos[2] = *z;
	};

	float pos[3];
};

extern DreamState Dream_State;


//======================================================
// extern functions, vDraw* are those functions which draw things
//======================================================

//--------------------
// func in draw.cpp
extern void vDrawScene();
extern void	vDrawLight(int i, int j, float x, float y, float z);
extern void vDrawLights();
extern void vDrawBody(float v_z, float r_x, float r_y, float r_z, float x, float y, float z, float xang, float yang, float zang, float wang);
extern void vDrawRects();
extern void vDrawSqs();
extern void vDrawTunnel(int ligsOn);
extern void vDrawPrtMesg();

//--------------------
// func in DrawInit.cpp
extern void set_diffi(int deg);
extern void set_light(int on);
extern void setMainMessage(const char* s);
extern void clearLastActions();

//--------------------
// func in DrawPlay.cpp
extern void setup_lig();
extern void setup_lig2();
extern void vDrawDREAM();
extern void initInternalStates();

//--------------------
// func in DrawEnd.cpp
extern void vDraw2DBackground();
extern void	vDrawEarths();



//======================================================
// extern variables
//======================================================

//--------------------
// for system.c
extern char Last_Actions[DR_NUM_BUT];
extern int Screen_W;
extern int Screen_H;

extern int Scene_State;		// = SCENE_TITLE;	
extern int Sub_State;		// = SUB_STATE0

extern int s_touchPadMode;		// 0: flick/fling  1:gamePad  2:absolutePosition
extern bool s_isTouchDown;


//--------------------
// for bound.c
extern int Energy;
extern int Passes;
extern int Passes_Count;
extern int Sp_Eng;
extern float Speed;
extern float Speed_Bkup;

extern int Game_Time;
extern int Light_On;
extern float Diffi;

extern int Turn_Mode;
extern float Ob_Yang;
extern float Obz[TURN_NUM];
extern float Obx[TURN_NUM];
extern int Act_Turn;


//--------------------
// for DrawInit.c
extern char Main_Mesg[128];


//--------------------
// for DrawPlay.c
extern char Is_GameOver;
extern char Is_GameCleared;
//extern int Input_win;
extern int ch_lplane, ch_rplane;
extern float Eval_Angle;


//--------------------
// for DrawEnd.c
extern char GameOver_Mesg[];
extern unsigned int T_w, T_h;					// texture size
extern GLubyte Pixel_Arr[1000*1000][3];	// texture data

