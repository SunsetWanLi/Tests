package com.z11.mobile.framework;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;
import android.util.Log;

public class MGGLRenderer implements GLSurfaceView.Renderer {
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    	Log.i("Renderer","onSurfaceCreated");
        	nativeInit();
    }

    public void onSurfaceChanged(GL10 gl, int w, int h) {
    	Log.i("Renderer","onSurfaceChanged");
        gl.glViewport(0, 0, w, h);
        width=w;
        height=h;
     	MGWebServiceThread.SetPixelSize(MGGLRenderer.width, MGGLRenderer.height);
        nativeResize(w, h);
    }
    //0 NULL
    //1 UpdateMap
	static int i=0;
    public void onDrawFrame(GL10 gl) {
    	//DemoActivity._this.onSendMessage("");//¥¶¿ÌHandleEvnet
    	MGMessageCommunication.GetNativeCommand();
         nativeRender();
    	
//    	//test code
//    	gl.glClearColor(1.0f, 0.0f, 0.0f, 0.5f);
//    	if(i++%2==0)gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
     
    }
    public static int width;
    public static int height;
    private boolean m_bFirstResize=true;
    private static native void nativeInit();
    private static native void nativeResize(int w, int h);
    private static native void nativeRender();
    private static native void nativeDone();

}
