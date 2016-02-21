
/*=====================================

 file: defines.h
 
 Owner: Hui Tin Yuen, Fred
 
 Description:

 define all game constants and simple macros


 Hui Tin Yuen
 92057645
 cs_fredh@ug.cs.ust.hk

 comp341 project

=====================================*/


#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif



/*================================ our boss... PUPPET ================================*/

/* scale of the elements of puppet <CHANGEABLE> */
#define ROOT_X		0.0
#define ROOT_Y		0.0
#define ROOT_Z		0.0
#define TORSO_X	4.0		
#define TORSO_Y	4.0		
#define TORSO_Z	2.0		
#define SHOUL_X	5.0		
#define SHOUL_Y	1.0		
#define SHOUL_Z	3.0		
#define UP_ARM_X	2.0
#define UP_ARM_Y	3.0
#define UP_ARM_Z	2.0
#define LO_ARM_X	1.0
#define LO_ARM_Y	2.0
#define LO_ARM_Z	1.0
#define HAND_X		1.0
#define HAND_Y		0.5
#define HAND_Z		1.3
#define NECK_X		1.0
#define NECK_Y		1.0
#define NECK_Z		1.0
#define HEAD_X		1.8
#define HEAD_Y		2.0
#define HEAD_Z		2.0
#define HIP_X		1.5
#define HIP_Y		1.5
#define HIP_Z		1.5
#define THIGH_X	2.0
#define THIGH_Y	4.0
#define THIGH_Z	2.0
#define CALF_X		1.0
#define CALF_Y		3.0
#define CALF_Z		1.0
#define FOOT_X		1.0
#define FOOT_Y		1.0	
#define FOOT_Z		2.0

#define ELE_NUM	17 	/* number of elements */
#define SEL_ELE 5 		/* number of pickable elements */


/* id for the different picked parts and joints (angles) */
#define R_ARM   0
#define L_ARM   1
#define R_LEG   2
#define L_LEG   3
#define HEADN   4

/* unpickable part */
#define BODY	5

#define UP_ANGLE	1
#define LO_ANGLE	3
#define TH_ANGLE	5
#define CA_ANGLE	7
#define HEADX_ANGLE 9
#define HEADY_ANGLE	10
#define NECK_ANGLE	11

/* constrains for the joints <CHANGABLE> */
#define UP_ARM_CONS_MAX		80
#define UP_ARM_CONS_MIN		-160
#define LO_ARM_CONS_MAX		0.5
#define LO_ARM_CONS_MIN		-130

#define THIGH_CONS_MAX		30
#define THIGH_CONS_MIN		-140
#define CALF_CONS_MAX		150
#define CALF_CONS_MIN		-0.5
#define HEADX_CONS_MAX		30
#define HEADX_CONS_MIN		-20
#define HEADY_CONS_MAX		30
#define HEADY_CONS_MIN		-30
#define NECK_CONS_MAX		20
#define NECK_CONS_MIN		-20

/* respond rate of the puppet <CHANGEABLE> */
#define SENS	0.6          /* sensitivity of (rotation by) mouse */
#define SAMPLE_RATE		10  /* sample rate of the mouse */



/*================================ OS/SYSTEM ================================*/

/* others <CHANGEABLE> */
#ifdef SGI
#define WIN_WIDTH  320
#define WIN_HEIGHT 240
#elif 1
#define WIN_WIDTH  640
#define WIN_HEIGHT 480
#else
#define WIN_WIDTH  640
#define WIN_HEIGHT 480
#endif

/* turn on back face culling functionality <DELETEABLE> */
#define BACK_FACE



/*================================ aircraft DREAM ================================*/

/* scales of objects */
#define CHAN_X		8.0
#define CHAN_Y		6.0
#define CHAN_Z		180.0

#define RECT_LEN		2.0
#define SQ_DIST		RECT_LEN * 4.5

#define WALL_Z  		1.0				//in IRIS version, it is around 5
#define OFFSET_FROM_WALL 0.01

#define PILLAR_X		1.0
#define PILLAR_Y		8.0
#define PILLAR_Z		1.0

#define DR_X	1.2
#define DR_Y	0.4
#define DR_Z	1.0
#define DR_SCALE	0.1f

#define RING_Z  	1.0
#define RING_X		5.0
#define RING_SCALE  0.3
#define RING_RADIUS	(RING_SCALE*RING_X)

#define SPARK_CIR_RADIUS	0.2
#define SPARK_X		0.25
#define SPARK_Y		0.25
#define SPARK_Z		0.25
#define SPARK_NUM		16
#define SPARK_PERIOD	20
#define SPARK_SP		0.5

#define VPLANE_Y		CHAN_Y        /* vertical plane */
#define VPLANE_Z		(CHAN_Z/2.0)


/* DREAM's UI's settings, sensitivity (for animation speed) */
#define NORM_Z_POS  -3.0

#define SENSI		0.2
#define VSENSI		8.0

#ifdef SB_WIN
#define TURN_SENSI	20.0
#else
#define TURN_SENSI	40.0
#endif

#define TURN_WSENSI 3.0
#define TURNM_SENSI 3.0

#define SP_SENSI	0.2

#define MAX_SPEED	1.6	/* 0.9 (indi2)  /* 3.0 */
#define NOR_SPEED	1.0	/* 0.6 (indi2) */
#define MIN_SPEED	0.4


/* view reference point constraints */
#define REF_X_CONS	90.0
#define REF_Y_CONS	70.0
#define MOV_X_CONS	(CHAN_X/2.0)
#define MOV_Y_CONS	(CHAN_Y/2.0)

#define TURN_XCONS	90.0
#define TURN_YCONS	30.0
#define TURN_ZCONS	30.0
#define TURN_WCONS	12.0


/* obstacle settings; stick, etc*/
#define NON			(NORM_Z_POS + 0.5)		// out of view z pos
#define INI_POS		-CHAN_Z/2.0


/* id for display list */
#define OBJ_CUBE1		1		//pillar!
#define OBJ_CUBE2		3
#define OBJ_CUBE3		4
#define OBJ_CUBE4		9
//#define OBJ_PLANE		2
#define OBJ_RING		5
#define OBJ_SPARK		6
#define OBJ_RECT		7
#define OBJ_VPLANE		8
#define OBJ_CHANNEL		10
#define OBJ_SQUARE		11
//#define OBJ_DREAM_ALL	12		// new in w95 ver
#define OBJ_DREAM		13
#define OBJ_DREAMh		14
#define OBJ_DREAMtl		15
#define OBJ_DREAMtr		16
#define OBJ_DREAMll		17
#define OBJ_DREAMlr		18
#define OBJ_DRSHA_LL	19
#define OBJ_DRSHA_LR	20
#define OBJ_DRSHA_TL	21
#define OBJ_DRSHA_TR	22
#define OBJ_FIRE		23		// fire bullet
#define OBJ_DREAMe		24
#define OBJ_SMALL_EARTH	25
#define OBJ_DRSHA		26
#define OBJ_RECT_EARTH	27


/* key button id for struct Device_Ids */
#define DR_NUM_BUT	13
#define UP			0
#define DOWN		1
#define LEFT		2
#define RIGHT		3
#define LTURN		4
#define RTURN		5
#define SPUP		6
#define SPDOWN		7
#define FIRE  		8
#define VIEW1		9
#define VIEW2		10
#define VIEW3		11
#define VIEW4		12

#ifdef SB_BUILD_VIRTUAL_PAD
enum
{
	DPAD_X = 160,
	DPAD_Y = 160,

	DPAD_SIZE_IN = 35,
	DPAD_SIZE_OUT = 140,

	// for Galaxy Note
//	DPAD_X = 640,
//	DPAD_Y = 400,
//
//	DPAD_SIZE_IN = 40,
//	DPAD_SIZE_OUT = 400,
};
#endif

#define MOV_X		1
#define MOV_Y		2
#define TURN		3

#define TRUE		1
#define FALSE		0
#define HALF		2

#define START		1
#define CONT		0


/* degree of difficulty */
#define BABY			0.1
#define DIFF_NORMAL		0.6
#define EXPERT			1.2
#define INITI_DIFFI		DIFF_NORMAL		// BABY=1,NORMAL=2,EXPERT=3
#define INIT_ENG_RATE	45.0
#define FULL_ENG		100
#define FULL_SPENG		200
#define SPEED_PERIOD	30    /* remaining max/min speed */


/* lighting */
#define LIG_DIST		30.0
#define LIGHT_COUNT		2		// in the IRIS version, it is 4


/* others */
#define TURN_NUM		2
#define NUM_CHK_PTS		20


//2000 #define REDRAW_RATE		2					//iris version: paint framebuffer per 2 redraw calls
#define REBOUND			1.4
#define SHAKE_ANG		(TURN_SENSI/1.5)
#define SHAKE_PERIOD	20

#define FIRE_NUM		7

#define END_TIME		3000		// win95 ver // old=3500
#define CHAN_Y2			15.0




/* -------------------------
Game state constants
2000 This could be used as index to array of functions of pointers
 if we need in future.
------------------------- */
enum
{
	SCENE_TITLE, SCENE_PLAY, SCENE_END
};


/* game state */
enum
{
	SUB_STATE0,
	SUB_STATE1,
	SUB_STATE2,
	SUB_STATE3,
	SUB_STATE4,
};


//----- -----// MACROs
#define SIZEOFARR(_Arr) (sizeof(_Arr)/sizeof(_Arr[0]))


/* disable warning in VC*/
//#pragma warning(disable : 4244)	// subpress double to float conversion
#pragma warning(disable : 4305)		// subpress const double to float conversion
