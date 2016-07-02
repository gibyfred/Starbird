#if 1
/* San Angeles Observation OpenGL ES version example
 * Copyright 2009 The Android Open Source Project
 * All rights reserved.
 *
 * This source is free software; you can redistribute it and/or
 * modify it under the terms of EITHER:
 *   (1) The GNU Lesser General Public License as published by the Free
 *       Software Foundation; either version 2.1 of the License, or (at
 *       your option) any later version. The text of the GNU Lesser
 *       General Public License is included with this source in the
 *       file LICENSE-LGPL.txt.
 *   (2) The BSD-style license that is included with this source in
 *       the file LICENSE-BSD.txt.
 *
 * This source is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files
 * LICENSE-LGPL.txt and LICENSE-BSD.txt for more details.
 */
#include <jni.h>
#include <sys/time.h>w
#include <time.h>
#include <android/log.h>
#include <stdint.h>
#include "System.h"

#include <android/api-level.h>
//#include <android/input.h>



//--------------------------------------//
void Engine_UpdateFPS( float );
void Engine_UpdateMessage( char* );
void Engine_SetPopup( int type, bool isShow );
static void Engine_ShowUI();

//--------------------------------------//
int   gAppAlive   = 1;

static int  sWindowWidth  = 320;
static int  sWindowHeight = 480;
static int  sDemoStopped  = 0;
static long sTimeOffset   = 0;
static int  sTimeOffsetInit = 0;
static long sTimeStopped  = 0;

static jobject* s_ActivityClass = NULL;
static JavaVM* sJvm = NULL;
static JNIEnv* s_jni = NULL;

//--------------------------------------//
static long
_getTime(void)
{
    struct timeval  now;

    gettimeofday(&now, NULL);
    return (long)(now.tv_sec*1000 + now.tv_usec/1000);
}

/**/
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	__android_log_print(ANDROID_LOG_INFO, "StarB:OnLoad:", "My JNI_OnLoad: %d, %d, %d" );
	sJvm = vm;
	SBInitApp();

	//
//	JNIEnv* env;
//	if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
//		return -1;
//	}
	return JNI_VERSION_1_2;     // use 1_6 instead?
}


/* Call to initialize the graphics state */
void
Java_com_giby_StarBird_DemoRenderer_nativeInit( JNIEnv* env )
{
	__android_log_print(ANDROID_LOG_INFO, "StarBNative:", "nativeInit()" );

    //importGLInit();
    SBInit();
    gAppAlive    = 1;
    sDemoStopped = 0;
    sTimeOffsetInit = 0;

	//----// calling to Java side
	jclass localClass = (*env)->FindClass( env, "com/giby/StarBird/StarBirdActivity");
//	jclass localClass = (*env)->FindClass( env, "StarBirdActivity");
	//jclass globalClass = reinterpret_cast<jclass>( env->NewGlobalRef(localClass));
	s_ActivityClass = (*env)->NewGlobalRef(env, localClass);

	//memo: teapot use GetObjectClass( activityClass )

	//
	Engine_ShowUI();
//	Engine_UpdateFPS(10101.f);  //Debug Only
}

void
Java_com_giby_StarBird_DemoRenderer_nativeResize( JNIEnv* env, jobject thiz, jint w, jint h )
{
    sWindowWidth  = w;
    sWindowHeight = h;
	SBOnResizeViewport(w,h);
    __android_log_print(ANDROID_LOG_INFO, "StarBNative:", "resize w=%d h=%d", w, h);
	//Engine_ShowUI();
}

/* Call to finalize the graphics state */
void
Java_com_giby_StarBird_DemoRenderer_nativeDone( JNIEnv* env )
{
	__android_log_print(ANDROID_LOG_INFO, "StarBNative:", "nativeDone()" );

	//----// calling to Java side
	s_ActivityClass = NULL;

	//TODO?   FreeGlobalRef

	//---//
    SBDeinit();
    //importGLDeinit();
}

/*
 */
//jboolean
void 
Java_com_giby_StarBird_DemoGLSurfaceView_nativeOnTouchEvent( JNIEnv* env, jobject thiz,
	jchar x, jchar y, jchar state )
{
	//jboolean b = 
	SBOnTouchEvent(x, y , state);

#if 0
//#if __ANDROID_API__ >= 9 //testing
	jint count = AMotionEvent_getPointerCount();
	if (count > 1)
		__android_log_print(ANDROID_LOG_INFO, "StarBT:", "nativeOnTouch: %d, %d, %d", x,y,state );
#endif
	return;
}

/*
 */
jboolean
Java_com_giby_StarBird_DemoGLSurfaceView_nativeOnKeyEvent( JNIEnv* env, jobject thiz, jint key, jchar state )
{
	jboolean b = SBOnKeyEvent(key, state);
	//__android_log_print(ANDROID_LOG_INFO, "StarBK:", "onKeyDown: ret=%d", b );
	return b;
}

/*
 */
jboolean
Java_com_giby_StarBird_DemoGLSurfaceView_nativeOnVirtualGameKeyEvent( JNIEnv* env, jobject thiz, jint key, jchar state )
{
	jboolean b = SBOnVirtualGameKeyEvent( key, state);
	//__android_log_print(ANDROID_LOG_INFO, "StarBK:", "onKeyDown: ret=%d", b );
	return b;
}


/* This is called to indicate to the render loop that it should
 * stop as soon as possible.
 */
void
Java_com_giby_StarBird_DemoGLSurfaceView_nativePause( JNIEnv* env, jobject thiz, jboolean isPause )
{
    sDemoStopped = !sDemoStopped;
    if (sDemoStopped) {
        /* we paused the animation, so store the current
         * time in sTimeStopped for future nativeRender calls */
        sTimeStopped = _getTime();
    } else {
        /* we resumed the animation, so adjust the time offset
         * to take care of the pause interval. */
        sTimeOffset -= _getTime() - sTimeStopped;
    }

	//Java_com_giby_StarBird_DemoGLSurfaceView_nativeOnKeyDown(env, (int)'p');
	SBOnPause( isPause == JNI_TRUE );
}

/* Call to render the next GL frame */
void
Java_com_giby_StarBird_DemoRenderer_nativeRender( JNIEnv* env )
{
	//
    long   curTime;

    /* NOTE: if sDemoStopped is TRUE, then we re-render the same frame
     *       on each iteration.
     */
    if (sDemoStopped) {
        curTime = sTimeStopped + sTimeOffset;
    } else {
        curTime = _getTime() + sTimeOffset;
        if (sTimeOffsetInit == 0) {
            sTimeOffsetInit = 1;
            sTimeOffset     = -curTime;
            curTime         = 0;
        }
    }

    //__android_log_print(ANDROID_LOG_INFO, "StarBX", "curTime=%ld", curTime);

	//----//

	// update message
	//version 1 - good for debug ONLY
	//Engine_UpdateFPS(getMainMessageF());

	//version 2
	Engine_UpdateMessage(getMainMessage());

	// main "game loop"
    SBDrawMain(curTime);//, sWindowWidth, sWindowHeight);
}

/*
-g -I"O:\app\android-ndk\platforms\android-9\arch-arm\usr\include" -I"O:\app\android-ndk\sources\cxx-stl\gnu-libstdc++\include" -I"O:\app\android-ndk\sources\cxx-stl\gnu-libstdc++\libs\armeabi\include" -D"ANDROID_NDK" -D"ANDROID" -D"__ANDROID__" -D"__ARM_EABI__" -D"__ARM_ARCH_5__" -D"__ARM_ARCH_5T__" -D"__ARM_ARCH_5E__" -D"__ARM_ARCH_5TE__" -Wno-psabi -O0 -marm -fno-strict-aliasing -funswitch-loops -finline-limit=100 -fomit-frame-pointer -D"DISABLE_IMPORTGL" -fno-exceptions -fpic -fstack-protector -fno-rtti -fno-short-enums -o "Android\Debug\app-android.o" 
*/



static JNIEnv* beginJNIEnv()
{
//	app_->activity->vm->AttachCurrentThread( &jni, NULL );

	JNIEnv *env;
	int status = (*sJvm)->GetEnv(sJvm, (void**)&env, JNI_VERSION_1_6);
	if ( status < 0 )
	{
		status = (*sJvm)->AttachCurrentThread( sJvm, &env, NULL);
		if(status < 0) {
			return;
		}
	}

	if ( s_jni != env )
	{
//		__android_log_print(ANDROID_LOG_INFO, "StarB: WTF", "status: %d", status );
	}

	s_jni = env;

	return env;
}

static JNIEnv* endJNIEnv()
{
	//	app_->activity->vm->DetachCurrentThread();
//	JNIEnv *env = s_jni;
//	if ( env )
	{
//		(*sJvm)->DetachCurrentThread( sJvm );
	}
//	s_jni = NULL;
}

static jobject* sGetActivityClass()
{
	return s_ActivityClass;
}

/*
 * for PopupWindow
 */
void Engine_ShowUI()
{
	beginJNIEnv();

	if ( sGetActivityClass() == NULL || s_jni == NULL )
	{
		endJNIEnv();
		return;
	}

	//Default class retrieval
//	jclass clazz = (*s_jni)->GetObjectClass( s_jni, sGetActivityClass() );

	// this part doesn't work now, because it calls the non-static method with class instead of class instance
#if 0
	jclass clazz = sGetActivityClass();
	__android_log_print(ANDROID_LOG_INFO, "StarBX1", "clazz=%p", clazz);
	jmethodID methodID = (*s_jni)->GetMethodID( s_jni, clazz, "showUI", "()V" );
	__android_log_print(ANDROID_LOG_INFO, "StarBX2", "methodID=%p", methodID);
	(*s_jni)->CallVoidMethod( s_jni, sGetActivityClass(), methodID );
#endif

	endJNIEnv();
	return;
}

/*
 * for PopupWindow
 */
void Engine_UpdateFPS( float fFPS )
{
	beginJNIEnv();

	if ( sGetActivityClass() == NULL || s_jni == NULL )
	{
		endJNIEnv();
		return;
	}

#if 1
	// call the static method
	jclass clazz = sGetActivityClass();
	//__android_log_print(ANDROID_LOG_INFO, "StarBX1", "clazz=%p", clazz);
	jmethodID methodID = (*s_jni)->GetStaticMethodID( s_jni, clazz, "updateFPS", "(F)V" );
	//__android_log_print(ANDROID_LOG_INFO, "StarBX2", "methodID=%p", methodID);
	(*s_jni)->CallStaticVoidMethod( s_jni, sGetActivityClass(), methodID, fFPS );
#endif

	endJNIEnv();
	return;
}

/*
void Engine_UpdateMessage( char* str )
{
	beginJNIEnv();

	if ( sGetActivityClass() == NULL || s_jni == NULL )
	{
		endJNIEnv();
		return;
	}

#if 1
	//----// call the static method
	jclass clazz = sGetActivityClass();

	// Construct a String
	jstring jstr = (*s_jni)->NewStringUTF( s_jni, str );

	//__android_log_print(ANDROID_LOG_INFO, "StarCX1", "clazz=%p", clazz);
	jmethodID methodID = (*s_jni)->GetStaticMethodID( s_jni, clazz, "updateMessage", "(Ljava/lang/String;)V" );
//	__android_log_print(ANDROID_LOG_INFO, "StarCX2", "methodID=%p", methodID);
	(*s_jni)->CallStaticVoidMethod( s_jni, sGetActivityClass(), methodID, jstr );

#endif

	endJNIEnv();
	return;
}
*/

void Engine_UpdateMessage( char* str )
{
	beginJNIEnv();

	if ( sGetActivityClass() == NULL || s_jni == NULL )
	{
		endJNIEnv();
		return;
	}

#if 1
	//----// call the static method
	jclass clazz = sGetActivityClass();

	// Construct a String
	jstring jstr = (*s_jni)->NewStringUTF( s_jni, str );

	//__android_log_print(ANDROID_LOG_INFO, "StarCX1", "clazz=%p", clazz);
	jmethodID methodID = (*s_jni)->GetStaticMethodID( s_jni, clazz, "updateMessage", "(Ljava/lang/String;)V" );
//	__android_log_print(ANDROID_LOG_INFO, "StarCX2", "methodID=%p", methodID);
	(*s_jni)->CallStaticVoidMethod( s_jni, sGetActivityClass(), methodID, jstr );

#endif

	endJNIEnv();
	return;
}

void Engine_SetPopup( int popUpType, bool isShow )
{
	beginJNIEnv();

	if ( sGetActivityClass() == NULL || s_jni == NULL )
	{
		endJNIEnv();
		return;
	}

#if 1
	//----// call the static method
	jclass clazz = sGetActivityClass();

	//__android_log_print(ANDROID_LOG_INFO, "StarDX1", "clazz=%p", clazz);
	jmethodID methodID = (*s_jni)->GetStaticMethodID( s_jni, clazz, "setPopup", "(II)V" );
	//__android_log_print(ANDROID_LOG_INFO, "StarDX2", "methodID=%p", methodID);
	(*s_jni)->CallStaticVoidMethod( s_jni, sGetActivityClass(), methodID, popUpType, (int)isShow );

#endif

	endJNIEnv();
	return;
}

#endif

