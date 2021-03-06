
/*=====================================

 file: DrawPlay.cpp

 Owner: Hui Tin Yuen, Fred

 Description:

 For DrawEngine::drawPlayScene(int state), big objects drawing 
 (like pillar, rings but except background like wall and light) and its helper methods

=====================================*/

//#include <stdio.h>		// scanf
#include <assert.h>
#include <math.h>
#include "DLib/DGL.h"


#include "System.h"
#include "Defines.h"
#include "ObjMake.h"		// fWhiteVec, etc
#include "Bound.h"
#include "Draw.h"


//#define BUILD_START_TURN

//---------------------------------------
// constants

#define OCC1			30	// number of occurances
#define OCC2			40


//---------------------------------------
// local static vars

static int s_isChan2ndHalf=FALSE;



//---------------------------------------
// helpers methods


static void vDrawBg();
static void vDrawPillars1();
static void vDrawPillars2();
static void vDrawChkPts();


static void updateDrBound(float x, float z, float ang);
static void set_view(float vx, float vy, float vz, float rx, float ry, float rz, float xang, float yang, float zang, float wang);


// for *Pillar*()
static float z_1[OCC1];
static float z_2[OCC2];		// z position of pillars
static int active_1 = 0;
static int active_2 = 0;
static int next_1 = 0;
static int next_2 = 0;

// for *ChkPts*()
static int active_chkPts = 0;
static int next_chkPts = 0;
static float z_chkPts[NUM_CHK_PTS];
static float passes_chkPts[NUM_CHK_PTS];

// for turning
static int nextTurnId = 0;

// Dream's status
static float s_x = 0.0, s_y = 0.0, s_z = NORM_Z_POS;
static float s_r_x = 0.0, s_r_y = 0.0, s_r_z = -CHAN_Z;
static float s_v_z = 0.0;
static float s_xang = 0.0, s_yang = 0.0, s_zang = 0.0;
static float s_wang = 0.0;


//
void initInternalStates()
{
	//
	s_isChan2ndHalf = FALSE;

	// pillars states
	active_1 = 0;
	active_2 = 0;
	next_1 = 0;
	next_2 = 0;

	for (int i=0; i<OCC1; ++i )
	{
		z_1[i]=NON;
	}
	for (int i=0; i<OCC2; ++i )
	{
		z_2[i]=NON;
	}

	// ChkPts states
	int active_chkPts = 0;
	int next_chkPts = 0;
	for (int i=0; i<NUM_CHK_PTS; ++i )
	{
		z_chkPts[i]=NON;
		passes_chkPts[i]=FALSE;
	}

	// for turning
	nextTurnId = 0;

	//
	s_x = 0.0, s_y = 0.0, s_z = NORM_Z_POS;
	s_r_x = 0.0, s_r_y = 0.0, s_r_z = -CHAN_Z;
	s_v_z = 0.0;
	s_xang = 0.0, s_yang = 0.0, s_zang = 0.0;
	s_wang = 0.0;
}

//---------------------------------------
// Iris version: p1eventloop;
// Win version: No event loop inside this function.
// @ return false if game can not be continued
//---------------------------------------
bool DrawEngine::drawPlayScene(int state)
{
	enum
	{
		SKIP_INPUT_RATE=2,
	};
	//static int handleInput=0;


	//1016Is_GameCleared = FALSE;

	//2000-1012, we use TimerCallback in windows instead of //if ( Game_Time == Game_Time/REDRAW_RATE*REDRAW_RATE )

	{
		if ( Energy <= 0 )
		{
			Is_GameOver = TRUE;
			return false;
		}
		else if ( Game_Time >= END_TIME )
		{
			Is_GameCleared = TRUE;
			return false;
		}
		else
		{
//			if (!game_paused)
			vDrawScene();

			// w95 ver: in old ver, this clear loop is placed just before "Game_Time++;" below
			//	clearLastActions();
			//handleInput = (handleInput+1)%SKIP_INPUT_RATE;
		}
	}

	return true;
}

//---------------------------------------
// clear buffers, draw scene depending on the state of game and swap buffer
//---------------------------------------
void vDrawScene() 
{
	int oldEnergy = Energy;

	if ( Game_Time + 300 >= END_TIME )
		glClearColor(1,1,1,1);   // sky color
	else
	   glClearColor(0,0,0,1);    // dark tunnel

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	//glLoadMatrixf(mTranslations);		//1005
	//glMultMatrixf(mRotations);			//1005

//	if (false)
////	if (game_paused)
//	{
//		//old:
//		//2013.03xx since we don't have updating and rendering separated,
//		// here we only render those which check
//		// the game_pause flag
//
//		//new: use Speed instead of game_pause
//		float bkupSpeed = Speed;
//		setup_lig();
//		vDrawLights();					// w95ver: drawlight in drawbg
//		vDrawBg();
//    	vDrawPrtMesg();					//
//    	Speed = bkupSpeed;
//	}
//	else
	if ( Game_Time < END_TIME )
	{
		if ( Energy > 0 )
			vDrawDREAM();				/* view trans is set here */

		setup_lig();
		vDrawLights();					// w95ver: drawlight in drawbg
		vDrawBg();
		vDrawPillars1();
		vDrawPillars2();
		vDrawChkPts();
    	vDrawPrtMesg();					//
	}
	else if ( Energy > 0 )
	{
		//game clear
		//TOREFACTOR is this part called??
		assert(0);
#if 0
		vDraw2DBackground();
		vDrawEarths();
		//setup_lig2();
    	vDrawDREAM();					/* view trans is set here */
#endif
	}
	else if ( Energy <= 0 )
	{
		//game over???
    	vDrawDREAM();					/* view trans is set here */
		setup_lig();
		vDrawLights();
	}

	glPopMatrix();

	if ( oldEnergy > Energy )
	{
		glClearColor(.8,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT);		// w95ver: RED screen; fredvec
	}

	//mySysSwapBuffers();
}


//---------------------------------------
//
//---------------------------------------
//disable all lights
void setup_lig()
{
	int j;
	int lig[LIGHT_COUNT];

	for (j=0; j<LIGHT_COUNT; j++)
	{
		lig[j] = j+GL_LIGHT0;
	}

	for( j=0; j<LIGHT_COUNT; j++)
		glDisable(lig[j]);
	
	if ( Light_On == TRUE )
	{
		glEnable(GL_LIGHTING);
    	//lmbind(MATERIAL, OBJ_DREAM);		//obsoleted IRIS GL  =P
	}
	else
	{
		glDisable(GL_LIGHTING);
    	//lmbind(MATERIAL, 0);
	}
}

//enable only lights0
void setup_lig2()
{
	int j;
	int lig[LIGHT_COUNT];

	for (j=0; j<LIGHT_COUNT; j++)
	{
		lig[j] = j+GL_LIGHT0;
	}

	for( j=0; j<LIGHT_COUNT; j++)
		glDisable(lig[j]);
	if ( Light_On == TRUE )
		glEnable(GL_LIGHTING);
	//lmbind(MATERIAL, OBJ_DREAM);
	//lmbind(GL_LIGHT0, GL_LIGHT7);
	glEnable(GL_LIGHT0);
}


//---------------------------------------
//
//---------------------------------------
void vDrawSparks(float pt[3], int time)
{
	static double x[SPARK_NUM], y[SPARK_NUM], z[SPARK_NUM];
	int i;

	int j;
	static int re_time = 0;
		
	for ( i=0; i<1; i++ )
	{
		if ( time != 0 )
		{
			re_time = SPARK_PERIOD;
			for ( j=0; j<SPARK_NUM; j++)
			{
				x[j] = ((float) j / (float) SPARK_NUM - 0.5) * SPARK_CIR_RADIUS + pt[0];
				y[j] = sqrt(-x[j]*x[j] + SPARK_CIR_RADIUS*SPARK_CIR_RADIUS);
			}
			for ( j=0; j<SPARK_NUM; j++)
				z[j] = pt[2];
		}

		if ( re_time != 0 )
		{
			re_time--;

			for (j=0; j<SPARK_NUM; j++)
			{
				#ifdef DE_SPARK
					printf(" j =%d,  (%f, %f, %f)\n", j,x[j],y[j],z[j]);
				#endif
				glPushMatrix();
				glLoadIdentity();
				glTranslatef(x[j],y[j],z[j]);
				glScalef(SPARK_X, SPARK_Y, SPARK_Z);
				vDrawObj(OBJ_SPARK);		//glCallList
				glPopMatrix();
				if ( x[j] >= 0.0 ) 
					x[j] += SPARK_SP;
				else
					x[j] -= SPARK_SP;
					
				if ( y[j] >= 0.0 ) 
					y[j] += SPARK_SP;
				else
					y[j] -= SPARK_SP;
			}
		}
	}
}

//---------------------------------------
// draw one pillar
//---------------------------------------
void vDrawPillar(float x, float y, float z, float ang)
{
	int tmp = 0;
	float colli_pt[3];

	tmp = collide(colli_pt, x, y, z, PILLAR_X, PILLAR_Y, PILLAR_Z, ang);	//w95ver

	glPushMatrix();
	//lmdef(DEFMATERIAL, OBJ_DREAM, 5, stick_mat);
	glColor3fv(fGrayVec);
	glTranslatef(x, y, z);
	if ( tmp < 0 )
		vDrawSparks(colli_pt, Game_Time);
	else
		vDrawSparks(colli_pt, 0);      // being called too many times for a draw !!

	glRotatef(ang, 0,0,1);
	glScalef(PILLAR_X, PILLAR_Y, PILLAR_Z); 
	vDrawObj(OBJ_CUBE1);		//glCallList
	glPopMatrix();
}

//---------------------------------------
// draw pillars
//---------------------------------------
void vDrawPillars2()
{
	int i;

	// a dirty array of time.  1st pillar appears when Game_Time is 140.
	int Live_Stick[OCC2] = 
		{ 140, 143, 146, 149, 152, 155, 158, 161, 164, 167,
		 170, 173, 550, 553, 556, 559, 548, 1400, 1410, 1420,
		 1450, 1470, 1490, 1510, 1530, 1545, 1570, 1630, 1640, 1700,
		 178, 1713, 1716, 1719, 1725, 1730, 1733, 1736, 1739, 1739 };

	// x position of pillars
	float Stick_Xpos[OCC2] = 
		{ -2.0, -2.0, -2.0, -2.0, 2.0, 2.0, 2.0, 2.0, 0.0, 0.0,
		  -0.0, -0.0, -3.0, -2.0, -1.0, -0.0, 1.0, 2.0, -2.0, 1.0,
		  0.0, -1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, 0.0, 0.0,
		  1.8, 1.0, 0.0, -1.0, -2.0, 2.0, 1.0, 0.0, -1.0, -1.0};
	
	if ( next_2 < OCC2 && Live_Stick[next_2] <= Game_Time )
	{
		z_2[next_2] = INI_POS;
		next_2++;
	}

	for ( i=active_2; i<next_2; i++)
	{
		if ( z_2[i] < NON )
		{
			vDrawPillar(0.0, Stick_Xpos[i], z_2[i], 90.0);
			z_2[i] += Speed;
		}
		else if ( active_2 == i && active_2 < next_2 )
			{active_2++;}
	}
}

//---------------------------------------
// draw pillars
//---------------------------------------
void vDrawPillars1()
{
	int i;
	int Live_Stick[OCC1] = 
		{ 90, 93, 96, 99, 102, 105, 108, 112, 115, 118,
		 121, 124, 192, 192, 198, 207, 207, 1504, 1527, 1540,
		 1745, 1750, 1754, 1758, 1763, 1763, 1769, 1769, 1780, 1767};
	float Stick_Xpos[OCC1] = 
		{ -2.0, -2.0, -2.0, -2.0, 2.0, 2.0, 2.0, 2.0, 0.0, 0.0,
		 -0.0, -0.0, -2.5, 2.5, 0.0, 2.5, -2.5, 0.0, 0.0, 1.0,
		-2.3, 2.5, -2.0, 1.8, 0.0, -1.0, 1.0, -2.0, 1.8, 0.0};
	float ang[OCC1] = 
		{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 45.0, 45.0, 45.0,
		 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

	if ( next_1 < OCC1 && Live_Stick[next_1] <= Game_Time )
	{
		z_1[next_1] = INI_POS;
		next_1++;
	}
	
	for ( i=active_1; i<next_1; i++)
	{
		if ( z_1[i] < NON )
		{
			vDrawPillar(Stick_Xpos[i], 0.0, z_1[i], ang[i]);
			z_1[i] += Speed;
		}
		else if ( active_1 == i && active_1 < next_1 )
			{active_1++;}
	}
}

//---------------------------------------
//
//---------------------------------------
int vDrawRing(float x, float y, float z)
{
	int tmp;

    tmp = chk_through(x, y, z, RING_RADIUS,RING_RADIUS,RING_Z);

    glPushMatrix();
        glTranslatef(x, 0.0, z);
        glScalef(RING_SCALE, RING_SCALE, RING_Z);
        vDrawObj(OBJ_RING);	//glCallList
    glPopMatrix();

    if ( tmp < 0 )
        return TRUE;
	else 
		return FALSE;
}

//---------------------------------------
//
//---------------------------------------
void vDrawChkPts()
{
	int Live_Stick[NUM_CHK_PTS] =
        { 350, 358, 366, 388, 398, 450, 484, 1480, 1550, 1570,
		 1590, 1650, 1670, 1690, 1710, 1730, 1740, 1750, 1760, 1770}; 
    float Stick_Xpos[NUM_CHK_PTS] =
        { 1.0, 2.0, -1.0, 0.0, 1.0, -1.0, -2.0, -1.0, 0.0, 1.0,
         1.0, 2.0, -1.0, 0.0, 2.0, -1.0, -2.0, -1.0, 0.0, 1.0};
    float Stick_Ypos[NUM_CHK_PTS] =
        { 1.0, 2.0, -1.0, 0.0, 1.0, -1.0, -2.0, -1.0, 0.0, 1.0,
         1.0, 2.0, -1.0, 0.0, 2.5, -1.0, -2.0, -1.0, 0.0, 1.0};

    if ( next_chkPts < NUM_CHK_PTS && Live_Stick[next_chkPts] <= Game_Time )
    {
        z_chkPts[next_chkPts] = INI_POS;
        next_chkPts++;
    }

    for ( int i=active_chkPts; i<next_chkPts; i++)
    {
        if ( z_chkPts[i] < NON )
        {
			// main draw here
			int pass = vDrawRing(Stick_Xpos[i], Stick_Ypos[i], z_chkPts[i]);
			if ( passes_chkPts[i] == FALSE && pass == TRUE )
			{
				passes_chkPts[i] = TRUE;
				Passes++;
			}
			z_chkPts[i] += Speed;
        }
        else if ( active_chkPts < next_chkPts )
            active_chkPts++;
    }

}

//---------------------------------------
//
//---------------------------------------
void vDrawChan(float dis)
{
	glPushMatrix();
	glTranslatef(CHAN_X*0.5, 0.0, 0.0);
	glRotatef(180, 0,1,0);
	glScalef(0.0, VPLANE_Y, VPLANE_Z);
	vDrawObj(OBJ_VPLANE);		//glCallList
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-CHAN_X*0.5, 0.0, -dis);
	glScalef(0.0, VPLANE_Y, VPLANE_Z);
	vDrawObj(OBJ_VPLANE);		// glCallList
	glPopMatrix();
}

void vDrawChans2(int ang)		/* assume ang > 0 */
{
	float b;

	b = CHAN_X * tan(fabs(deg2r((float)ang/2.0)));

/*
printf("Draw2:  obxz = %f %f   yang = %f\n",Obx[Act_Turn], Obz[Act_Turn],Ob_Yang );
*/
	glPushMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.0, VPLANE_Z * 0.5 );
	vDrawRects();
	vDrawSqs();
	glPopMatrix();

	vDrawChan(-b);  /* 2nd chan */

	glTranslatef(CHAN_X*0.5, 0.0, VPLANE_Z * 0.5);
	glRotatef(-ang, 0,1,0);
	glTranslatef(-CHAN_X*0.5, 0.0, 0.0);

	glTranslatef( 0.0, 0.0, VPLANE_Z * 0.5 );

	glPushMatrix();
	glTranslatef(0.0, 0.0, VPLANE_Z * 0.5 );
	vDrawRects();
	vDrawSqs();
	glPopMatrix();

	vDrawChan(b);  /* 1st chan */

	glPopMatrix();
}

void vDrawChans(int ang)		/* assume ang > 0 */
{
	float b;

	b = CHAN_X * tan(fabs(deg2r((float)ang/2.0)));

/*
printf("DrawC1:  obxz = %f %f   yang = %f\n",Obx[Act_Turn], Obz[Act_Turn],Ob_Yang );
*/
	glPushMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.0, VPLANE_Z * 0.5 );
	vDrawRects();
	vDrawSqs();
	glPopMatrix();
	vDrawChan(b);  /* 1st chan */

	glTranslatef(CHAN_X*0.5, 0.0, VPLANE_Z * -0.5);
	glRotatef(ang, 0,1,0);
	glTranslatef(-CHAN_X*0.5, 0.0, 0.0);

	glTranslatef(0.0, 0.0, VPLANE_Z * -0.5 );

	glPushMatrix();
	glTranslatef(0.0, 0.0, VPLANE_Z * 0.5 );
	vDrawRects();
	vDrawSqs();
	glPopMatrix();
	vDrawChan(-b);  /* 2nd chan */

	glPopMatrix();
}

//---------------------------------------
//
//---------------------------------------
void vDrawCorner(float x, float z, int ang)
{
	//int i;

	#ifdef DCHAN
		printf(" Dcorner: z = %f, Ob_Yang = %f AT = %d, Ch2= %d\n", z, Ob_Yang, Act_Turn, s_isChan2ndHalf); 
	#endif

	updateDrBound(x, z, ang);

	glPushMatrix();
	glRotatef(Ob_Yang, 0,1,0);                 /**** change base for obstacles ****/
	glTranslatef(x, 0.0, z);
	if ( s_isChan2ndHalf == FALSE )
		vDrawChans(ang);
	else
		vDrawChans2(ang);
	glPopMatrix();

}

//---------------------------------------
// tunnel, squares and rects;  If turning corner, draw corner
//---------------------------------------
void vDrawBg()
{
	int Live_Con[TURN_NUM] = 	{ 900, 2200 };
	int Ang[TURN_NUM] =			{ -90, -90 };

#ifdef BUILD_START_TURN
	if ( nextTurnId < TURN_NUM && Live_Con[nextTurnId] <= Game_Time )
	{
		// start a new corner
		Turn_Mode = TRUE;
		Ob_Yang = 0.0;
		Obz[nextTurnId] = -VPLANE_Z;
		Obx[nextTurnId] = 0.0;
		ch_lplane = FALSE; ch_rplane = FALSE;
		s_isChan2ndHalf = FALSE;
		Act_Turn = nextTurnId;
		vDrawCorner(Obx[nextTurnId], Obz[nextTurnId], Ang[nextTurnId]);		// main call
		nextTurnId++;
	}
	else
#endif
	{
		if ( Turn_Mode == FALSE )
		{
			// draw tunnel
			update_walls(CHAN_X*0.5, -CHAN_X*0.5, CHAN_Y*0.5, -CHAN_Y*0.5);
   			vDrawTunnel(FALSE);		// TRUE: w95 ver!?
			vDrawSqs();
			vDrawRects();
		}
		else if ( Game_Time < END_TIME )
		{
			// draw corner
			Obz[Act_Turn] += Speed * cos(fabs(deg2r(Ob_Yang)));
			Obx[Act_Turn] -= sign(Ob_Yang) * Speed * sin(fabs(deg2r(Ob_Yang)));
			vDrawCorner(Obx[Act_Turn], Obz[Act_Turn], Ang[Act_Turn]);		// main call
		}
	}

	if ( s_isChan2ndHalf == TRUE && Obz[Act_Turn] >= VPLANE_Z/2.0 )
	{
		#ifdef DTEST
			printf(" *** end turn obz = %f Act_Turn = %d\n", Obz[Act_Turn], Act_Turn);
		#endif
		Turn_Mode = FALSE;
		s_isChan2ndHalf = FALSE;
		Ob_Yang = 0.0;
		ch_lplane = FALSE; ch_rplane = FALSE;
	}

	//	Turn_Mode = FALSE;
}


//---------------------------------------
//
//---------------------------------------
void vDrawDREAM()
{
	#ifdef DE_DREAM
		printf("   DrawDr: %f*%f  %f %f %d \n", r_x, r_y , x, y);
	#endif

		reset_view_co(&s_r_y);
		reset_view_co(&s_r_x);
		reset_turn(&s_xang, 'x');
		reset_turn(&s_yang, 'y');
		reset_turn(&s_zang, 'z');
		reset_turn(&s_wang, 'w');
		
		if ( Last_Actions[UP] == TRUE )
		{
			s_y -= SENSI;
			s_r_y -= VSENSI;
			s_zang += TURN_SENSI*0.5;
			chk_tcons(&s_zang, 'z');
			chk_vcons(&s_y, &s_r_y, MOV_Y);
		}
		if ( Last_Actions[DOWN] == TRUE )
		{
			s_y += SENSI;
			s_r_y += VSENSI;
			s_zang -= TURN_SENSI*0.5;
			chk_tcons(&s_zang, 'z');
			chk_vcons(&s_y, &s_r_y, MOV_Y);
		}
		if ( Last_Actions[LEFT] == TRUE )
		{
			if ( Turn_Mode == TRUE )
				Ob_Yang -= TURNM_SENSI;
			else
				s_x -= SENSI;
			s_r_x -= VSENSI;
			s_yang += TURN_SENSI*0.5;
			chk_tcons(&s_yang, 'y');
		}
		if ( Last_Actions[RIGHT] == TRUE )
		{
			if ( Turn_Mode == TRUE )
				Ob_Yang += TURNM_SENSI;
			else
				s_x += SENSI;
			s_r_x += VSENSI;
			s_yang -= TURN_SENSI*0.5;
			chk_tcons(&s_yang, 'y');
		}

		// check bounding case
		if ( Last_Actions[LEFT] == TRUE ||  Last_Actions[RIGHT] == TRUE 
			|| Turn_Mode == TRUE )
			chk_vcons(&s_x, &s_r_x, MOV_X);

		// shaking
		if ( Last_Actions[LTURN] == TRUE  && Shake == FALSE )
		{
			s_xang += TURN_SENSI;
			chk_tcons(&s_xang, 'x');
		}
		else if ( Last_Actions[RTURN] == TRUE && Shake == FALSE )
		{
			s_xang -= TURN_SENSI;
			chk_tcons(&s_xang, 'x');
//			dbg_msg("drawDREAM(): xang:%.4f  \n", s_xang);
		}

		// speed up/down
		if ( Last_Actions[SPUP] == TRUE )
		{
			if ( Sp_Eng == FULL_SPENG )
			{
				set_sp(MAX_SPEED);
			}
		}
		if ( Last_Actions[SPDOWN] == TRUE )
		{
			if ( Sp_Eng == FULL_SPENG )
			{
				set_sp(MIN_SPEED);
			}
		}

		// the wing angle depends on the speed
		if ( set_sp(-1.0) > 0.0 )
		{
			s_wang += TURN_WSENSI;
		}
		else if ( set_sp(-1.0) < 0.0 )
		{
			s_wang -= TURN_WSENSI;
		}
		chk_tcons(&s_wang, 'w');

	shake(&s_xang);


	#ifdef DE_DREAM
		printf("   DrawDr: %f %f  %f %f \n", r_x, r_y, x, y);
	#endif

	// internal update
	update_dream(&s_x, &s_y, &s_z, &s_xang);		//TODOF3 speedup
	Dream_State.update(&s_x, &s_y, &s_z, &s_xang);

	// prepare OGL view
	set_view(s_x, s_y, s_z, s_r_x, s_r_y, s_r_z, s_xang, s_yang, s_zang, s_wang);

	// draw it
	vDrawBody(s_v_z,
			s_r_x, s_r_y, s_r_z,
			s_x, s_y, s_z, s_xang, s_yang, s_zang, s_wang);

	Eval_Angle = s_zang;		// w95 ver //old = zang
}


//---------------------------------------
//
//---------------------------------------
float ang2y(int w)
{
	return cos( (float) fabs((float)w));
}

float ang2x(int w)
{
	return sin( (float) (w));
}


void set_view(float vx, float vy, float vz, float rx, float ry, float rz, float xang, float yang, float zang, float wang)
{
	static int oldv = -1;
	static float a,b,c,d,e,f;
	static int w;
	int view = VIEW4;

	if (Last_Actions[VIEW4] == TRUE) 
		view = VIEW4;
	else if (Last_Actions[VIEW1] == TRUE )
		view = VIEW1;
	else if (Last_Actions[VIEW3] == TRUE )
		view = VIEW3;
	else
	{
		if ( Last_Actions[VIEW2] == TRUE )
			view = VIEW2;
		else 			/* no view button pushed */
			view = oldv;
	}
	if ( Game_Time >= END_TIME )
		view = VIEW2;

	#ifndef VIEW4
		printf("view: %d %d, %f %f %f %f %f %f\n", view, oldv, vx, vy, vz, rx, ry, rz);
	#endif

	if ( oldv == -1 )		// if this func is called first time
	{
		a=0.0;  b=0.0;  c=0.0;    
		d=rx;  e=ry;  f=rz;    w=xang;		// w=xang*10
		oldv = VIEW2;
		if ( view == VIEW1 )
			//gluLookAt(a, b, c, d, e, f,w);
			gluLookAt(a, b, c, d, e, f, ang2x(w), ang2y(w),0);	//BUGHERE
		else
			//gluLookAt(a, b, c, d, e, f,0.0);
			gluLookAt(a, b, c, d, e, f, 0,1,0);
		return;
	}
	else if ( view == oldv )
	{
		if ( view == VIEW2 )
		{
			a=0.0;  b=0.0;  c=0.0;
			d=rx;  e=ry;  f=rz;    w=0;
		}
		else if ( view == VIEW1 )
		{
			a=vx;  b=vy;  c=vz+0.5;    
			f=-5;    w=10*xang;
			d= sign(yang)*f*tan(fabs(deg2r(yang)));
			e= sign(zang)*f*tan(fabs(deg2r(zang)));
		}
		else if ( view == VIEW3 )
		{
			a=0.0;  b=3.0; c=3.36;
			d=rx;  e=ry;  f=rz, w=0;    
		}
		//gluLookAt(a, b, c, d, e, f, w);
		gluLookAt(a, b, c, d, e, f, ang2x(w),ang2y(w),0);
		return;
	}

	#ifndef VIEW4
		printf("chview: %d %d, %f %f %f %f %f %f\n", view, oldv, vx, vy, vz, rx, ry, rz);
	#endif

	//------------------ view changed by player
//DEBUGONLY
//view = VIEW4;

	switch (view)
	{
		case VIEW2:
			a=0.0;  b=0.0;  c=vz;    
			d=rx;  e=ry;  f=rz;    w=0;
			break;

		case VIEW1:
			a=vx;  b=vy;  c=vz+0.5;    
			f=-5;    w=10*xang;
			d= sign(yang)*f*tan(fabs(deg2r(yang)));
			e= sign(zang)*f*tan(fabs(deg2r(zang)));
			break;

		case VIEW3:
			a=0.0;  b=3.0; c=3.34;    
			d=rx;  e=ry;  f=rz;    w=0;
			break;

		case VIEW4:  // fixed view
			a=0.;  b=9.0;  c=-5.0;    
			d=0.;  e=0.;  f=-10.0;    w=0;
			break;
	}
	//gluLookAt(a,b,c,d,e,f,w);
	gluLookAt(a,b,c,d,e,f, ang2x(w),ang2y(w),0);
	oldv = view;
}



//---------------------------------------
// for DrawCorner
//---------------------------------------
void updateDrBound(float x, float z, float ang)
{
	float coang = cos(fabs(deg2r(Ob_Yang)));
	float d, b, B=10.0, A=10.0, c, t;
	float x2;	//, x1;

	if ( s_isChan2ndHalf == FALSE )
	{
		t = tan(fabs(deg2r(Ob_Yang)));
		if ( ch_lplane == FALSE )
		{
			c = (CHAN_X * 0.5 - x) * t;
			b = CHAN_X * tan(fabs(deg2r((float)ang/2.0)));
			B = VPLANE_Z * 0.5 + b - c - z;
		}
		if ( ch_rplane == FALSE )
		{
			d = (CHAN_X * 0.5 + x) * t;
			A = VPLANE_Z*0.5 - z + d;
		}
	
		if ( A <= 0 || Obz[Act_Turn] >= VPLANE_Z*0.5 )
		{
			#ifdef DCORNER
				printf(" A = %f  obx = %f  obz = %f\n",A, Obx[Act_Turn], Obz[Act_Turn]);
			#endif
			ch_lplane = TRUE;
		}
		if ( B <= 0 || Obz[Act_Turn] >= VPLANE_Z*0.5 )
		{
			#ifdef DCORNER
				printf(" B = %f  obx = %f  obz = %f\n",B, Obx[Act_Turn], Obz[Act_Turn]);
			#endif
			ch_rplane = TRUE;
		}
		if ( (ch_rplane == TRUE && ch_rplane == TRUE ) && Obz[Act_Turn] > 0.0 )
		{
			Ob_Yang = -sign(Ob_Yang) * (90.0 - Ob_Yang);    /* change base */

			d = Obz[Act_Turn];
			Obz[Act_Turn] = - ((VPLANE_Z + CHAN_X)/2.0 + Obx[Act_Turn]);
			Obx[Act_Turn] = -sign(Obx[Act_Turn]) *(d - (VPLANE_Z + CHAN_X)/2.0);
			s_isChan2ndHalf = TRUE;
			coang = cos(fabs(deg2r(Ob_Yang)));
			x = Obx[Act_Turn];
			z = Obz[Act_Turn];
		}
	}


	x2 = -(CHAN_X/2.0 - x)/coang;
	
	update_walls(CHAN_X/coang + x2, x2, CHAN_Y*0.5, -CHAN_Y*0.5);
}
