
#include <math.h>
#include "DGL.h"
#include "../GLHLib/MathLibrary.h"
#include "../FreeGlut/freeglut_internal.h"

/*

GLU port

ref: GLHLIB http://www.opengl.org/wiki/GluLookAt_code

other alternatives: http://code.google.com/p/glues/
*/

//#define BUILD_USE_GLHLIB	//MEMO: use standard GL calls instead of the C functions from GLHLib	//MEMO: use standard GL calls instead of the C functions from GLHLib


#ifdef BUILD_USE_GLHLIB
//
static sreal GlobalIdentityMatrixf[]=
	{1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0};


//
static void glhLoadIdentityf2(sreal *matrix)
{
	memcpy(matrix, GlobalIdentityMatrixf, sizeof(sreal)*16);
}

//
static void glhTranslatef2(sreal *matrix, sreal x, sreal y, sreal z)
{
	matrix[12]=matrix[0]*x+matrix[4]*y+matrix[8]*z+matrix[12];
	matrix[13]=matrix[1]*x+matrix[5]*y+matrix[9]*z+matrix[13];
	matrix[14]=matrix[2]*x+matrix[6]*y+matrix[10]*z+matrix[14];
	matrix[15]=matrix[3]*x+matrix[7]*y+matrix[11]*z+matrix[15];
}
#endif

//
static void glhLookAtf2( float *matrix, float *eyePosition3D,
                  float *center3D, float *upVector3D )
{
#define ComputeNormalOfPlane ComputeNormalOfPlaneFLOAT_2
#define NormalizeVector NormalizeVectorFLOAT_2

#ifdef BUILD_USE_GLHLIB
	float resultMatrix[16];
#endif

	float forward[3], side[3], up[3];
	float matrix2[16];
	//------------------
	forward[0] = center3D[0] - eyePosition3D[0];
	forward[1] = center3D[1] - eyePosition3D[1];
	forward[2] = center3D[2] - eyePosition3D[2];
	NormalizeVector(forward);
	//------------------
	//Side = forward x up
	ComputeNormalOfPlane(side, forward, upVector3D);
	NormalizeVector(side);
	//------------------
	//Recompute up as: up = side x forward
	ComputeNormalOfPlane(up, side, forward);
	//------------------
	matrix2[0] = side[0];
	matrix2[4] = side[1];
	matrix2[8] = side[2];
	matrix2[12] = 0.0;
	//------------------
	matrix2[1] = up[0];
	matrix2[5] = up[1];
	matrix2[9] = up[2];
	matrix2[13] = 0.0;
	//------------------
	matrix2[2] = -forward[0];
	matrix2[6] = -forward[1];
	matrix2[10] = -forward[2];
	matrix2[14] = 0.0;
	//------------------
	matrix2[3] = matrix2[7] = matrix2[11] = 0.0;
	matrix2[15] = 1.0;
	//------------------
#ifdef BUILD_USE_GLHLIB
	MultiplyMatrices4by4OpenGL_FLOAT(resultMatrix, matrix, matrix2);
	glhTranslatef2(resultMatrix,
				-eyePosition3D[0], -eyePosition3D[1], -eyePosition3D[2]);
	//------------------
	memcpy(matrix, resultMatrix, 16*sizeof(float));
#else
	glMultMatrixf(matrix2);
	glTranslatef( -eyePosition3D[0], -eyePosition3D[1], -eyePosition3D[2] );
#endif
}


#ifdef SB_GLES

void gluPerspective( GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar )
{
   GLfloat xmin, xmax, ymin, ymax;

   ymax = zNear * tan(fovy * M_PI / 360.0);
   ymin = -ymax;
   xmin = ymin * aspect;
   xmax = ymax * aspect;

   glFrustumf( xmin, xmax, ymin, ymax, zNear, zFar );
}

void gluLookAt(
    GLfloat eyex, 
    GLfloat eyey, 
    GLfloat eyez, 
    GLfloat centerx, 
    GLfloat centery, 
    GLfloat centerz, 
    GLfloat upx, 
    GLfloat upy, 
    GLfloat upz)
{
	//ref: http://www.manpagez.com/man/3/gluLookAt/
	//ref: http://www.opengl.org/wiki/GluLookAt_code

	float matrix[16], 
		eyePosition3D[3] = {eyex, eyey, eyez }, 
		center3D[3] = {centerx, centery, centerz }, 
		upVector3D[3] = {upx, upy, upz };
#ifdef BUILD_USE_GLHLIB
	glhLoadIdentityf2(matrix);
	glhLookAtf2(matrix, eyePosition3D, center3D, upVector3D);
	glMultMatrixf(matrix);
#else
	glhLookAtf2(matrix, eyePosition3D, center3D, upVector3D);
#endif
}
#endif



#ifdef SB_ANDROID


/*
 * Matches a font ID with a SFG_StrokeFont structure pointer.
 * This was changed to match the GLUT header style.
 */
extern SFG_StrokeFont fgStrokeRoman;
extern SFG_StrokeFont fgStrokeMonoRoman;

static SFG_StrokeFont* fghStrokeByID( void* font )
{
    if( font == GLUT_STROKE_ROMAN      )
        return &fgStrokeRoman;
//    if( font == GLUT_STROKE_MONO_ROMAN )
	else
        return &fgStrokeMonoRoman;

//    fgWarning( "stroke font 0x%08x not found", font );
//    return 0;
}


#define _freeglut_return_if_fail(_x_) \
if(!_x_) \
{ \
	return;\
}
//	dbg_msg( "unexpected error: failed at %s \n", #_x_); \


void glutStrokeCharacter( void* fontID, int character )
{
	const SFG_StrokeChar *schar;
	const SFG_StrokeStrip *strip;
	int i, j;
	SFG_StrokeFont* font;
	font = fghStrokeByID( fontID );
	if ( character < 0 )
		{return;}

	if (character >= font->Quantity)
	{
		dbg_msg( "failed at: %c %d \n", character,character);		
		return;
	}
	//_freeglut_return_if_fail( character < font->Quantity );
	_freeglut_return_if_fail( font );

	schar = font->Characters[ character ];
	_freeglut_return_if_fail( schar );
	strip = schar->Strips;

#if 1
	glEnableClientState( GL_VERTEX_ARRAY );
	for( i = 0; i < schar->Number; i++, strip++ )
	{
		float v[128][2];
		const int ptCount = strip->Number < 128? strip->Number : 128;

		for( j = 0; j < ptCount; j++ )
		{
			v[ j ][0] = strip->Vertices[ j ].X;
			v[ j ][1] = strip->Vertices[ j ].Y;
		}

		glVertexPointer( 2, GL_FLOAT, 0, v );
		glDrawArrays( GL_LINE_STRIP, 0, ptCount );		
	}
	glDisableClientState( GL_VERTEX_ARRAY );

#else
	for( i = 0; i < schar->Number; i++, strip++ )
	{
		glBegin( GL_LINE_STRIP );
		for( j = 0; j < strip->Number; j++ )
			glVertex2f( strip->Vertices[ j ].X, strip->Vertices[ j ].Y );
		glEnd( );
		glBegin( GL_POINTS );
		for( j = 0; j < strip->Number; j++ )
			glVertex2f( strip->Vertices[ j ].X, strip->Vertices[ j ].Y );
		glEnd( );
	}
#endif

	glTranslatef( schar->Right, 0.0, 0.0 );
}

#endif

