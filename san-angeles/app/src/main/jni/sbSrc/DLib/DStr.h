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

#pragma once

#include "DPlatform.h"

#ifdef SB_WIN
#define Dfscanf fscanf_s
#define Dsprintf sprintf_s
#define Dstrcpy strcpy_s
#else
#define Dfscanf fscanf
#define Dsprintf sprintf
#define Dstrcpy strcpy
#endif