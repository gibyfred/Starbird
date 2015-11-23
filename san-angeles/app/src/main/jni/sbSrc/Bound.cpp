
/*=====================================

 file: bound.cpp

 Owner: Hui Tin Yuen, Fred

 Description:

Game logic
- current state of Dream
- includes boundary checking and simulation of collision detection

=====================================*/

#include <math.h>
#include <assert.h>

#include "DLib/DGL.h"

#include "Defines.h"
#include "Draw.h"
#include "Bound.h"


/*----------------------------------------------------------------------
global variables
----------------------------------------------------------------------*/
float Eng_Deduct_Rate = INIT_ENG_RATE;		// controls the difficulty of the game
int Shake = FALSE;							// Dream check if it collides with other objects

static float Drx, Dry, Drz, Drang;			//Dr*: Dream's translation and rotation
static float Boundx, Boundy;
static float Boundx2, Boundy2;
static int live = 0;
static float sh_ang = SHAKE_ANG;


//---------------------------------------
//
//---------------------------------------
void bound_resetInternalStates()
{
	live = 0;
	sh_ang = SHAKE_ANG;
	Shake = FALSE;
}

/*----------------------------------------------------------------------
reset coordination for animation
----------------------------------------------------------------------*/
void reset_view_co(float *v)
{
	// if not saturated
	if ( *v != 0.0 )
	{
		if ( fabs(*v) < 0.009 )
			*v = 0;
		else if ( *v > 0.0 )
			*v -= VSENSI/4.0;
		else
			*v += VSENSI/4.0;
	}
}


/*----------------------------------------------------------------------
reset rotation angle specified by rot (= 'x','y','z') for animation
----------------------------------------------------------------------*/
void reset_turn(float *v, char rot)
{
	float scale;

	// if not saturated
	if ( *v != 0.0 )		//2000-10
	{
		if ( rot == 'x' )
			scale = 0.5* TURN_SENSI;
		else if ( rot == 'y' || rot == 'z' )
			scale = 0.25 * TURN_SENSI;
		else if ( rot == 'w' )
			scale = 0.7 * TURN_WSENSI;

		if ( fabs(*v) < scale )
			*v = 0.0;
		else if ( *v > 0.0 )
			*v -= scale;
		else
			*v += scale;
	}
}

/*----------------------------------------------------------------------
It should be a macro //TODOA7
----------------------------------------------------------------------*/
int sign(float x)
{
	if ( x>=0.0 )
		return 1;
	else
		return -1;
}

/*----------------------------------------------------------------------
modify the rotation angle so that Dream looks like shaking
----------------------------------------------------------------------*/
void shake(float *ang)
{
	if ( Shake == TRUE && live == 0 )
	{
		live = SHAKE_PERIOD;
	}
	if ( live != 0 )
	{
		live--;
		if( live == (SHAKE_PERIOD / 2) )
		{	
			sh_ang = -sh_ang;
		}
		*ang += sh_ang;
	}
	if ( live == 0 )
		Shake = FALSE;
}

/*----------------------------------------------------------------------
deduct energy according to the current game's difficulty
----------------------------------------------------------------------*/
int deduct_energy(float eng)
{
	Energy -= (int) ceil(fabs(eng * Diffi));
	#ifdef DTEST
		printf(" eng -= %f\n", eng * Diffi);
		printf(" walls:  %f %f %f %f   Drx: %f   Ob_Yang: %f\n", Boundx,Boundx2,Boundy,Boundy2,Drx,Ob_Yang);
	#endif

	Shake = TRUE;

	return Energy;
}

/*----------------------------------------------------------------------
helper function for chk_vcons to detect turning around corner
----------------------------------------------------------------------*/
void set_ob_base()
{
	if ( fabs(Ob_Yang) <= 10 )
		Ob_Yang = 0.0;
	else if ( Ob_Yang >= 0.0 )
		Ob_Yang -= TURN_SENSI;
	else
		Ob_Yang += TURN_SENSI;

	Obz[Act_Turn] -= 1.0;
/*
printf("hi, Obx = %f\n", Obx[Act_Turn]);
*/
	Obx[Act_Turn] += sign(Ob_Yang) *Speed*sin(fabs(deg2r(Ob_Yang))) 
						- sign(Obx[Act_Turn]) * 0.5;
}

/*----------------------------------------------------------------------
check vertical or horizontal constrains
----------------------------------------------------------------------*/
void chk_vcons(float *pos, float *dis, int axis)
{
    switch(axis)
    {
        case MOV_Y:
         if ( *pos >= Boundy -0.2 )
			{
				deduct_energy(Eng_Deduct_Rate/4.0);
				*pos = Boundy - REBOUND;
			}
			else if ( *pos <= Boundy2 + 0.2)
			{
				deduct_energy(Eng_Deduct_Rate/4.0);
				*pos = Boundy2 + REBOUND;
			}

            if ( *dis > REF_Y_CONS )
                *dis = REF_Y_CONS;
            else if ( *dis < - REF_Y_CONS )
                *dis = - REF_Y_CONS;
            break;

        case MOV_X:
         if ( *pos >= Boundx - 0.3 )
			{
				deduct_energy(Eng_Deduct_Rate/4.0);
				if ( Turn_Mode == FALSE )
					*pos = Boundx - REBOUND;
				else
					set_ob_base();
			}
         else if ( *pos <= Boundx2 + 0.3 )
			{
				deduct_energy(Eng_Deduct_Rate/4.0);
				if ( Turn_Mode == FALSE )
                	*pos =  Boundx2 + REBOUND;
				else
					set_ob_base();
			}

            if ( *dis > REF_X_CONS )
                *dis = REF_X_CONS;
            else if ( *dis < - REF_X_CONS )
                *dis = - REF_X_CONS;
            break;
    }
}

/*----------------------------------------------------------------------
check turn constrains
----------------------------------------------------------------------*/
void chk_tcons(float *ang, char rot)
{
	float cons;

	if ( rot == 'x')
		cons = TURN_XCONS;
	else if ( rot == 'y' )
		cons = TURN_YCONS;
	else if ( rot == 'z' )
		cons = TURN_ZCONS;
	else if ( rot == 'w' )
		cons = TURN_WCONS;

    if ( *ang > cons )
        *ang = cons;
    else if ( *ang < - cons )
        *ang = - cons;
}

/*----------------------------------------------------------------------
If limit is not -1, set the current speed, otherwise the speed will slowly
be changed to normal speed.
Return a value for controlling the angle between Dream's wings.
----------------------------------------------------------------------*/
float set_sp(float limit)
{
	static float climit = -1.0;
	static int reset = FALSE;
	static int reset_sp = FALSE;
	static int st_time = -1;    // start time

	if ( limit != -1.0 ) /* && Speed == NOR_SPEED */
	{
		st_time = Game_Time;
		climit = limit;
	}

	if ( fabs(climit - MAX_SPEED) <= 0.01 )
	{
		if ( Speed < MAX_SPEED )
		{
			Speed += SP_SENSI*0.6;
			Sp_Eng = 0;
		}
		if ( Speed > MAX_SPEED )
		{
			Speed = MAX_SPEED;
			reset = TRUE;
			climit = -1;
		}
	}
	if ( fabs(climit - MIN_SPEED) <= 0.01 )
	{
		if ( Speed > MIN_SPEED )
		{
			Speed -= SP_SENSI*0.2;
			Sp_Eng = 0;
		}
		if ( Speed < MIN_SPEED )
		{
			Speed = MIN_SPEED;
			reset = TRUE;
			climit = -1;
		}
	}

//
int	debug_rate = 3.0;	// ONLY for debug
	if ( st_time != -1 &&  st_time + SPEED_PERIOD*debug_rate <= Game_Time )
	{
		// reset speed
		reset_sp = TRUE;
		st_time = -1;
	}

	if ( reset == TRUE )
		Sp_Eng += 1;

/*
	printf(" *.   %d,  %d\n", reset, reset_sp);
	printf("  .   %f,  %d\n", Speed, Sp_Eng);
*/

	if ( Sp_Eng >= FULL_SPENG )
	{
		Sp_Eng = FULL_SPENG;
		reset = FALSE;	
	}

	if ( reset_sp == TRUE )
	{
	  if ( fabs(Speed - NOR_SPEED) < 0.1 )
			Speed = NOR_SPEED;
	  else if ( Speed > NOR_SPEED )
			Speed -= SP_SENSI/5.0;
	  else if ( Speed < NOR_SPEED )
			Speed += SP_SENSI/20.0;

	  if ( fabs(Speed - NOR_SPEED) < 0.1 )
			Speed = NOR_SPEED;
	}
	if ( fabs(Speed - NOR_SPEED) < 0.01 )
		reset_sp = FALSE;

	return (Speed - NOR_SPEED);
}

/*----------------------------------------------------------------------
update Dream's position and rotation
----------------------------------------------------------------------*/
void update_dream(float *x, float *y, float *z, float *angle)
{
	Drx = *x;
	Dry = *y;
	Drz = *z;
	Drang = *angle;
}

/*----------------------------------------------------------------------
update Dream's movable boundary
----------------------------------------------------------------------*/
void update_walls(float w, float w2, float h, float h2)
{
	assert( w > w2 && h > h2 );
	Boundx = w;
	Boundx2 = w2;
	Boundy = h;
	Boundy2 = h2;
}

/*----------------------------------------------------------------------
convert degree to radian;  It should be a macro		//TODOA7
----------------------------------------------------------------------*/
float deg2r(float ang)
{
	return (ang/180.0) * M_PI;
}

/*----------------------------------------------------------------------
check collision with objects such as pillar
 @param angle       angle of pillar
 @return minus value if it collids
----------------------------------------------------------------------*/
int collide(float pt[3], float x, float y, float z, float sizex, float sizey, float sizez, float ang)
{
	if ( Is_GameOver )
	{
		return 0;
	}

	float d=0, b=0;
	float dis = fabs(z - Drz);
	float tmp=0;
	float er = fabs(deg2r(Drang+90.0-ang));

	// for 1st two cases (when ang is )
	if ( ang == 0.0 )
		return collide2(pt,x,y,z,sizex,sizey,sizez);
	else if ( ang == 90.0 )
		return collide3(pt,x,y,z,sizex,sizey,sizez);

	// for the 3rd case
	if ( dis <= 1.0 )
	{
		b = fabs(cos(er)*DR_X) + sizex;
		if ( Drang == ang )                  /* only for ang = 0 */
		{
			if ( fabs(x - Drx) < sizex || fabs(y - Dry) < sizey )
				return deduct_energy(Eng_Deduct_Rate);
		}
		else
			d = 1/cos(er) * ( fabs(x - Drx) );

		if ( (d - b) < 0.0 )
        {
			#ifdef DTEST
				printf("  **c1:  dis = %f, z = %f, Drz = %f\n", dis, z, Drz);
			#endif
            pt[0] = 0;
            pt[1] = 0;
            pt[2] = Drz;

            if ( d != 0.0 && fabs(d) > 1.0 )
            {
                tmp = (float) Eng_Deduct_Rate;
            }
            else
            {
                tmp = (float) Eng_Deduct_Rate;
            }
            deduct_energy(tmp);
            return -1.0;
		}
		else
			return (int) d;
	}
	else
		return dis;		// w95 ver
}

/*----------------------------------------------------------------------
helper method for collide to handle horizontal/wide object
----------------------------------------------------------------------*/
int collide2(float pt[3], float x, float y, float z, float sizex, float sizey, float sizez)
{
	float d=0, b=0; 
	float dis = fabs(z - Drz);
	float tmp=0;

	if ( dis <= 1.0 )
	{
		b = fabs(cos(deg2r(Drang)) * DR_X) + sizex/2.0;
		d = fabs(x - Drx);

		if ( (d - b) < 0.0 )
		{
			pt[0] = Drx + sign(Drx) * (b - sizex/2.0);
			pt[1] = fabs(sin(deg2r(Drang)) * DR_X) + sizex/2.0 + Dry;
			pt[2] = Drz;
			#ifdef DTEST
				printf("  **c2:  dis = %f, z = %f, Drz = %f\n", dis, z, Drz);
			#endif

			if ( d != 0.0 && d > 1.0)
			{
				tmp =  (float) Eng_Deduct_Rate / d;
			}
			else
			{
				tmp = (float) Eng_Deduct_Rate;
			}
			deduct_energy(tmp);
			return -tmp;
		}
		else
		{
			return (int) d;
		}
	}
	else
		return dis;		// w95 ver
}

/*----------------------------------------------------------------------
helper method for collide to handle vertical/tall object
----------------------------------------------------------------------*/
int collide3(float pt[3], float x, float y, float z, float sizex, float sizey, float sizez)
{
	float d=0, b=0;
	float dis = fabs(z - Drz);
	float tmp=0;

	if ( dis <= 1.0 )
	{
		b = fabs(sin(deg2r(Drang)) * DR_X) + sizex/2.0;
		d = fabs(y - Dry);

		if ( (d - b) < 0.0 )
		{
			pt[1] = Dry + sign(Dry) * b;
			pt[0] = fabs(cos(deg2r(Drang)) * DR_X) + Drx;
			pt[2] = Drz;
			#ifdef DTEST
				printf("  **c3:  dis = %f, z = %f, Drz = %f\n", dis, z, Drz);
			#endif

			if ( d != 0.0 && d > 1.0 )
			{
				tmp =  (float) Eng_Deduct_Rate / d;
			}
			else
			{
				tmp =  (float) Eng_Deduct_Rate;
			}
			deduct_energy(tmp);
			return -tmp;
		}
		else
		{
				return (int) d;
		}
	}
	else
		return dis;		// w95 ver
}

/*----------------------------------------------------------------------
check if (x,y,z) go through a ring at (sizex,sizey,sizez)
----------------------------------------------------------------------*/
int chk_through(float x, float y, float z, float sizex, float sizey, float sizez)
{
    float d, b;
    float dis = fabs(z - Drz);

    if ( dis <= 0.5 )
    {
        b = fabs(cos(Drang) * DR_X) + sizex/2.0;
        d = fabs(x - Drx);

        if ( d - b < 0.0 )
				return (int) - (Eng_Deduct_Rate);
        else
        {
            return (int) d;
        }
    }
	 else
		return dis;		// w95 ver
}
