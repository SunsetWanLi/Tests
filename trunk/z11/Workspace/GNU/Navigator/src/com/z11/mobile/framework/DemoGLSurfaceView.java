package com.z11.mobile.framework;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;

import com.cgrj.wenzhoutong.CityFetcherThread;
import com.cgrj.wenzhoutong.MapThread;
import com.cgrj.wenzhoutong.TrafficThread;
import com.cgrj.wenzhoutong.WeatherThread;

class DemoGLSurfaceView extends GLSurfaceView {
    public DemoGLSurfaceView(Context context) {
        super(context);
        mRenderer = new MGGLRenderer();
        setRenderer(mRenderer);
       //setRenderMode(RENDERMODE_WHEN_DIRTY);
       _this=this;
       imm=(InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE); 
       this.setFocusable(true);
       this.setFocusableInTouchMode(true);
    }
    public boolean onTouchEvent(final MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
        	int x=(int) event.getX();
        	int y=(int) event.getY();
        	if (imm.isActive())
        		imm.hideSoftInputFromWindow(DemoActivity._this.getCurrentFocus().getWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS);
        	nativeMouseDown(x,y);
        	DemoActivity._this.onSendMessage("123");
        	if (DEBUG) Log.i("TAG", "ACTION_DOWN");
        }
        else if(event.getAction()==MotionEvent.ACTION_UP)
        {
        	int x=(int) event.getX();
        	int y=(int) event.getY();
        	nativeMouseUp(x,y);
        	//imm.hideSoftInputFromWindow(DemoActivity._this.getCurrentFocus().getWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS);
                	
        } else if(event.getAction()==MotionEvent.ACTION_MOVE)
        {
        	int x=(int) event.getX();
        	int y=(int) event.getY();
        	nativeMouseMove(x,y);
        }
    	
        
    	return true;
    }


    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {           
             //setTitle("you pressed key:" + String.valueOf(keyCode));
    	if (DEBUG) Log.i("TAG", "onKeyDown"+String.valueOf(keyCode));
    	return super.onKeyDown(keyCode, event);
    }
    @Override
	public InputConnection onCreateInputConnection(EditorInfo outAttrs) { 
    	if (DEBUG)
			Log.i("TAG", "in onCreateInputConnection");
    	mIC = new MGInputConnection(this,false);//super.onCreateInputConnection(outAttrs);

        return mIC;
    }
    @Override
    public boolean onKeyPreIme (int keyCode, KeyEvent event) {
//    	String cs = "aaaa";
//    	cs = cs + keyCode;
//    	mIC.commitText(cs.toUpperCase(), 1);
		return false;
    }
    public static void ToggleSoftInput()
    {
    	imm.toggleSoftInput(0, InputMethodManager.HIDE_NOT_ALWAYS);
    }
    
    public static void HideSoftInput()
    {
    	imm.hideSoftInputFromWindow(DemoActivity._this.getCurrentFocus().getWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS); 
    }
    private static InputMethodManager imm = null;	
    MGGLRenderer mRenderer;
    public static DemoGLSurfaceView _this;
    public static final boolean DEBUG=true;
    InputConnection mIC;
    
    private static native void nativePause();
    private static native void nativeMouseDown(int x,int y);
    private static native void nativeMouseUp(int x,int y);
    private static native void nativeMouseMove(int x,int y);
}