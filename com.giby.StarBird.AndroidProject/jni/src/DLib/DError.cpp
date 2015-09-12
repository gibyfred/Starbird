
/*=====================================

 file: DError.cpp

 Owner: Hui Tin Yuen, Fred

 Description:

 - contains error printing routine for VC and OpenGL

=====================================*/

#include <stdio.h>
#include <assert.h>
#include "DGL.h"


#ifdef SB_ANDROID
#include <android/log.h>
#endif


//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////

#define _COUNT_OF(array) (sizeof(array)/sizeof(array[0]))

void dbg_msg( const char* lpszFormat, ...)
{
#ifdef SB_DEBUG

#ifdef SB_WIN
//	GetOpenGLError();	//only work if we have the GL context...
#else

#endif

	va_list args;
	va_start(args, lpszFormat);

	int nBuf;
	char szBuffer[512];

#ifdef SB_WIN
	nBuf = _vsnprintf_s(szBuffer, _COUNT_OF(szBuffer), lpszFormat, args);
	assert(nBuf >= 0);
	OutputDebugStringA((LPCSTR)szBuffer);		// win32 call
#else
	nBuf = vsnprintf(szBuffer, _COUNT_OF(szBuffer), lpszFormat, args);
	assert(nBuf >= 0);
	__android_log_print(ANDROID_LOG_INFO, "StarBDbMsg", "%s", szBuffer);
#endif
	
	va_end(args);
#endif
}


//////////////////////////////////////////////////////////////////////
// OpenGL errors
//////////////////////////////////////////////////////////////////////

// print stack depths for debug
void traceGLStack()
{
	int d1, d2, d3=0;
	glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &d1);
	glGetIntegerv(GL_PROJECTION_STACK_DEPTH, &d2);		// if necessary: add GL_TEXTURE_STACK_DEPTH
#ifdef SB_WIN
	glGetIntegerv(GL_NAME_STACK_DEPTH, &d3);
#endif

	dbg_msg("> SS stack depths(mv,pj,nm): %d, %d, %d\n", d1, d2, d3);
}


// See if there is an error and trace it
bool GetOpenGLError()
{
	//SPEEDUP return in release build
    GLenum err=glGetError();

	if (err==GL_NO_ERROR)
		return false;

    //LPCSTR ErrorStr;
	const char* ErrorStr;

    switch (err)
    {
       case GL_NO_ERROR         : break;		// NO ERROR
       case GL_INVALID_ENUM     : ErrorStr="GL_INVALID_ENUM"     ; break;
       case GL_INVALID_VALUE    : ErrorStr="GL_INVALID_VALUE"    ; break;
       case GL_INVALID_OPERATION: ErrorStr="GL_INVALID_OPERATION"; break;
       case GL_STACK_OVERFLOW   : ErrorStr="GL_STACK_OVERFLOW"   ; break;
       case GL_STACK_UNDERFLOW  : ErrorStr="GL_STACK_UNDERFLOW"  ; break;
       case GL_OUT_OF_MEMORY    : ErrorStr="GL_OUT_OF_MEMORY"    ; break;
       default                  : ErrorStr="<Unknown>";assert(0) ; break; 
    }

#ifdef SB_ANDROID
	const char* pChar = "";	 
	//const char* pChar = gluErrorString(err);	//TODOF: use glues http://code.google.com/p/glues/ 
	dbg_msg("GL error #(0x%x): %s \r\n", err, ErrorStr) ;
#else
	const char* pChar = gluErrorStringWIN(err);
	dbg_msg("GL error #(0x%x): %s \r\n", err, pChar) ;
#endif
	
	//1026 TRACE1("OGL's glGetError() returned error %s\n",(LPCSTR)Error);

return true;
	if ( err == GL_STACK_OVERFLOW )
	{
		traceGLStack();
		GetOpenGLError();		// test once more
	}

	return true;
}
