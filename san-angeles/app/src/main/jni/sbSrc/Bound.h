
/*=====================================

 file: bound.h

 Owner: Hui Tin Yuen, Fred

 Description:

 - includes boundary checking and simulation of polygon dectection

=====================================*/


//--------------------
// public functions for drawPlay
extern void bound_resetInternalStates();
extern void reset_view_co(float *);
extern void reset_turn(float *v, char rot);
extern float set_sp(float );
extern void chk_vcons(float *pos, float *dis, int axis);
extern void chk_tcons(float *ang, char rot);
extern int collide(float *pt, float x, float y, float z, float orgx, float orgy, float orgz, float ang);
extern void update_dream(float *, float *, float *, float *);
extern void update_walls(float, float, float,float);
extern int chk_through(float x, float y, float z, float orgx, float orgy, float orgz);
extern void shake(float *);
extern float deg2r(float);
extern int sign(float);

//private functions
extern int collide2(float *pt, float x, float y, float z, float orgx, float orgy, float orgz);
extern int collide3(float *pt, float x, float y, float z, float orgx, float orgy, float orgz);


//--------------------
// for c
extern int Shake;		// = true/false

//--------------------
// for drawInit
extern float Eng_Deduct_Rate;

