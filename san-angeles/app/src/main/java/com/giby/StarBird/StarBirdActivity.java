/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This is a small port of the "San Angeles Observation" demo
 * program for OpenGL ES 1.x. For more details, see:
 *
 *    http://jet.ro/visuals/san-angeles-observation/
 *
 * This program demonstrates how to use a GLSurfaceView from Java
 * along with native OpenGL calls to perform frame rendering.
 *
 * Touching the screen will start/stop the animation.
 *
 * Note that the demo runs much faster on the emulator than on
 * real devices, this is mainly due to the following facts:
 *
 * - the demo sends bazillions of polygons to OpenGL without
 *   even trying to do culling. Most of them are clearly out
 *   of view.
 *
 * - on a real device, the GPU bus is the real bottleneck
 *   that prevent the demo from getting acceptable performance.
 *
 * - the software OpenGL engine used in the emulator uses
 *   the system bus instead, and its code rocks :-)
 *
 * Fixing the program to send less polygons to the GPU is left
 * as an exercise to the reader. As always, patches welcomed :-)
 */

package com.giby.StarBird;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.opengles.GL11;

//import java.lang.Thread;

//os, view stuff
import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.WindowManager;
import android.view.View;
import android.view.View.OnClickListener;

//input
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.EditorInfo;

//gesture
import android.view.GestureDetector;
//import android.view.RotationGestureDetector;
import android.support.v4.view.GestureDetectorCompat;

//debug
import android.util.Log;

// for testing texture with colors
//import android.graphics.Rect;
import android.graphics.Bitmap;


final class RotationGestureDetector {
	private static final int INVALID_POINTER_ID = -1;
	private float fX, fY, sX, sY;	//, focalX, focalY;
	private int ptrID1, ptrID2;
	private float mAngle;
	private boolean firstTouch;

	private OnRotationGestureListener mListener;

	public float getAngle() {
	    return mAngle;
	}

	public RotationGestureDetector(OnRotationGestureListener listener){
	    mListener = listener;
	    ptrID1 = INVALID_POINTER_ID;
	    ptrID2 = INVALID_POINTER_ID;
	}

	public boolean onTouchEvent(MotionEvent event){
		boolean isRotated = false;
	    switch (event.getActionMasked()) {
	        case MotionEvent.ACTION_DOWN:
	            sX = event.getX();
	            sY = event.getY();
	            ptrID1 = event.getPointerId(0);
	            mAngle = 0;
	            firstTouch = true;
	            break;
	        case MotionEvent.ACTION_POINTER_DOWN:
	            fX = event.getX();
	            fY = event.getY();
//	            focalX = getMidpoint(fX, sX);
//	            focalY = getMidpoint(fY, sY);
	            ptrID2 = event.getPointerId(event.getActionIndex());
	            mAngle = 0;
	            firstTouch = true;
	            break;
	        case MotionEvent.ACTION_MOVE:

	            if(ptrID1 != INVALID_POINTER_ID && ptrID2 != INVALID_POINTER_ID){
	                float nfX, nfY, nsX, nsY;
	                nsX = event.getX(event.findPointerIndex(ptrID1));
	                nsY = event.getY(event.findPointerIndex(ptrID1));
	                nfX = event.getX(event.findPointerIndex(ptrID2));
	                nfY = event.getY(event.findPointerIndex(ptrID2));
	                if (firstTouch) {
	                    mAngle = 0;
	                    firstTouch = false;
	                } else {
	                    mAngle = angleBetweenLines(fX, fY, sX, sY, nfX, nfY, nsX, nsY);
	                }

	                if (mListener != null) {
	                    mListener.onRotation(this);
	                    isRotated = true;
	                }
	                fX = nfX;
	                fY = nfY;
	                sX = nsX;
	                sY = nsY;
	            }
	            break;
	        case MotionEvent.ACTION_UP:
	            ptrID1 = INVALID_POINTER_ID;
	            break;
	        case MotionEvent.ACTION_POINTER_UP:
	            ptrID2 = INVALID_POINTER_ID;
	            break;
	    }

        if ( !isRotated && mListener != null) {
            mListener.onNoInput();
        }
	    return true;
	}

	private float getMidpoint(float a, float b){
	    return (a + b) / 2;
	}

	float findAngleDelta( float angle1, float angle2 )
	{
	    float From = ClipAngleTo0_360( angle2 );
	    float To   = ClipAngleTo0_360( angle1 );

	    float Dist  = To - From;

	    if ( Dist < -180.0f )
	    {
	        Dist += 360.0f;
	    }
	    else if ( Dist > 180.0f )
	    {
	        Dist -= 360.0f;
	    }

	    return Dist;
	}

	float ClipAngleTo0_360( float Angle ) { 
	    return Angle % 360.0f; 
	}

	private float angleBetweenLines (float fx1, float fy1, float fx2, float fy2, float sx1, float sy1, float sx2, float sy2)
	{
	       float angle1 = (float) Math.atan2( (fy1 - fy2), (fx1 - fx2) );
	       float angle2 = (float) Math.atan2( (sy1 - sy2), (sx1 - sx2) );

	       return findAngleDelta((float)Math.toDegrees(angle1),(float)Math.toDegrees(angle2));
	}

	public static interface OnRotationGestureListener {
	    public boolean onRotation(RotationGestureDetector rotationDetector);
	    public void onNoInput();
	}
}

public class StarBirdActivity extends Activity {
    @Override
	protected void onCreate(Bundle savedInstanceState) { 
    	Log.v( TAG, "Activity.onCreate()" );

    	//
    	super.onCreate(savedInstanceState);

    	//
		if ( !requestWindowFeature(android.view.Window.FEATURE_NO_TITLE) )
		{
			//setFeatureInt(android.view.Window.FEATURE_NO_TITLE);
			setTitle("Starbird (for Android 2.2 or above)");
		    setTitleColor(0);
		}
		//if ( requestWindowFeature(android.view.Window.PROGRESS_VISIBILITY_OFF) )
		{
		}
		if ( !this.getPackageManager().hasSystemFeature( android.content.pm.PackageManager.FEATURE_TOUCHSCREEN_MULTITOUCH_DISTINCT ) )
		{
			Log.e(TAG, "The device does not support multi-touch wtih support of distinct fingers!" );
		}
		//
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
 //       getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN);

        // use onLoad() now
        //nativeStartApp();
        
        // set layout
        setContentView( R.layout.main );
//		mGLView = new DemoGLSurfaceView(this);
//		setContentView(mGLView);
    
        mGLView =
            (DemoGLSurfaceView) findViewById(R.id.glsurfaceview);

		// setup PauseMenu
        mVictimContainer = findViewById(R.id.hidecontainer);
        mVictim1 = findViewById(R.id.backtogame);
        mVictim1.setOnClickListener( mGoneListener );
        mVictim2 = findViewById(R.id.backtotitle);
        mVictim2.setOnClickListener( mGoneListener );
        mVictim3 = findViewById(R.id.quitgame);
        mVictim3.setOnClickListener( mGoneListener );
    }

    @Override
    protected void onRestart() {
        super.onRestart();
    	Log.v(TAG, "Activity.onRestart()");
    }

    @Override
    protected void onPause() {
        super.onPause();
    	Log.v( TAG, "Activity.onPause()" );

		if (!isFinishing()) {   // use this check when Quit button is pushed
			pauseAll();
		}
    }

    @Override
    protected void onResume() {
    	super.onResume();
    	Log.v( TAG, "Activity.onResume()" );
        mGLView.onResume();
        
//        if (mPauseViewVisibility)
        {
    		setMenuVisibility(mPauseViewVisibility);
        }
    }

    @Override
    protected void onDestroy()
    {
    	//TODO call nativeOnQuit? on VirutalGameKey Quit
    	Log.v( TAG, "Activity.OnDestroy(): isByUser: " + isFinishing() );
    	super.onDestroy();
    }

	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		if ( keyCode == KeyEvent.KEYCODE_BACK )
		{
			pauseAll();
			return true;
		}
		else if ( keyCode == KeyEvent.KEYCODE_MENU )
		{
			pauseAll();
			//DEBUG: toggle light
//			DemoGLSurfaceView.nativeOnVirtualGameKeyEvent( DemoGLSurfaceView.VirtualGameKey.TOGGLE_LIGHT.getValue(), (char)1 );
		}

		//use system default
		return super.onKeyDown(keyCode, event);
	}

    OnClickListener mGoneListener = new OnClickListener() {
        public void onClick(View v)
        {
    		setMenuVisibility(false);

        	if ( v==mVictim1 || v==mVictim2 )
        	{                
                if (v==mVictim1) // Back to current State
                {
					// unpause it
                	DemoGLSurfaceView.nativePause(false);
                }
                else // move to Title
                {
                    DemoGLSurfaceView.nativeOnVirtualGameKeyEvent( DemoGLSurfaceView.VirtualGameKey.GO_TITLE.getValue(), (char)1 );
                }
                onResume();        		
        	}
        	else if (v==mVictim3) // QUIT
        	{
				// unpause it
				//DemoGLSurfaceView.nativePause(false);

				// move to Title
                DemoGLSurfaceView.nativeOnVirtualGameKeyEvent( DemoGLSurfaceView.VirtualGameKey.GO_TITLE.getValue(), (char)1 );
        		finish();  // it just stops the activity! instead of killing the activity!
        	}
        }
    };

    private void pauseAll() {
        // pause the game
        mGLView.onPause();
        DemoGLSurfaceView.nativePause(true);

        // show the PauseMenu
        setMenuVisibility(true);
    }

    private void setMenuVisibility(boolean v)
    {
		Log.d(TAG, "setVIs: vis:" + v );
    	final int state = v ? View.VISIBLE : View.GONE;
		mVictim1.setVisibility( state );
        mVictim2.setVisibility( state );
        mVictim3.setVisibility( state );
        mVictimContainer.setVisibility( state );
        mPauseViewVisibility = v;
    }
    
    // main view
    private DemoGLSurfaceView mGLView;
    
    // the pause menu
    private View mVictimContainer;
    private View mVictim1;
    private View mVictim2;
    private View mVictim3;
    //private View mDebugFlag;	//ForDebug
    private static boolean mPauseViewVisibility = false;

    //static stuff
	private static final String TAG = "StarBActivity";
	
    static {
    	Log.v(TAG, "load SB lib");
        System.loadLibrary("com.giby.StarBirdLib");		// which will call onLoad()
    	//System.loadLibrary("AndroidApk");
        //PackageManager.getPackageInfo(vCode,vStr);
    }
    
//    public static native void nativeStartApp();
//    public static native void nativeEndApp();
}


class DemoGLSurfaceView extends GLSurfaceView {
	

    /**
     * Constructor.  This version is only needed if you will be instantiating
     * the object manually (not from a layout XML file).
     * @param context
     */
    public DemoGLSurfaceView(Context context) 
    {
		//memo: by default GLSurfaceView tries to find a surface that's as close as possible to a 16-bit RGB frame buffer with a 16-bit depth buffer.
        super(context);
        initView(context);
    }
        
    public void initView(Context context) {

		setDebugFlags(DEBUG_CHECK_GL_ERROR | DEBUG_LOG_GL_CALLS);
		//super.setPreserveEGLContextOnPause(true);	//for API level 11
        
        //
//     // Create an empty, mutable bitmap
//        Bitmap bitmap = Bitmap.createBitmap(256, 256, Bitmap.Config.ARGB_4444);
//        // get a canvas to paint over the bitmap
//        Canvas canvas = new Canvas(bitmap);
//        bitmap.eraseColor(0);
//
//        // get a background image from resources
//        // note the image format must match the bitmap format
//        Drawable background = context.getResources().getDrawable( android.R.drawable.btn_default);
//        background.setBounds(0, 0, 256, 256);
//        background.draw(canvas); // draw the background to our bitmap
////TODO: clean up this part?
//        // Draw the text
//        Paint textPaint = new Paint();
//        textPaint.setTextSize(32);
//        textPaint.setAntiAlias(true);
//        textPaint.setARGB(0xff, 0x00, 0x00, 0x00);
//        // draw the text centered
//        canvas.drawText("Hello World", 16,112, textPaint);

		
//		setEGLConfigChooser(); 
		
//        mRenderer = new DemoRenderer(bitmap);
        mRenderer = new DemoRenderer(null);
        setRenderer(mRenderer);
        setFocusable(true);
        setFocusableInTouchMode(true);
        
        //
        mDetector = new GestureDetectorCompat(context, new MyGestureListener());
        mDetector.setIsLongpressEnabled(false);
        mRotationDetector = new RotationGestureDetector( new MyRotationGestureListener());
    }

    /**
     * Construct object, initializing with any attributes we understand from a
     * layout file. These attributes are defined in
     * SDK/assets/res/any/classes.xml.
     * 
     * @see android.view.View#View(android.content.Context, android.util.AttributeSet)
     */
    public DemoGLSurfaceView(Context context, android.util.AttributeSet attrs) {
        super(context, attrs);
        initView(context);
    }

    // tried to hide IME by the two methods below but it doesn't work for my DesireZ
	@Override
	public boolean onCheckIsTextEditor()
	{
		return false;
	}
	
	@Override
	public InputConnection onCreateInputConnection(EditorInfo outAttrs)
	{
		return null;
	}

	
	//TODO: for resume playing of sound. ref: http://android-developers.blogspot.jp/2011/11/making-android-games-that-play-nice.html
//    @Override
//    protected void onFocusChanged (boolean gainFocus, int direction, Rect previouslyFocusedRect)
//    {
//    	if (!gainFocus)
//    	{
//    		//nativePause();
//    	}
//    	super.onFocusChanged(gainFocus, direction, previouslyFocusedRect);
//    	return;
//    }
    
    @Override
    public boolean onTouchEvent(final MotionEvent event) 
    {
		//int dummy=0;
    	{
    	//	dummy++;
    	}

    	if ( event.getPointerCount() == 1 )
    	{
        	//Log.v(TAG,"onTouchEvent: count:" + event.getPointerCount() + ", " + event.getX() + ", " + event.getY() + ", " + event.getAction() );
        	char states[] = { 0, 2, 1 };	// map ACTION_DOWN, ACTION_UP, ACTION_MOVE to "key" state
        	if ( event.getAction() >= MotionEvent.ACTION_DOWN && event.getAction() <= MotionEvent.ACTION_MOVE )
        	{
        		nativeOnTouchEvent( (char) event.getX(), (char) event.getY(), states[event.getAction()] );	
        	}    	
        	else if ( event.getAction() >= MotionEvent.ACTION_CANCEL )
        	{
        	}
        	//else event.getButtonState() 
        	//BAD?: 
        	//return super.onTouchEvent(event);
        	
       // 	if (event.getAction() == MotionEvent.ACTION_MOVE )	
        //		return true;
        	//else let mDetector do its job
    	}
    	else if ( event.getPointerCount() >= 3 )
    	{
    		//SPEED_DOWN
    		DemoGLSurfaceView.nativeOnVirtualGameKeyEvent( VirtualGameKey.SPEED_DOWN.getValue(), (char)0 );	// 0:keydown
    	}

    	//else
    	//only pass two-pointer event to it
    	{
    		this.mRotationDetector.onTouchEvent(event);
    		return this.mDetector.onTouchEvent(event);
    	}
    }

    @Override
    public boolean onTrackballEvent(final MotionEvent event) {
    	//super.onTrackballEvent(event);
    	//Log.v(TAG,"onTrackballEvent: " + event.getDownTime()+ ", " + event.getAction() );
    	Log.v(TAG,"onTrackballEvent:" + event.getX() + ", " + event.getY() + ", " + event.getAction() );
    	return true;
    }

    @Override
    public boolean onKeyMultiple(int keyCode, int repeatCount, KeyEvent event) {
    	//super.onKeyMultiple(keyCode,repeatCount,event);
    	Log.v(TAG,"onKeyMultiple: " + keyCode + ", " + event.getDownTime()+ ", " + event.getAction() );
    	if ( nativeOnKeyEvent(event.getKeyCode(), (char)2) )
        {
        	//return true;
        }
    	return false;
    }

    @Override
    public boolean dispatchKeyShortcutEvent(KeyEvent event) {
    	return super.dispatchKeyShortcutEvent(event);
    }
        
//queueing events is often the simplest way of dealing with cross-thread communication.
//  @Override
//	public boolean onKeyDown(int keyCode, KeyEvent event) {
//		if (keyCode == KeyEvent.KEYCODE_DPAD_CENTER) {
//			queueEvent(new Runnable() {
//				// This method will be called on the rendering
//				// thread:
//				public void run() {
//					mMyRenderer.handleDpadCenter();
//				}});
//			return true;
//		}
//		return super.onKeyDown(keyCode, event);
//	}

	public boolean onKeyDown(int keyCode, KeyEvent event){
		if ( keyCode == KeyEvent.KEYCODE_BACK )
		{
			//use system default
		}
		else if ( keyCode == KeyEvent.KEYCODE_MENU )
		{
//		   DemoGLSurfaceView.nativeOnVirtualGameKeyEvent( VirtualGameKey.TOGGLE_LIGHT.getValue(), (char)1 );
		}
		//DPAD
		else //if ( //keyCode >= KeyEvent.KEYCODE_DPAD_UP && keyCode <= KeyEvent.KEYCODE_DPAD_RIGHT
				//&& 
				//event.getAction() == MotionEvent.ACTION_DOWN 
				//) 
		{
			//Log.v(TAG,"onKeyDown:" + keyCode + ", " + event.getAction() );
            if ( nativeOnKeyEvent(event.getKeyCode(), (char)0 ) )
            {
            	return true;
            }
        }
		return super.onKeyDown(keyCode, event);
	}

	public boolean onKeyUp(int keyCode, KeyEvent event){
		if ( keyCode == KeyEvent.KEYCODE_BACK )
		{
			//use system default
		}
		else if ( keyCode == KeyEvent.KEYCODE_MENU )
		{
		}
		//DPAD
		else 
		{
			Log.v(TAG,"onKeyUp:" + keyCode + ", " + event.getAction() );
            if ( nativeOnKeyEvent(event.getKeyCode(), (char)1 ) )
            {
            	//return true;
            }
        }
		return super.onKeyUp(keyCode, event);
	}

	//====//====// gesture
    
    class MyGestureListener extends GestureDetector.SimpleOnGestureListener {
        private static final String DEBUG_TAG = "StarBInput: Gestures: ";

        @Override
        public boolean onDoubleTap(MotionEvent event)
        {
            Log.d(DEBUG_TAG,"onDoubleDown: " + event.toString());
//            DemoGLSurfaceView.nativeOnVirtualGameKeyEvent( VirtualGameKey.GO_TITLE.getValue(), (char)1 );
        	return true;
        }

        @Override
        public void onLongPress(MotionEvent event)
        {
            Log.d(DEBUG_TAG,"onLongPress: " + event.toString()); 
        	return;
        }

        @Override
        public boolean onDown(MotionEvent event) { 
            //Log.d(DEBUG_TAG,"onDown: " + event.toString()); 
            return true;
        }

        @Override
        public boolean onFling(MotionEvent event1, MotionEvent event2, 
                float speedX, float speedY) {
            Log.d(DEBUG_TAG, "onFling: distX:" + speedX + "  distY:" + speedY + "  "
            		+ event1.toString()+event2.toString());
            return true;
        }

        @Override
        public boolean onScroll(MotionEvent event1, MotionEvent event2, 
                float distX, float distY) {
//            Log.d(DEBUG_TAG, "onScroll: distX:" + distX + "  distY:" + distY + "  "
//            		+ (event1!=null?event1.toString():"nil") + "  " + event2.toString());
            return true;
        }
    }

    class MyRotationGestureListener
    	implements RotationGestureDetector.OnRotationGestureListener
    {
        private static final String DEBUG_TAG = "StarBInput: RotationGestures: ";
        
        MyRotationGestureListener()
        {
        	m_NoInputCounter = 0;
//        	mNativeWrapper = aWrapper;
        }
        
//    	@Override
        public boolean onRotation( RotationGestureDetector detector) 
    	{
            final float mRotationFactor = detector.getAngle();
            
            //Log.d(DEBUG_TAG, "onRotation: mRotationFactor:" + mRotationFactor );
            
            if ( Math.abs(mRotationFactor) < 0.7 )
            	return true;

            VirtualGameKey key = mRotationFactor > 0 ? VirtualGameKey.ROTATE_LEFT : VirtualGameKey.ROTATE_RIGHT;
            DemoGLSurfaceView.nativeOnVirtualGameKeyEvent( key.getValue(), (char)0 );

            m_NoInputCounter=0;
            return true;
        }
        
        public void onNoInput()
        {
        	if ( m_NoInputCounter < 30 )  // assumption: fps
        	{
            	m_NoInputCounter++;
        		return;
        	}
 
        	//release keys
            DemoGLSurfaceView.nativeOnVirtualGameKeyEvent( VirtualGameKey.ROTATE_LEFT.getValue(), (char)1 );
            DemoGLSurfaceView.nativeOnVirtualGameKeyEvent( VirtualGameKey.ROTATE_RIGHT.getValue(), (char)1 );
            DemoGLSurfaceView.nativeOnVirtualGameKeyEvent( VirtualGameKey.SPEED_DOWN.getValue(), (char)1 );	// 0:keydown
        }
        //DemoGLSurfaceView mNativeWrapper;
        private int m_NoInputCounter;
    }
    
    private GestureDetectorCompat mDetector; 		// as speed controller
    private RotationGestureDetector mRotationDetector;	// as rotation controller

    //====//====//

    enum VirtualGameKey
    {
    	ROTATE_LEFT(0),
    	ROTATE_RIGHT(1),
    	GO_TITLE(2),
    	TOGGLE_LIGHT(3),
    	SPEED_DOWN(4);
    	//QUIT_APP
    	
        private final int value;
        private VirtualGameKey(int value) {
            this.value = value;
        }

        public int getValue() {
            return value;
        }
    };


    public DemoRenderer mRenderer;

    public static native void nativePause( boolean isPause );

	private static native boolean nativeOnTouchEvent( char x, char y, char s );
	private static native boolean nativeOnKeyEvent(int keyCode, char x );
	public static native boolean nativeOnVirtualGameKeyEvent( int keyCode, char x );
	
	private static final String TAG = "StarBView";
}

class DemoRenderer implements GLSurfaceView.Renderer {

	public DemoRenderer(Bitmap bitmap)
	{
		mBitmap = bitmap;
	}

	public void finalize()
	{
        //Clean up
		if (mBitmap != null)
			mBitmap.recycle();
        
		Log.d(TAG,"DemoRenderer.finalize" );
		
		try {
			super.finalize();
		} catch (Throwable e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void onSurfaceCreated(GL10 gl, EGLConfig config) 
	{
		if ( gl instanceof GL11 )
		{
			Log.d( TAG, "DemoRenderer: it supports G11: " + gl.toString() + " " + config.toString() );
		}
		
	    String extensions = gl.glGetString(GL10.GL_EXTENSIONS);
	    Log.d( TAG, "DemoRenderer: extension: " + extensions);

	    //----//
		nativeInit();

/*
		if (true)return;
		if (mBitmap == null) return;

        //Generate one texture pointer...
        int textures[] = {0};
        gl.glGenTextures(1, textures, 0);
        //...and bind it to our array
        gl.glBindTexture(GL10.GL_TEXTURE_2D, textures[0]);

        //Create Nearest Filtered Texture
        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST);
        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR);

        //Different possible texture parameters, e.g. GL10.GL_CLAMP_TO_EDGE
        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S, GL10.GL_REPEAT);
        gl.glTexParameterf(GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T, GL10.GL_REPEAT);

        //Use the Android GLUtils to specify a two-dimensional texture image from our bitmap
        android.opengl.GLUtils.texImage2D(GL10.GL_TEXTURE_2D, 0, mBitmap, 0);
 */
        //TODOA1A??  call it here!?
        //mBitmap.recycle();
    }

    public void onSurfaceChanged(GL10 gl, int w, int h) {
        nativeResize(w, h);
        startTime = System.currentTimeMillis();
    }

    public void onDrawFrame(GL10 gl) {
		long endTime = System.currentTimeMillis();
		long dt = endTime - startTime;
		if (dt < 33)
		{
			try
			{
				Thread.sleep(33 - dt);
			}
			catch (InterruptedException e)
			{
				Log.e(TAG,"onDrawFrame: peroid:" + (33-dt) + "  Exception:" + e );
			}
		}
		startTime = System.currentTimeMillis();

    	//TODO UpdateGame(dt);
        nativeRender();
    	
    	//TODOA1 do sth.
    	//use textures[] = {0};
    }
    
    //
    long startTime; 
    Bitmap mBitmap;

    //
    
    private static final String TAG = "StarBRenderer";
    //
    private static native void nativeInit();
    private static native void nativeResize(int w, int h);
    private static native void nativeRender();
    
    private static native void nativeDone();
}