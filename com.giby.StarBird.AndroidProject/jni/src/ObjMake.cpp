
/*=====================================

 file: ObjMake.c

 Owner: Hui Tin Yuen, Fred

 Description:

 - functions making objects into ogl display lists
 - with some reusable global variables like color vectors


 Hui Tin Yuen
 92057645
 cs_fredh@ug.cs.ust.hk

 comp341 project

=====================================*/


#include <math.h>
#include <assert.h>

#include "DLib/DGL.h"
#include "defines.h"
#include "ObjMake.h"
#include "Draw.h"
#include "Dream.h"


/*----------------------------------------------------------------------
global variables - shared to outside
----------------------------------------------------------------------*/

float fBlackVec[]     = {0.0, 0.0, 0.0}, fRedVec[]        = {1.0, 0.0, 0.0},
      fGreenVec[]     = {0.0, 1.0, 0.0}, fBlueVec[]       = {0.0, 0.0, 1.0},
      fWhiteVec[]     = {1.0, 1.0, 1.0}, fGrayVec[]       = {0.5, 0.6, 0.5},
      fYellowVec[]    = {1.0, 1.0, 0.0}, fCyanVec[]		  = {1.0, 0.0, 1.0},
	   fLightBlueVec[] = {0.2, 0.2, 1.0};

float fDeepGrassGreenVec[] = { 0.0, 0.4, 0.0 };
float fGrassGreenVec[] = { 0.0, 0.6, 0.0 };
float fOrangeVec[] = { 1.0, 0.6, 0.0 };



/*----------------------------------------------------------------------
global variables - for this file only
----------------------------------------------------------------------*/

/*

float stick_mat[] = {
    DIFFUSE, .4, .4, .4,
    AMBIENT, .6, .6, .6,
    SPECULAR, .8, .8, .8,
    SHININESS, 100,
    LMNULL,
};

float e_mat1[] = {
    DIFFUSE, .0, 1., 0.0,
    AMBIENT, .3, 1.0, 0.3,
    SPECULAR, .6, 1., .6,
    SHININESS, 0,
    LMNULL,
};

float lm[] = {
    AMBIENT, .6, .6, .6,
    LOCALVIEWER, 1,     // view's point of view *//* 1: point, 0: infiinte 
    LMNULL
};

float lm2[] = {
    AMBIENT, 1.0, 1.0, 1.0,
    LOCALVIEWER, 1,     //* view's point of view *//* 1: point, 0: infiinte 
    LMNULL
};
*/

static CGMaterial rect_mat = 
{
	{1., .9, .0, 1},		//DIFFUSE, 
	//{1., .9, .0, 1},		//DIFFUSE, 
	//{1., .9, .0, 1},		//DIFFUSE, 
	{0, 0, 0, 1},		//AMBIENT, 
	{0, 0, 0, 1},		//AMBIENT, 
	//{0, 0, 0, 1},	//SPECULAR, 
	{128},					//SHININESS, 
};


static CGMaterial e_mat1 = 
{
	{0, .6, 0, 1},		//DIFFUSE, 
	{.3, .6, .3, 1},		//AMBIENT, 
	{0, 0, 0, 1},	//SPECULAR, 
	{0},					//SHININESS, 
};

static CGMaterial e_mat2 = 
{
	{.1, .4, .1, 1},		//DIFFUSE, 
	{.1, .4, .1, 1},		//AMBIENT, 
	{0, 0, 0, 1},	//SPECULAR, 
	{0},					//SHININESS, 
};


static CGMaterial black_mat = // full black
{
	{0, 0, 0, 1},		//DIFFUSE, 
	{0, 0, 0, 1},		//AMBIENT, 
	{0, 0, 0, 1},	//SPECULAR, 
	{0},					//SHININESS, 
};

static CGMaterial red_mat = // full black
{
	{1, 0, 0, 1},		//DIFFUSE, 
	{1, 0, 0, 1},		//AMBIENT, 
	{0, 0, 0, 1},	//SPECULAR, 
	{128},					//SHININESS, 
};

static CGMaterial wall_mat = 
{
	{.5, .5, 0.5, 1},		//DIFFUSE, 
	{.3, .3, 0.3, 1},		//AMBIENT, 
	{.8, .8, .8, 1},	//SPECULAR, 
	{128},					//SHININESS, 
};



/*----------------------------------------------------------------------
change current mat
----------------------------------------------------------------------*/
void vChangeMat(CGMaterial mat)
{
	// colorize front face only
#ifdef SB_WIN
	unsigned int FACE_ENUM = GL_FRONT;
#else
	unsigned int FACE_ENUM = GL_FRONT_AND_BACK;		// SB_ANDROID only accept this!!
#endif

	glMaterialfv(FACE_ENUM, GL_AMBIENT, mat[0]);
	glMaterialfv(FACE_ENUM, GL_DIFFUSE, mat[1]);
	glMaterialfv(FACE_ENUM, GL_SPECULAR, mat[2]);
	glMaterialfv(FACE_ENUM, GL_SHININESS, mat[3]);
}



/*----------------------------------------------------------------------
initialize lights but NOT put it in display list
----------------------------------------------------------------------*/
void vMakeLights()
{
/*
float spot[] = {
	LCOLOR, 1, 1, 1,
	POSITION, .0, .0, .0, 1.0,
	SPOTDIRECTION, 0.0, -1.0, 0.0,
	SPOTLIGHT, 0.0, 40.0,
	LMNULL,
};

float sun[] = {
	LCOLOR, 1, 1, 1,
	POSITION, .0, .0, .0, 0.0,
	LMNULL,
};

static float lm[] = {
    AMBIENT, .6, .6, .6,
    LOCALVIEWER, 1,     //view's point of view *//* 1: point, 0: infiinte 
    LMNULL
};

    lmdef(DEFMATERIAL, OBJ_DREAM, 5, dr_mat);
    lmdef(DEFLIGHT, LIGHT0, 3, spot);
    lmdef(DEFLIGHT, LIGHT1, 3, spot);
    lmdef(DEFLIGHT, LIGHT2, 3, spot);
    lmdef(DEFLIGHT, LIGHT3, 3, spot);
    lmdef(DEFLIGHT, LIGHT4, 3, spot);
    lmdef(DEFLIGHT, LIGHT5, 3, spot);
    lmdef(DEFLIGHT, LIGHT6, 3, spot);
    lmdef(DEFLIGHT, LIGHT7, 3, sun);
    lmdef(DEFLMODEL, OBJ_DREAM, 3, lm);
*/

	//----------------
	// global env.
	
	//GLfloat light_Ka[] = { 0.2f, 0.2f, 0.2f, 1.0f };		/* ambient */
	GLfloat light_Ka[] = { 0.7f, 0.7f, 0.7f, 1.0f };		/* ambient */
	//GLfloat light_Ks[] = { 0.0f, 0.0f, 0.0f, 1.0f };		/* specular */
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_Ka);
//	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);		//SPEEDUP? // view's point of view   // 1: point, 0: infinite   //2012: default is 0

	//----------------
	// ver w95

	GLfloat lightZeroPosition[] = {0, 0, 0, 0};
	GLfloat lightZeroColorA[] = {0.5, 0.5, 0.5, 1.0};
	GLfloat lightZeroColorD[] = {.2, .2, .2, 1.0};
	GLfloat lightZeroColorS[] = {.04, .04, .04, 1.0};
	GLfloat lightZeroDir[] = {0.0, -1.0, 0.0, 1.0};

	//----------- set light
	float spot_ang = 40.0f;			// 40 deg
	float exp = 0.5f;				//GL default: 0.0, which make the lights being dark for my Galaxy Note

	for (int id=GL_LIGHT0; id<GL_LIGHT0+LIGHT_COUNT; id++)
	{
		//glLightfv(id, GL_POSITION, lightZeroPosition);
		glLightfv(id, GL_AMBIENT, lightZeroColorA);
		glLightfv(id, GL_DIFFUSE, lightZeroColorD);
		glLightfv(id, GL_SPECULAR, lightZeroColorS);
		glLightfv(id, GL_SPOT_DIRECTION, lightZeroDir);
		glLightfv(id, GL_SPOT_EXPONENT, &exp);
		glLightfv(id, GL_SPOT_CUTOFF, &spot_ang);
		glLightf(id, GL_CONSTANT_ATTENUATION, 1.0f);
		glLightf(id, GL_LINEAR_ATTENUATION, 0.0f);		//GL default: 0.0

		glEnable(id);
	}

}


/*----------------------------------------------------------------------
old legacy stuff (IrisGL)
----------------------------------------------------------------------*/
void vMakePlane()
{
	//BUG in OpenGL version
/*
    int nIndex;

    glNewList(OBJ_PLANE, GL_COMPILE);

       glColor3fv(fGrayVec);
       for (nIndex = 0; nIndex < 41; nIndex++)
	   {
          glRasterPos3d(-10 + 0.5 * nIndex, 0, -10);
          rdr(0, 0, 20);
          glRasterPos3d(-10, 0, -10 + 0.5 * nIndex);
          rdr(20, 0, 0);
       }

       glLineWidth(5);

       glRasterPos3d(0, 0, 0);	// move
       glColor3fv(fRedVec);
       draw(2, 0, 0);			// glDrawPixel !?

       glRasterPos3d(0, 0, 0);
       glColor3fv(fGreenVec);
       draw(0, 2, 0);

       glRasterPos3d(0, 0, 0);
       glColor3fv(fBlueVec);
       draw(0, 0, 2);

       glLineWidth(1);
    glEndList();
	*/
}


/*----------------------------------------------------------------------
normalize a vector
----------------------------------------------------------------------*/
void normalize(float vect[3])
{
    float length;

    length = vect[0]*vect[0] + vect[1]*vect[1] + vect[2]*vect[2];
    length = sqrt(length);
    vect[0] /= length;
    vect[1] /= length;
    vect[2] /= length;

	assert(length != 0.0);
}


/*----------------------------------------------------------------------
vMakeCube: make cube with displayListId being objId into display list
----------------------------------------------------------------------*/
void vMakeCube(unsigned short objId) 
{
   float v[8][3] = { {-0.5, -0.5, -0.5}, {-0.5, -0.5, 0.5},
         {-0.5,  0.5,  0.5}, {-0.5,  0.5, -0.5},
         { 0.5, -0.5, -0.5}, { 0.5, -0.5,  0.5},
         { 0.5,  0.5,  0.5}, { 0.5,  0.5, -0.5} };
   float n0123[3] = {-1.0, 0.0, 0.0},
         n0451[3] = { 0.0,-1.0, 0.0},
         n4765[3] = { 1.0, 0.0, 0.0},
         n3267[3] = { 0.0, 1.0, 0.0},
         n5621[3] = { 0.0, 0.0, 1.0},
         n3740[3] = { 0.0, 0.0,-1.0};

	GLubyte path[16] = 
	{
	    0, 1, 2, 3, 0, 4, 5, 6, 7, 4, 5, 1, 2, 6, 7, 3
	};

#ifdef SB_GLES
#else
		glNewList(objId, GL_COMPILE);
#endif

	if ( objId == OBJ_CUBE1 )
	{
		glColor3fv(fGrayVec);
		vChangeMat(wall_mat); // pillar
	}
	else if ( objId == OBJ_CUBE2 )
	  glColor3fv(fRedVec);
	else if ( objId == OBJ_CUBE3 )
	  glColor3fv(fBlueVec);
	else if ( objId == OBJ_CUBE4 )	//light box
	  glColor3fv(fYellowVec);

#ifdef SB_GLES
	GLubyte indices[] = {
		0, 1, 2,  2, 3, 0,
		0, 4, 5,  5, 1, 0,
		4, 7, 6,  6, 5, 4,
		3, 2, 6,  6, 7, 3,
		5, 6, 2,  2, 1, 5,
	};

	//draw triangles
	glVertexPointer( 3, GL_FLOAT, 0, v );
	glEnableClientState( GL_VERTEX_ARRAY );
	glDrawElements( GL_TRIANGLES, SIZEOFARR(indices), GL_UNSIGNED_BYTE, indices );

	// draw outline
	glColor3fv(fBlackVec);
	glLineWidth(2);
	vChangeMat(black_mat);
	glDrawElements( GL_LINE_STRIP, SIZEOFARR(path), GL_UNSIGNED_BYTE, path );
	glDisableClientState( GL_VERTEX_ARRAY );
#else
	float saveColor[4];

    glNormal3fv(n0123);
    glBegin(GL_POLYGON);
    glVertex3fv (v[0]);
    glVertex3fv (v[1]);
    glVertex3fv (v[2]);
    glVertex3fv (v[3]);
    glEnd();
    glNormal3fv(n0451);
    glBegin(GL_POLYGON);
    glVertex3fv (v[0]);
    glVertex3fv (v[4]);
    glVertex3fv (v[5]);
    glVertex3fv (v[1]);
    glEnd();
    glNormal3fv(n4765);
    glBegin(GL_POLYGON);
    glVertex3fv (v[4]);
    glVertex3fv (v[7]);
    glVertex3fv (v[6]);
    glVertex3fv (v[5]);
    glEnd();
    glNormal3fv(n3267);
    glBegin(GL_POLYGON);
    glVertex3fv (v[3]);
    glVertex3fv (v[2]);
    glVertex3fv (v[6]);
    glVertex3fv (v[7]);
    glEnd();
    glNormal3fv(n5621);
    glBegin(GL_POLYGON);
    glVertex3fv (v[5]);
    glVertex3fv (v[6]);
    glVertex3fv (v[2]);
    glVertex3fv (v[1]);
    glEnd();
    glNormal3fv(n3740);
    glBegin(GL_POLYGON);
    glVertex3fv (v[3]);
    glVertex3fv (v[7]);
    glVertex3fv (v[4]);
    glVertex3fv (v[0]);
    glEnd();

	//
	glGetFloatv(GL_CURRENT_COLOR, saveColor); /* save current color */
	glColor3fv(fBlackVec);
	glLineWidth(2);

	// draw outline
	vChangeMat(black_mat);
	glBegin(GL_LINE_STRIP);
		for (int i = 0; i < 16; i++)
			glVertex3fv (v[path[i]]);
	glEnd();

	//
	glColor3fv(saveColor);     /* restore color */
	glEndList();
#endif
}


/*----------------------------------------------------------------------
make wall of tunnel and put it into display list
----------------------------------------------------------------------*/
void vMakeChannel()
{
	float v[8][3] = { {-0.5, -0.5, -0.5}, {-0.5, -0.5, 0.5},	//left floor
         {-0.5,  0.5,  0.5}, {-0.5,  0.5, -0.5},		//left roof
         { 0.5, -0.5, -0.5}, { 0.5, -0.5,  0.5},		//right floor
         { 0.5,  0.5,  0.5}, { 0.5,  0.5, -0.5} };		//right root
	float n0321[3] = { 1.0, 0.0, 0.0 };
	float n0154[3] = { 0.0, 1.0, 0.0 };
	float n4567[3] = { -1.0, 0.0, 0.0 };
	float n3762[3] = { 0.0, -1.0, 0.0 };
	float n5126[3] = { 0.0, 0.0, -1.0 };


#ifdef SB_GLES
	GLubyte indicesOld[] = {
		0, 3, 2, 1,		0, 1, 5, 4,
		4, 5, 6, 7,		3, 7, 6, 2 };
	GLubyte indices[] = {
		0, 3, 2, 2, 1, 0,	0, 1, 5, 5, 4, 0,
		4, 5, 6, 6, 7, 4, 	3, 7, 6, 6, 2, 3,
		};

	glColor3fv(fBlueVec);
	vChangeMat(wall_mat);

	glVertexPointer( 3, GL_FLOAT, 0, v );
//TODO A1:	glNormalPoionter
	glEnableClientState( GL_VERTEX_ARRAY );
	glDrawElements( GL_TRIANGLES, SIZEOFARR(indices), GL_UNSIGNED_BYTE, indices );
	glDisableClientState( GL_VERTEX_ARRAY );
#else
    glNewList(OBJ_CHANNEL, GL_COMPILE);

    glColor3fv(fBlueVec);
 	vChangeMat(wall_mat);

	glNormal3fv(n0321);
    glBegin(GL_POLYGON);
    glVertex3fv (v[0]);
    glVertex3fv (v[3]);
    glVertex3fv (v[2]);
    glVertex3fv (v[1]);
    glEnd();
	glNormal3fv(n0154);
    glBegin(GL_POLYGON);
    glVertex3fv (v[0]);
    glVertex3fv (v[1]);
    glVertex3fv (v[5]);
    glVertex3fv (v[4]);
    glEnd();
	glNormal3fv(n4567);
    glBegin(GL_POLYGON);
    glVertex3fv (v[4]);
    glVertex3fv (v[5]);
    glVertex3fv (v[6]);
    glVertex3fv (v[7]);
    glEnd();
	glNormal3fv(n3762);
    glBegin(GL_POLYGON);
    glVertex3fv (v[3]);
    glVertex3fv (v[7]);
    glVertex3fv (v[6]);
    glVertex3fv (v[2]);
    glEnd();
/*
	glNormal3fv(n5126);
    glBegin(GL_POLYGON);
    glVertex3fv (v[5]);
    glVertex3fv (v[1]);
    glVertex3fv (v[2]);
    glVertex3fv (v[6]);
    glEnd();
*/

    glEndList();
#endif
}

/*----------------------------------------------------------------------
draw a triangle; helper method for makeShadow
----------------------------------------------------------------------*/
void vDrawPolygon2(float b[3][3])
{
#ifdef SB_GLES
	//TODOA1b
#else
    float n0[3] = {0.0, 1.0, 0.0};

    glNormal3fv(n0);
    glBegin(GL_POLYGON);
    glVertex3fv(b[0]);
    glVertex3fv(b[2]);
    glVertex3fv(b[1]);
    glVertex3fv(b[0]);
    glEnd();
#endif
}

/*----------------------------------------------------------------------
make a spark and put it into display list
----------------------------------------------------------------------*/
void vMakeSpark()
{
   float v[4][3] = { {-0.5, -0.5, -0.5},
         { 0.5, -0.5, -0.5},
         { 0.5,  0.5,  -0.5},
         {-0.5,  0.5, -0.5}};

#ifdef SB_GLES
	GLubyte indices[] = {
		0, 1, 2,  2, 3, 0 };

	glColor3fv(fRedVec);

	glVertexPointer( 3, GL_FLOAT, 0, v );
	glEnableClientState( GL_VERTEX_ARRAY );
	glDrawElements( GL_TRIANGLES, SIZEOFARR(indices), GL_UNSIGNED_BYTE, indices );
	glDisableClientState( GL_VERTEX_ARRAY );
#else
    glNewList(OBJ_SPARK, GL_COMPILE);
    glColor3fv(fRedVec);

    glBegin(GL_POLYGON);
    glVertex3fv(v[0]);
    glVertex3fv(v[1]);
    glVertex3fv(v[2]);
    glVertex3fv(v[3]);
    glEnd();
    glEndList();
#endif
}

/*----------------------------------------------------------------------
make a yellow rectangle for for generic use and put it into display list
----------------------------------------------------------------------*/
void vMakeRect()
{
	const float UNIT =  0.5;	// these 0.01 offsets are not necessary for iris version.
#ifdef SB_GLES
	float v[4][3] =	{
		{ -UNIT, 0.0, 0.0 },
		{ UNIT, 0.0, 0.0 },
		{ UNIT, 0.0, -RECT_LEN },
		{ -UNIT, 0.0, -RECT_LEN } };

	GLubyte indices[] = {
		0, 1, 3, 2, 3, 1 };

	glColor3fv(fYellowVec);
	vChangeMat(rect_mat);

	glVertexPointer( 3, GL_FLOAT, 0, v );
	glEnableClientState( GL_VERTEX_ARRAY );
	glDrawElements( GL_TRIANGLES, SIZEOFARR(indices), GL_UNSIGNED_BYTE, indices );
	glDisableClientState( GL_VERTEX_ARRAY );
#else
	float v1[3] = { -0.5, 0.0, 0.0 };
    float v2[3] = { 0.5, 0.0, 0.0 };
    float v3[3], v4[3];
//    float n[3] = { 0.0, 1.0, 0.0 };

    v3[0] = v1[0];
    v3[1] = v1[1];
    v3[2] = -RECT_LEN;
    v4[0] = v2[0];
    v4[1] = v2[1];
    v4[2] = -RECT_LEN;

	glNewList(OBJ_RECT, GL_COMPILE);

	glColor3fv(fYellowVec);
	vChangeMat(rect_mat);
    glBegin(GL_POLYGON);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v4);
    glVertex3fv(v3);
    glEnd();
    glEndList();
#endif
}

/*----------------------------------------------------------------------
make a rectangle with displayListId being objId into display list
for ending scene
----------------------------------------------------------------------*/
void vMakeRect2(int objId)
{
#ifdef SB_GLES
	
    float v[4][3] = {{ -0.5, 0.0, 0.0},
					{ 0.5, 0.0, 0.0},
					{ -0.5, 0.0, -1.0},
					{ 0.5, 0.0, -1.0}};

	GLubyte indices[] = {
		0, 1, 3,	3, 2, 0,
	};

	if ( objId == OBJ_SMALL_EARTH )
	{
		glColor3fv(fGrassGreenVec);
		vChangeMat(e_mat1);
	}
	else
	{
		glColor3fv(fDeepGrassGreenVec);
		vChangeMat(e_mat2);
	}

//TODO A1d add normal
	glVertexPointer( 3, GL_FLOAT, 0, v );
	glEnableClientState( GL_VERTEX_ARRAY );
	glDrawElements( GL_TRIANGLES, SIZEOFARR(indices), GL_UNSIGNED_BYTE, indices );
	glDisableClientState( GL_VERTEX_ARRAY );
#else
	float v1[3] = { -0.5, 0.0, 0.0 };
    float v2[3] = { 0.5, 0.0, 0.0 };
    float v3[3], v4[3];
    float n[3] = { 0.0, 1.0, 0.0 };

    v3[0] = v1[0];
    v3[1] = v1[1];
    v3[2] = -1.0;
    v4[0] = v2[0];
    v4[1] = v2[1];
    v4[2] = -1.0;

    glNewList(objId, GL_COMPILE);
	
	 if ( objId == OBJ_SMALL_EARTH )
	 {
		glColor3fv(fGrassGreenVec);
		vChangeMat(e_mat1);
	 }
	 else
	 {
		glColor3fv(fDeepGrassGreenVec);
		vChangeMat(e_mat2);
	 }
	
	glNormal3fv(n);
    glBegin(GL_POLYGON);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v4);
    glVertex3fv(v3);
    glEnd();
    glEndList();
#endif
}


/*----------------------------------------------------------------------
make a vertical plane and put it into display list
----------------------------------------------------------------------*/
void vMakeVPlane()
{
    float v[4][3] = {{ 0.0, 0.5, 0.5},
					{ 0.0, -0.5, 0.5},
					{ 0.0, -0.5, -0.5},
					{ 0.0, 0.5, -0.5}};
#ifdef SB_GLES
	GLubyte indices[] = {
		0, 1, 2,	2, 3, 0,
	};

	vChangeMat(wall_mat);
    glColor3fv(fGreenVec);

	glVertexPointer( 3, GL_FLOAT, 0, v );
	glEnableClientState( GL_VERTEX_ARRAY );
	glDrawElements( GL_TRIANGLES, SIZEOFARR(indices), GL_UNSIGNED_BYTE, indices );
	glDisableClientState( GL_VERTEX_ARRAY );
#else
	glNewList(OBJ_VPLANE, GL_COMPILE);
	vChangeMat(wall_mat);
    glColor3fv(fGreenVec);
    glBegin(GL_POLYGON);
    glVertex3fv(v[0]);
    glVertex3fv(v[1]);
    glVertex3fv(v[2]);
    glVertex3fv(v[3]);
    glVertex3fv(v[0]);
    glEnd();
	glEndList();
#endif
}


/*----------------------------------------------------------------------
make a ring plane and put it into display list
----------------------------------------------------------------------*/
void vDrawRingPlane(float v1[3], float v2[3])
{
#ifdef SB_GLES
	float v[4][3] = { 
		v1[0], v1[1], v1[2], 
		v2[0], v2[1], v2[2], 
		v1[0], v1[1], RING_Z, 
		v2[0], v2[1], RING_Z,
	};
	GLubyte indices[] = {
		0, 2, 3,	3, 1, 0,
	};

	glVertexPointer( 3, GL_FLOAT, 0, v );
	glEnableClientState( GL_VERTEX_ARRAY );
	glDrawElements( GL_TRIANGLES, SIZEOFARR(indices), GL_UNSIGNED_BYTE, indices );
	glDisableClientState( GL_VERTEX_ARRAY );
#else
    float v3[3];
    float v4[3];

    v3[0] = v1[0];
    v3[1] = v1[1];
    v3[2] = RING_Z;
    v4[0] = v2[0];
    v4[1] = v2[1];
    v4[2] = RING_Z;

	glBegin(GL_POLYGON);
    glVertex3fv(v1);
    glVertex3fv(v3);
    glVertex3fv(v4);
    glVertex3fv(v2);
    glEnd();
#endif
}


/*----------------------------------------------------------------------
make a ring and put it into display list
----------------------------------------------------------------------*/
void vMakeRing()
{
    float v[8][3] = {{ 5.0, 0.0, 0.0 },
                    { 3.53553, 3.53553, 0.0},
                    { 0.0, 5.0, 0.0 },
                    { -3.53553, 3.53553, 0.0},
                    { -5.0, 0.0, 0.0 },
                    { -3.53553, -3.53553, 0.0},
                    { 0.0, -5.0, 0.0 },
                    { 3.53553, -3.53553, 0.0}};

#ifdef SB_GLES
	//TOSPEEDUP replace this 8 calls by one call to glDrawElements()
	//TODOA1d?  try double faces?
#else
	glNewList(OBJ_RING, GL_COMPILE);
#endif

	vChangeMat(red_mat);
    glColor3fv(fRedVec);
    vDrawRingPlane(v[0], v[1]);
    vDrawRingPlane(v[1], v[2]);
    vDrawRingPlane(v[2], v[3]);
    vDrawRingPlane(v[3], v[4]);
    vDrawRingPlane(v[4], v[5]);
    vDrawRingPlane(v[5], v[6]);
    vDrawRingPlane(v[6], v[7]);
    vDrawRingPlane(v[7], v[0]);

#ifdef SB_GLES
#else
	glEndList();
#endif
}


/*----------------------------------------------------------------------
make a square using line and put it into display list
----------------------------------------------------------------------*/
void vMakeSq()
{
	const float UNIT =  0.5 - OFFSET_FROM_WALL;	// these 0.01 offsets are not necessary for iris version.

#ifdef SB_GLES
	float v[4][3] =	{
		{ -UNIT, -UNIT, 0.0 },
		{ UNIT, -UNIT, 0.0 },
		{ UNIT, UNIT, 0.0 },
		{ -UNIT, UNIT, 0.0 } };

	glColor3fv(fBlackVec);
	glLineWidth(1);

	glVertexPointer( 3, GL_FLOAT, 0, v );
	glEnableClientState( GL_VERTEX_ARRAY );
	glDrawArrays( GL_LINE_LOOP, 0, SIZEOFARR(v) );
	glDisableClientState( GL_VERTEX_ARRAY );
#else
	float v1[3] = { -UNIT, -UNIT, 0.0 };
	float v2[3] = { UNIT, -UNIT, 0.0 };
	float v3[3], v4[3];

	v3[0] = v1[0];
	v3[1] = UNIT;
	v3[2] = v1[2];
	v4[0] = v2[0];
	v4[1] = UNIT;
	v4[2] = v2[2];

	glNewList(OBJ_SQUARE, GL_COMPILE);

	glColor3fv(fBlackVec);
	vChangeMat(black_mat);
	glLineWidth(1);

	glBegin(GL_LINE_STRIP);
	glVertex3fv(v1);
	glVertex3fv(v2);
	glVertex3fv(v4);
	glVertex3fv(v3);
	glVertex3fv(v1);
	glEnd();

	glEndList();
#endif
}


/*----------------------------------------------------------------------
make a bullet and put it into display list
----------------------------------------------------------------------*/
void vMakeFire()
{
#ifdef SB_GLES
	float v[4][3] = { 
		-0.5, 0.0, 0.0,
		0.5, 0.0, 0.0,
		0.5, 0.0, -0.5, 
		0.5, 0.0, -0.5,
	};
	GLubyte indices[] = {
		0, 1, 3,	3, 2, 0,
	};

	glColor3fv(fOrangeVec);

	glVertexPointer( 3, GL_FLOAT, 0, v );
	glEnableClientState( GL_VERTEX_ARRAY );
	glDrawElements( GL_TRIANGLES, SIZEOFARR(indices), GL_UNSIGNED_BYTE, indices );
	glDisableClientState( GL_VERTEX_ARRAY );
#else
    float v1[3] = { -0.5, 0.0, 0.0 };
    float v2[3] = { 0.5, 0.0, 0.0 };
//    float n[3] = { 0.0, 1.0, 0.0 };

	float v3[3], v4[3];
    v3[0] = v1[0];
    v3[1] = v1[1];
    v3[2] = -0.5;
    v4[0] = v2[0];
    v4[1] = v2[1];
    v4[2] = -0.5;

    glNewList(OBJ_FIRE, GL_COMPILE);
    glColor3fv(fOrangeVec);
	 //vChangeMat();
    glBegin(GL_POLYGON);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v4);
    glVertex3fv(v3);
    glEnd();
    glEndList();
#endif
}


/*----------------------------------------------------------------------
draw four polygons with normal well set
----------------------------------------------------------------------*/
void vDrawPolygon(float b[4][3], float n[4][3])
{
	//TODOF?  use GL Buffer Objects (which is the only way in OpenGL 3.0 and OpengGLES 2.0???)
#ifdef SB_GLES
	GLubyte indices[] = {
		1, 2, 0,	3, 2, 1,
		0 ,2 ,3,	0, 3, 1 };
//TODOA1? use normal!?
	glVertexPointer( 3, GL_FLOAT, 0, b );
	glEnableClientState( GL_VERTEX_ARRAY );
	glDrawElements( GL_TRIANGLES, SIZEOFARR(indices), GL_UNSIGNED_BYTE, indices );
	glDisableClientState( GL_VERTEX_ARRAY );
#else
	glNormal3fv(n[0]);
    glBegin(GL_POLYGON);
    glVertex3fv(b[1]);
    glVertex3fv(b[2]);
    glVertex3fv(b[0]);
    glEnd();

    glNormal3fv(n[1]);
    glBegin(GL_POLYGON);
    glVertex3fv(b[3]);
    glVertex3fv(b[2]);
    glVertex3fv(b[1]);
    glEnd();

    glNormal3fv(n[2]);
    glBegin(GL_POLYGON);
    glVertex3fv(b[0]);
    glVertex3fv(b[2]);
    glVertex3fv(b[3]);
    glEnd();

    glNormal3fv(n[3]);
    glBegin(GL_POLYGON);
    glVertex3fv(b[0]);
    glVertex3fv(b[3]);
    glVertex3fv(b[1]);
    glEnd();
#endif
}


/*----------------------------------------------------------------------
variables for making texture
----------------------------------------------------------------------*/
static int stencilReflection = 1, stencilShadow = 1, offsetShadow = 1;
//static int renderShadow = 1, renderDinosaur = 1, renderReflection = 1;
static int linearFiltering = 0, useMipmaps = 1;

static char *circles[] = {
  "....xxxx........",
  "..xxxxxxxx......",
  ".xxxxxxxxxx.....",
  ".xxx....xxx.....",
  "xxx......xxx....",
  "xxx......xxx....",
  "xxx......xxx....",
  "xxx......xxx....",
  ".xxx....xxx.....",
  ".xxxxxxxxxx.....",
  "..xxxxxxxx......",
  "....xxxx........",
  "................",
  "................",
  "................",
  "................",
};

/*----------------------------------------------------------------------
make 2D texture 
----------------------------------------------------------------------*/
static void vMakeFloorTexture(void)
{
	if ( ! Draw_Engine.isTexture )
		return;

	GLubyte floorTexture[16][16][3];
	GLubyte *loc;
	int s, t;

	/* Setup RGB image for the texture. */
	loc = (GLubyte*) floorTexture;
	for (t = 0; t < 16; t++) {
	 for (s = 0; s < 16; s++) {
		if (circles[t][s] == 'x') {
	/* Nice green. */
		  loc[0] = 0x1f;
		  loc[1] = 0x8f;
		  loc[2] = 0x1f;
		} else {
	/* Light gray. */
		  loc[0] = 0xaa;
		  loc[1] = 0xaa;
		  loc[2] = 0xaa;
		}
		loc += 3;
	 }
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (useMipmaps)
	{
		 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
#ifdef SB_GLES
		 //TODOA1c
		 //ref: glhBuild2DMipmaps OR glues
#else

		 gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 16, 16,
			GL_RGB, GL_UNSIGNED_BYTE, floorTexture);
#endif
	}
	else 
	{
		 if (linearFiltering) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		 } else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		 }
		 glTexImage2D(GL_TEXTURE_2D, 0, 3, 16, 16, 0,
			GL_RGB, GL_UNSIGNED_BYTE, floorTexture);
	}
}

extern void vDrawObj(unsigned short id)
{
#ifdef SB_GLES
	switch(id)
	{
	case OBJ_SQUARE:
		vMakeSq();
		break;
	case OBJ_RING:
		vMakeRing();
		break;
	case OBJ_VPLANE:
		vMakeVPlane();
		break;
	case OBJ_SPARK:
		vMakeSpark();
		break;
	case OBJ_RECT:
		vMakeRect();
		break;
	case OBJ_FIRE:
		vMakeFire();
		break;
	case OBJ_SMALL_EARTH:
	case OBJ_RECT_EARTH:
		vMakeRect2(id);
		break;
	case OBJ_CUBE1:
	case OBJ_CUBE2:
	case OBJ_CUBE3:
	case OBJ_CUBE4:
		vMakeCube(id);
		break;
	case OBJ_CHANNEL:
		vMakeChannel();
		break;
	default:
		assert(0);
		break;
	}
#else
	glCallList(id);
#endif
//	dbg_msg("vDrawObj(): end");
}

/*----------------------------------------------------------------------
make all objects into display list
----------------------------------------------------------------------*/
void vMakeAll(void)
{
	glPushMatrix();

#ifdef SB_GLES
#else
	dream::vMakeDREAM();
	vMakePlane();
	vMakeChannel();
	vMakeCube(OBJ_CUBE1);
	vMakeCube(OBJ_CUBE2);
	vMakeCube(OBJ_CUBE3);
	vMakeCube(OBJ_CUBE4);
	vMakeSq();
	vMakeRing();
	vMakeVPlane();
	vMakeSpark();
	vMakeRect();
	vMakeRect2(OBJ_SMALL_EARTH);
	vMakeRect2(OBJ_RECT_EARTH);
	dream::vMakeDRSha();
	vMakeFire();
#endif

	//glRotated(90,1,1,1);

	vMakeLights();
	glLineWidth(2.0f);

#ifdef SB_ANDROID
	//TODOA2? for ANdroid!?
#else
	vMakeFloorTexture();
#endif

	glPopMatrix();
}
