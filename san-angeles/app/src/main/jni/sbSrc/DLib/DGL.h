//
// Dlib - includes gl headers
// 
// 
// Owner: Hui Tin Yuen, Fred
// 
// Description:
// 
// include basic libraries for application which use ogl;  support MFC and win32
//

#ifndef DGL_H
#define DGL_H

#include "DPlatform.h"



#ifdef __cplusplus
//extern "C" {
#endif

#ifndef __cplusplus
//typedef unsigned char bool;
#endif


//-----------------------------------------------------------------------------//
// To link you will need opengl32.lib, glu32.lib.
// You will need opengl32.dll and glu32.dll to run the application too.
//-----------------------------------------------------------------------------//

/////////////////////////////////////////////////////////////////////
// includes gl headers
//
// note: since this file include <windows.h> for mfc app but mfc doesn't allow it.
// so please include your mfc file header *before* include this file.
//

// note: gl/gl.h need definition of WINGDIAPI, etc

#ifdef SB_ANDROID

#define SB_GLES1        // use GL ES v1.x

#elif defined(SB_WIN)


#ifndef _AFXDLL		//TODOA8 should include other mfc version like _AFXSTATIC
//------------------- WIN32 ---------------------//
// for win32 non-mfc application

#include "DWin32.h"

#else
//------------------- MFC ---------------------//
// for mfc application
#define D_USE_MFC			// designed for DImage //TODOF4 this method should be improved
// useful for testing: #pragma message("D_USE_MFC defined")

#ifdef _AFXDLL
#include <windows.h>		// Since it assumes that
								// MFC user of this file have to include stdafx.h *before*
								// this file.  This include is useful for some cpp files
								// which don't use MFC stuffs.
								// This method should be improved.

//#include "stdafx.h"	// it slows things down, we should have put this in .cpp.
#endif

#endif

#endif
//------------------- END #if ---------------------//




//----------------------------------------// include headers and "simulate" common functions not in GLES

#ifdef SB_ANDROID

#ifdef SB_GLES1
// use OGL ES 1
#include <GLES/gl.h>
//#include <GLES/glext.h>
//#pragma message (">hello a1")
#else
// use OGL ES 2.x
//#include <EGL/egl.h>
#include <GLES2/gl2.h>
//#include <GLES2/gl2ext.h>
#endif

#else
#include <gl/gl.h>			// Include the standard OpenGL headers
#include <gl/glu.h>			// Include the OpenGL utility library
#endif // SB_ANDROID

#include "DError.h"


//----------------------------------------//

#define MAX_NAME_LENGTH 512

#ifndef M_PI
#define M_PI 3.1415926536
#endif

#define RAND2DEG(r)			((r)*180/M_PI)
#define DEG2RAND(r)			((r)*M_PI/180)

// global definiable constansts for this glctrl library
//D_FOR_SMC_1_0		// calibration, etc
//D_USE_CVIEW_AS_BASECLASS_OF_OGLVIEW


#pragma warning (disable:4244)		/* disable bogus conversion warnings */


//----------------------------------------//
//gl util and glut -like stuff


inline void dglOrtho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
#ifdef SB_WIN
	glOrtho(left, right, bottom, top, zNear, zFar);
#else
#ifdef SB_GLES1
	glOrthof(left, right, bottom, top, zNear, zFar);
#else
	//use GLM library instead??
	//TODOGLES2?
#endif
#endif
}

#ifdef SB_GLES
typedef GLfloat DFloats[];



#ifdef SB_WIN

#define glFrustumf glFrustum

#else
/*
 * Draw a stroke character
 */
#define GLUT_STROKE_ROMAN 0
void glutStrokeCharacter( void* fontID, int character );

#endif //SB_WIN


inline void glColor3fv(DFloats v)
{
#ifdef SB_GLES1
	glColor4f(v[0], v[1], v[2], 1.0f );
#else
#endif
}

extern void gluPerspective( GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar );

extern void gluLookAt(
    GLfloat eyex, 
    GLfloat eyey, 
    GLfloat eyez, 
    GLfloat centerx, 
    GLfloat centery, 
    GLfloat centerz, 
    GLfloat upx, 
    GLfloat upy, 
    GLfloat upz);

#endif  // SB_GLES


#endif


#ifdef __cplusplus
//}
#endif

