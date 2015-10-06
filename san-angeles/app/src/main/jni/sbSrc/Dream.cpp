
#include <math.h>
#include <assert.h>

#include "DLib/DGL.h"
#include "Defines.h"
#include "ObjMake.h"
#include "Draw.h"


namespace dream
{

static CGMaterial dr_mat = 
{
	{.2, .2, .2, 1},		//DIFFUSE, 
	{.2, .2, .2, 1},		//AMBIENT, 
	{0.5, 0.5, 0.5, 1},	//SPECULAR, 
	{20},					//SHININESS, 
};

static CGMaterial dr_matl = 
{
	{.1, .1, 1.0, 1},		//DIFFUSE, 
	{.3, .3, 1.0, 1},		//AMBIENT, 
	{.5, .5, 1.0, 1},	//SPECULAR, 
	{20},					//SHININESS, 
};

static CGMaterial dr_mate = 
{
	{1., .9, .0, 1},		//DIFFUSE, 
	{1., .9, .0, 1},		//AMBIENT, 
	{1., 1., .0, 1},	//SPECULAR, 
	{128},					//SHININESS, 
};

static CGMaterial dr_matt = 
{
	{.2, .2, .2, 1},
	{.07, .07, .07, 1},
	{.3, .3, .3, 1},
	{20},
};

static CGMaterial sha_mat = 
{
	{.2, .2, .2, 1},
	//{.2, .2, .2, 1},
	{.01, .01, .01, 1},
	{.0, .0, .0, 0},
	{0},
};

static float m[4][3] = { { 3.85059, -0.30601, 2.08459 },
         { 3.85059, -0.30601, -2.08459 },
         { 2.35461, 1.93931, 0.0 },
         { -0.63735, -0.30601, 0.0 } };
static float h[4][3] = {{ 2.65947, -0.29160, 1.61340 },
          { 2.65947, -0.29160, -1.61340 },
          { 0.0,     0.87480,  0.0 },
          { -5.31895, -0.29160, 0.0 }};

static float tr[4][3] = {
          { 1.09240, -1.62955, -2.3162 },
		  { 1.13615, -1.59017, -1.33643 },
          { 2.76710, 0.52186, -1.98582 },
          { 7.03273, 3.71914, -2.30476 }};

static float tl[4][3] = {
		  { 1.13615, -1.59017, 1.33643 },
          { 1.09240, -1.62955, 2.3162 },
          { 2.76710, 0.52186, 1.98582 },
          { 7.03273, 3.71914, 2.30476 }};

static float ll[4][3] = {
		  { 2.39998, -0.28919, 0.16544 },
          { 1.63190, -0.04183, 0.95637 },
          { 4.72177, -0.44921, 3.26241 },
          { 9.74279, -2.36707, 8.66543 }};
static float lr[4][3] = {
          { 1.63190, -0.04183, -0.95637 },
		  { 2.39998, -0.28919, -0.16544 },
          { 4.72177, -0.44921, -3.26241 },
          { 9.74279, -2.36707, -8.66543 }};
static float e[4][3] = {
          { 3.75993, -0.27071, 0.63933 },
          { 3.75993, -0.27071, -0.63933 },
          { 3.45912, 1.20989, 0.0 },
          { 2.63805, -0.693, 0.0 }};

static float n1[4][3] = {          /* main */
		  { 2.0, 1.2, 0.0},
          { 0.0, 1.0, -1.0},
          { 0.0, 1.0, 1.0},
          { 0.0, -1.0, 0.0}};
static float n2[4][3] = {			/* head */
		  { 2.0, 1.3, 0.0},
          { 0.0, 1.0, -1.0},
          { 0.0, 1.0, 1.0},
          { 0.0, -1.0, 0.0}};
static float n3[4][3] = {			/* top wing */
          { -3.74, 3.06, 0.0},
          { -4.91, 2.94, 2.29},
          { -6.35, 3.37, -1.86},
		  { 2.5, -2.88, 0.0}};
static float n4[4][3] = {			/* low L wing */
          { 0.4, 5.3, 0.0 },
          { 0.2, 3.24, 2.55 },
		  { 3.68, 2.00, -2.0 },
          { -0.84, -5.16, 0.0} };
static float n5[4][3] = {			/* low R wing */
          { 0.4, 5.3, 0.0 },
		  { 3.68, 2.00, 2.0 },
		  { 0.2, 3.48, -2.55},
          { -0.84, -5.16, 0.0} };
/*
    float n4[4][3] = {
          { 0.4, 5.3, 0.0 },
          { 0.2, 3.24, 2.55 },
		  { 4.29, 6.79, -0.51 },
          { -0.84, -5.16, 0.0} };
    float n5[4][3] = {
          { 0.4, 5.3, 0.0 },
		  { 3.68, 3.27, 2.0 },
		  { 0.2, 3.78, -3.15},
          { -0.84, -5.16, 0.0} };
*/

/*----------------------------------------------------------------------
make our Dream and put it into display list
----------------------------------------------------------------------*/
void vMakeDREAM()
{
#ifdef SB_GLES
	//memo: it is copied to drawPart()
#else
	glPushMatrix();

	glNewList(OBJ_DREAM, GL_COMPILE);
	vChangeMat(dr_mat);
	glColor3fv(fGrayVec);
	vDrawPolygon(m,n1);
	glEndList();

	glNewList(OBJ_DREAMh, GL_COMPILE);
	vChangeMat(dr_matl);
	glColor3fv(fGrayVec);
	vDrawPolygon(h,n2);
	glEndList();

	glNewList(OBJ_DREAMtl, GL_COMPILE);
	vChangeMat(dr_matt);
	glColor3fv(fGrayVec);
	vDrawPolygon(tl,n3);
	glEndList();

	glNewList(OBJ_DREAMtr, GL_COMPILE);
	vChangeMat(dr_matt);
	glColor3fv(fGrayVec);
	vDrawPolygon(tr,n3);
	glEndList();

	//-----------//
	glNewList(OBJ_DREAMll, GL_COMPILE);
	vChangeMat(dr_matl);
	glColor3fv(fGrayVec);	
	vDrawPolygon(ll,n4);
	glEndList();

	glNewList(OBJ_DREAMlr, GL_COMPILE);
	vChangeMat(dr_matl);
	glColor3fv(fGrayVec);
	vDrawPolygon(lr,n5);
	glEndList();

	glNewList(OBJ_DREAMe, GL_COMPILE);
	glColor3fv(fGrayVec);
	vChangeMat(dr_mate);
	vDrawPolygon(e,n1);
	glEndList();

#ifdef DEBUG_TITLE
	glNewList(OBJ_DREAM_ALL, GL_COMPILE);
	dream::drawPart(OBJ_DREAM);
	dream::drawPart(OBJ_DREAMh);
	dream::drawPart(OBJ_DREAMtl);
	dream::drawPart(OBJ_DREAMtr);
	dream::drawPart(OBJ_DREAMll);
	dream::drawPart(OBJ_DREAMlr);
	dream::drawPart(OBJ_DREAMe);
	glEndList();
#endif

	glPopMatrix();
#endif
}


void drawPart(unsigned short id)
{
#ifdef SB_GLES
	switch(id)
	{
	case OBJ_DREAM:
		vChangeMat(dr_mat);
		glColor3fv(fGrayVec);
		vDrawPolygon(m,n1);
		break;

	case OBJ_DREAMh:
		vChangeMat(dr_matl);
		glColor3fv(fGrayVec);
		vDrawPolygon(h,n2);
		break;

	case OBJ_DREAMtl:
		vChangeMat(dr_matt);
		glColor3fv(fGrayVec);
		vDrawPolygon(tl,n3);
		break;

	case OBJ_DREAMtr:
		vChangeMat(dr_matt);
		glColor3fv(fGrayVec);
		vDrawPolygon(tr,n3);
		break;

	//-----------//
	case OBJ_DREAMll:
		vChangeMat(dr_matl);
		glColor3fv(fGrayVec);	
		vDrawPolygon(ll,n4);
		break;

	case OBJ_DREAMlr:
		vChangeMat(dr_matl);
		glColor3fv(fGrayVec);
		vDrawPolygon(lr,n5);
		break;

	case OBJ_DREAMe:
		glColor3fv(fGrayVec);
		vChangeMat(dr_mate);
		vDrawPolygon(e,n1);
		break;
	default:
		assert(0);
		break;
	}
#else
	glCallList(id);
#endif
	//dbg_msg("DrawPart() end: id:%d", id);
}



/*----------------------------------------------------------------------
make a shadow and put it into display list
----------------------------------------------------------------------*/
void vMakeDRSha()
{
#ifdef SB_GLES
#else
    float shm[3][3] = {
        { -0.63735, 0.0, 0.0 },
        { 3.85059, 0.0, -2.08459 },
		{ 3.85059, 0.0, 2.08459 }};
    float shh[3][3] = {
          { -5.31895, 0.0, 0.0 },
          { 2.65947, 0.0, -1.61340 },
		  { 2.65947, 0.0, 1.61340 }};
    float shtr[3][3] = {
		  { 1.13615, 0.0, -1.33643 },
          { 1.09240, 0.0, -2.3162 },
          { 7.03273, 0.0, -2.30476 }};
    float shtl[3][3] = {
		  { 1.13615, 0.0, 1.33643 },
          { 7.03273, 0.0, 2.30476 },
          { 1.09240, 0.0, 2.3162 }};
    float shll[3][3] = {
          { 9.74279, 0.0, 8.66543 },
          { 1.63190, 0.0, 0.95637 },
		  { 2.39998, 0.0, 0.16544 }};
    float shlr[3][3] = {
		  { 2.39998, 0.0, -0.16544 },
          { 1.63190, 0.0, -0.95637 },
          { 9.74279, 0.0, -8.66543 }};

  glNewList(OBJ_DRSHA, GL_COMPILE);
   glColor3fv(fGrayVec);
	vChangeMat(sha_mat);
   vDrawPolygon2(shm);
   vDrawPolygon2(shh);
	glEndList();

   glNewList(OBJ_DRSHA_TL, GL_COMPILE);
    vDrawPolygon2(shtl);
	glEndList();

    glNewList(OBJ_DRSHA_TR, GL_COMPILE);
    vDrawPolygon2(shtr);
	glEndList();

    glNewList(OBJ_DRSHA_LL, GL_COMPILE);
    vDrawPolygon2(shll);
	glEndList();

    glNewList(OBJ_DRSHA_LR, GL_COMPILE);
    vDrawPolygon2(shlr);
	glEndList();
#endif
}
void drawShaPart(unsigned short id)
{
}
}
