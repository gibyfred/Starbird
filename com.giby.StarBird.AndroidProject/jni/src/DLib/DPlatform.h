//
// Dlib - 
// 
// 
// Owner: Hui Tin Yuen, Fred
// 
// Description:
// 
// include 
//

#pragma once


//----//
#ifdef ANDROID_NDK

#define SB_ANDROID
#define SB_GLES			// It is OpenGL ES, instead of OpenGL!
#define SB_BUILD_VIRTUAL_PAD


//----//
#elif defined(WIN32)

#define SB_WIN
#define SB_GLES			// only use GL commands available in GLES
#define SB_BUILD_VIRTUAL_PAD
//#define SB_BUILD_TRACKBALL

//----//
#else

#define SB_IRIS

#endif



#ifndef NDEBUG			// Android/ndk defines it!!!
#define SB_DEBUG
#else
//TESTxxx
#endif


typedef signed short S16;

