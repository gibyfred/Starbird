
/*=====================================

 file: events.c

 Hui Tin Yuen
 92057645
 cs_fredh@ug.cs.ust.hk

 comp341 project

=====================================*/

#include <stdio.h>
#include <gl/glut.h>
#include <fmclient.h>
#include <math.h>
#include <string.h>
#include "defines.h"
#include "objmk.h"
#include "bound.h"
#include "init.h"
#include "trackball.h"





/*|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
void vDrawLight(int i, int j, float x, float y, float z)
void setup_lig()
void setup_lig2()
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/


/*----------------------------------------------------------------------
vDrawLights()
void vPrtMesg()
----------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------
----------------------------------------------------------------------*/
void vDrawHelp()
{
 
  	float x = 0.0, y = 0.0, z = -10.0;
	float r_x = 0.0, r_y = 0.0, r_z = -10.0;
	float xang = 0.0, yang = 180.0, zang = 0.0;
	float wang = 0.0, bgang = 0.0;
	float lx = 0.0, ly = CHAN_Y/2.0 + 0.5, lz = -12.0;
	int st_time, titxang = 0;
   Matrix mat;
   long prev_mode;
   char str[30];
   fmfonthandle font1, font2;
	int len = strlen(Message);
	static i = 0;
	int j;
	float tmp;
	const int instr_num = 21;
	char mesg[21][100] =
	{
		"         View            Key",
		" ",
		  "        pilot           pad 7",
		  "        behind          pad 8",
		  "        behind-up       pad 4",
		  "        top             pad 5",
		" ",
		" ",
		"  Usage                               Key",
		" ",
      "                1st setting         2nd setting         3rd setting",
		" ",
		"downward        up arrow            down arrow          down arrow",
    	"upward          down arrow          up arrow            up arrow",
    	"leftward        left arrow          left arrow          left arrow",
    	"rightward       right arrow         right arrow         right arrow",
    	"left turn       pad number 2        pad number 0        akey",
    	"right turn      pad number 3        pad number period   skey",
    	"nitro           pad number 0        pad number 3        zkey",
    	"break           pad number period   pad number 2        xkey",
    	"fire            pad enter           pad enter           dkey",
	};


	#ifdef DOFF
		printf("draw help\n");
	#endif
    glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    

	glPushMatrix();

	gluLookAt(0.3, 1.0, -8.0, r_x, r_y, r_z, 0);
	setup_lig();
	vDrawLight(GL_LIGHT0, GL_LIGHT0, lx, ly, lz);
	vDrawBody(-8.2, 0.0, 0.0, 0.0, x,y,z, 0.0, 180.0, 0.0, 0.0);

	glPushMatrix();
	tmp = Speed;
	Speed = 0.0;
   	vDrawChannel(FALSE);
   	vDrawSqs();
	Speed = tmp;
	glPopMatrix();

	glPopMatrix();


    glPushMatrix();
   glLoadIdentity();
   prev_mode = glGetIntegerv(GL_MATRIX_MODE);
   glMatrixMode(GL_PROJECTION);
   glGetIntegerf(GL_PROJECTION_MATRIX, mat);;

    	glColor3fv(fBlueVec);
   		ortho2(-160, 160, -130, 130);
 
		for (j=0; j<instr_num; j++)
		{
   			cmov2(-150, 100-j*10);
   			charstr(mesg[j]);
		}
 
   glLoadMatrixf(mat);
   glMatrixMode(prev_mode);
   glPopMatrix();


    glutSwapBuffers();
    glFlush();
}





/*----------------------------------------------------------------------
void vDrawWall(float x, float y, float z)
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
void vDrawChannel(int lig_on)
Sq
Sqs
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
vDrawSparks(float pt[3], int time)
void vDrawStick(float x, float y, float z, float ang)
void vDrawSticks2()
void vDrawSticks()
----------------------------------------------------------------------*/


/*----------------------------------------------------------------------
int vDrawRing(float x, float y, float z)
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
void vDrawChkPts()
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
void vDrawRect(float x, float y, float z)
void vDrawRects()
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
void vDrawChan(float dis)
void vDrawChans2()
void vDrawChans()
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
void vDrawCorner(float x, float z, int ang)
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
void vDrawBg()
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
 Purpose   : Draws the entire scene.***********************************
 void vDrawScene() 
 vDrawOverScene()
----------------------------------------------------------------------*/

/*|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/

/***********************************************************************
 set the properties of the picked/unpicked object
 Object vSetObj(int pick)
***********************************************************************/

/***********************************************************************
 rotate element under( considering its ) constraint
 void map_glRotatef(int ele, float *angle)
***********************************************************************/

/***********************************************************************
 rotating leg
 float vObjRotation(Object obj, int ele, int picked)
***********************************************************************/

/***********************************************************************
float vHeadRotation(Object obj, int picked)
***********************************************************************/

/***********************************************************************
float vHeadRotation2(Object obj, int picked)
***********************************************************************/

/***********************************************************************
 rotating leg
 void rot_leg(int ele, float *th_angle, float *ca_angle, int picked)
***********************************************************************/

/***********************************************************************
 draw puppet according to the picked elements
 void vDrawPuppet()
***********************************************************************/


/***********************************************************************
void vEventLoop(void (*scene)())
***********************************************************************/

/*---------------------------------------------------------------------
 ---------------------------------------------------------------------*/


/***********************************************************************
void vDrawFire(float x, float y, float z)
***********************************************************************/

/*---------------------------------------------------------------------
void vDrawDrSha(float x, float y, float z, float xang, float yang, float zang, float wang, float wang2)
 ---------------------------------------------------------------------*/

 
/*---------------------------------------------------------------------
void vDrawDREAM()
 ---------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
void vDrawBody(float v_z, float r_x, float r_y, float r_z, float x, float y, float z, float xang, float yang, float zang, float wang)
---------------------------------------------------------------------------*/


/*
#############################################################################
#############################################################################
*/

/*
 */

/*---------------------------------------------------------------------------
---------------------------------------------------------------------------*/
void vDrawHelpScene()
{
		winset(Win_ids[1]);
		vDrawHelp();
		winset(Win_ids[0]);
}

/*---------------------------------------------------------------------------
---------------------------------------------------------------------------*/
void settings(int on)
{
	int set1[DR_NUM_BUT] =
    {
        UPARROWKEY, DOWNARROWKEY, LEFTARROWKEY, RIGHTARROWKEY,
        PAD2, PAD3, PAD0, PADPERIOD, PADENTER, PAD7, PAD8,
        PAD4, PAD5
    };
	int set2[DR_NUM_BUT] =
    {
       	DOWNARROWKEY, UPARROWKEY, LEFTARROWKEY, RIGHTARROWKEY,
       	PAD0, PADPERIOD, PAD2, PAD3, PADENTER, PAD7, PAD8, 
		PAD4, PAD5
   	};
	int set3[DR_NUM_BUT] =
    {
       	DOWNARROWKEY, UPARROWKEY, LEFTARROWKEY, RIGHTARROWKEY,
       	AKEY, SKEY, ZKEY, XKEY, DKEY, PAD7, PAD8, 
		PAD4, PAD5
   	};
	short last_actions[DR_NUM_BUT] =
    {
        FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
        FALSE, FALSE, FALSE
    };

	int i, size = DR_NUM_BUT;

	if ( on == 1 )
	{
		for (i=0;i<size;i++)
			Devs[i] = set1[i];
	}
	else if ( on == 2 )
	{
		for (i=0;i<size;i++)
			Devs[i] = set2[i];
	}
	else if ( on == 3 )
	{
		for (i=0;i<size;i++)
			Devs[i] = set3[i];
	}
	else if ( on == 4 )
	{
		vOpenWindow("Help Page", 650, 400, 1);
		init_gl();
		vDrawHelpScene();
	}
	for (i=0;i<size;i++)
		Last_Actions[i] = last_actions[i];
}



/*---------------------------------------------------------------------------
void init_menu()
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
void last_mesg()
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
main play loop
---------------------------------------------------------------------------*/
void p1EventLoop() {

   short     sVal;
   long      lDevice;
   Matrix    mNewMat;
   int		 reset = TRUE, i;
	static OKMode = FALSE;

	PMode = TRUE;

    strcpy(Message, "");
    while (TRUE) 
	{

		if (qtest()) 
		{
    		lDevice = qread(&sVal);

        	switch (lDevice) 
			{

			case INPUTCHANGE:
				#ifdef DOFF
					printf(" %d, %d, %d\n", Win_ids[0], Win_ids[1], sVal);
				#endif
				if ( sVal != 0 )
					Input_win = sVal;
				else
					Input_win = -1;
				break;

            case REDRAW:
				if ( sVal == Win_ids[1] && Win_ids[1] != -1 )
					vDrawHelpScene();
				else if ( sVal == Win_ids[0] )
				{
					winset(Win_ids[0]);
               		reshapeviewport();
               		getsize(&nWinWidth, &nWinHeight);
               		getorigin(&nXWinOrigin, &nYWinOrigin);
/*
               		vDrawScene();
*/
				}
               break;

				case ESCKEY:
					if ( OKMode != TRUE)
						return;
					break;

            case QKEY:
				if ( Input_win == Win_ids[1] ) 
				{
					winclose(Input_win);
					Win_ids[1] != -1;
				}
				else if ( Input_win == Win_ids[0] )
				{
					winclose(Win_ids[0]);
					if ( Win_ids[1] != -1 )
						winclose(Win_ids[1]);
	               exit(0);
				}
                	break;
        	}
		}

		getdev(DR_NUM_BUT, Devs, Last_Actions);

		if ( Time == Time/REDRAW_RATE*REDRAW_RATE )
		{
			if ( Energy <= 0 )
			{
				EMode = TRUE;
				return;
			}
			else if ( Time >= END_TIME )
			{
				OKMode = TRUE;
				last_mesg();
			}
			else
				vDrawScene();
		}

		for ( i=0; i<DR_NUM_BUT; i++)
		{
			Last_Actions[i] = FALSE;
		}
		Time++;

	}
}


/*--
read_text
void vDrawText()
void vDrawTitle(int menu)
--*/


/*--
void make_all()
--*/

/*
 half OK w95
 */
void subEventLoop() 
{
   short     sVal, sXCurrent, sYCurrent;
   long      lDevice;
	float tmp;

   nCurrentDir = DIR_NONE;
   czclear(0x404040, getgdesc(GD_ZMAX));

   strcpy(Message, ReadyMesg);
   vDrawScene();
   qreset();
   while (TRUE) {

      if (qtest()) {
         lDevice = qread(&sVal);

         switch (lDevice)
			{

			case INPUTCHANGE:
				#ifdef DOFF
					printf(" %d, %d, %d\n", Win_ids[0], Win_ids[1], sVal);
				#endif
				if ( sVal != 0 )
					Input_win = sVal;
				else
					Input_win = -1;
				break;

            case REDRAW:
				if ( sVal == Win_ids[1] && Win_ids[1] != -1 )
					vDrawHelpScene();
				else if ( sVal == Win_ids[0] )
				{
					winset(Win_ids[0]);
               		reshapeviewport();
               		getsize(&nWinWidth, &nWinHeight);
               		getorigin(&nXWinOrigin, &nYWinOrigin);
					tmp = Speed;
					Speed = 0.0;
               		vDrawScene();
					Speed = tmp;
				}
            break;

         case RIGHTMOUSE:
               dopup(mainmenu);
               break;
  
			case SPACEKEY:
				if ( EMode == FALSE )
				{
					p1EventLoop();
					strcpy(Message, "");
				}
				if ( EMode == TRUE )    /* game over */
				{
					vDrawOverScene();
				}
				break;

			case RKEY:
				if ( Last_Actions[FIRE] == TRUE )
					vEventLoop((&vDrawScene));
				break;

         case QKEY:
				if ( Input_win == Win_ids[1] ) 
				{
					winclose(Input_win);
					Win_ids[1] != -1;
				}
				else if ( Input_win == Win_ids[0] )
				{
					winclose(Win_ids[0]);
					if ( Win_ids[1] != -1 )
						winclose(Win_ids[1]);
	               exit(0);
				}
				break;

             default:
               break;
         }
      }
	  else
		getdev(DR_NUM_BUT, Devs, Last_Actions);

   }
}


/*
OK w95
 */
void mainEventLoop() {

   short     sVal, sXCurrent, sYCurrent;
   long      lDevice;
	int		i=0;
	int 	state = FALSE;
    short attached = 1;
    short value;
    int dev;
    int pupval;

  // make_all();
	//init_menu();

   nCurrentDir = DIR_NONE;

   czclear(0x404040, getgdesc(GD_ZMAX));

   while (TRUE) {

      if (qtest()) {
         lDevice = qread(&sVal);

         switch (lDevice) {

			case INPUTCHANGE:
				#ifdef DOFF
					printf(" %d, %d, %d\n", Win_ids[0], Win_ids[1], sVal);
				#endif
				if ( sVal != 0 )
					Input_win = sVal;
				else
					Input_win = -1;
				break;

            case REDRAW:
				if ( sVal == Win_ids[1] && Win_ids[1] != -1 )
					vDrawHelpScene();
				else if ( sVal == Win_ids[0] )
				{
               		reshapeviewport();
               		getsize(&nWinWidth, &nWinHeight);
               		getorigin(&nXWinOrigin, &nYWinOrigin);
				}
               break;

            case RIGHTMOUSE:
               pupval = dopup(mainmenu);
               break;
  
			case SPACEKEY:
				state = TRUE;
				i++;
				if ( i == 4 )
					subEventLoop();
				break;

			case RKEY:
				if ( Last_Actions[FIRE] == TRUE )
					vEventLoop(&vDrawTitle);
				break;

         case QKEY:
				if ( Input_win == Win_ids[1] ) 
				{
					winclose(Input_win);
					Win_ids[1] != -1;
				}
				else if ( Input_win == Win_ids[0] )
				{
					winclose(Win_ids[0]);
					if ( Win_ids[1] != -1 )
						winclose(Win_ids[1]);
	               exit(0);
				}
               break;

            default:
               break;
         }
      }
		getdev(DR_NUM_BUT, Devs, Last_Actions);

	  	vDrawTitle(state);
   }
}
