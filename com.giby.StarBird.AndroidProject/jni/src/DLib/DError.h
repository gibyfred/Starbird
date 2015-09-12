
/*=====================================

 file: DError.h

 Owner: Hui Tin Yuen, Fred

 Description:

 - contains error printing routine for VC and OpenGL

=====================================*/

//#pragma once


#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////
// error handling
//////////////////////////////////////////////////////////////////////

// See if there is an error and trace it
extern void dbg_msg(const char* lpszFormat, ...);
extern void traceGLStack();
extern bool GetOpenGLError();

#ifdef __cplusplus
}
#endif
