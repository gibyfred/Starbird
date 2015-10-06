//
// DLib - for win32 app.
//
// 
// Owner: Hui Tin Yuen, Fred
// 
// Description:
// 
// support error reporting for win32 program developed in VC++
//


#ifndef DW32_H
#define DW32_H


/////////////////////////////////////////////////////////////////////
// For using win32 api without MFC.
// Some .cpp files should include this intead of *afx.h 
// if no mfc stuff is needed.

#include <windows.h>
#include "DError.h"
// for win32 only:
//#include <windef.h>
//#include <wingdi.h>	// defined WINGDIAPI

#endif
