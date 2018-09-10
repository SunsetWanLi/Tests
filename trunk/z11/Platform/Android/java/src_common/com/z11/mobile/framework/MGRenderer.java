package com.z11.mobile.framework;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.opengl.GLSurfaceView;
import android.opengl.GLU;
import android.util.Log;
import android.widget.LinearLayout;

public class MGRenderer implements GLSurfaceView.Renderer {
	private final static long NANOSECONDSPERSECOND = 1000000000L;
	private final static long NANOSECONDSPERMINISECOND = 1000000;
	private static long animationInterval = (long)(1.0 / 60 * NANOSECONDSPERSECOND);
	private long last;
	public MGGLSurfaceView gl_view;
	//OpenGLRenderer sample_render=new OpenGLRenderer();
	//public static boolean bOnResume=false;
    public void onSurfaceCreated(GL10 gl, EGLConfig config) { 	
    	Log.i("Renderer","onSurfaceCreated");
    	last = System.nanoTime();
    	//sample_render.onSurfaceCreated(gl, config);
    }

    public void onSurfaceChanged(GL10 gl, int w, int h) {
//    	Log.e("Renderer","onSurfaceChanged " + String.valueOf(w) + " " + String.valueOf(h));
    	
    	// MGGLSurfaceView的onResume会将bOnResume赋值为true //
//     	if(bOnResume)
//     	{
//     		nativeOnResume();
//     		bOnResume=false;
//     	}
    	//nativeOnResume();
    }
    
    public void onDrawFrame(GL10 gl) {
    	//Log.i("z11","onDrawFrame");
    	nativeRender();
        //sample_render.onDrawFrame(gl);
    }
    
    public void handleActionDown(int id, float x, float y)
    {
    	nativeTouchesBegin(id, x, y-MGActivity.activity_instance.need_adjust_status_bar_height);
//    	Log.v("MGRenderer", "=============handleActionDown============="+id);
//    	float tx = gl_view.getX();
//    	float ty = gl_view.getY();
//    	float tw = gl_view.getWidth();
//    	float th = gl_view.getHeight();
//    	Log.v("Touch Test", "x:"+x+"  y:"+y);
//    	Log.v("Touch Test", "tx:"+tx+"  ty:"+ty+"  tw:"+tw+"  th:"+th);
    }
    
    public void handleActionUp(int id, float x, float y)
    {
    	nativeTouchesEnd(id, x, y-MGActivity.activity_instance.need_adjust_status_bar_height);
//    	Log.v("MGRenderer", "=============handleActionUp============="+id);
    }
    
    public void handleActionCancel(int[] id, float[] x, float[] y)
    {
    	float ny[] = new float[y.length];
    	for	(int i = 0; i < y.length; ++i)
    	{
    		ny[i] = y[i]-MGActivity.activity_instance.need_adjust_status_bar_height;
    	}
    	nativeTouchesCancel(id, x, ny);
//    	String log = "=============handleActionCancel=============";
//    	for (int i : id) {
//    		log += i + ",";
//    	}
//    	Log.v("MGRenderer", log);
    }
    
    public void handleActionMove(int[] id, float[] x, float[] y)
    {
    	float ny[] = new float[y.length];
    	for	(int i = 0; i < y.length; ++i)
    	{
    		ny[i] = y[i]-MGActivity.activity_instance.need_adjust_status_bar_height;
    	}
    	nativeTouchesMove(id, x, ny);
//    	String log = "=============handleActionMove=============";
//    	for (int i : id) {
//    		log += i + ",";
//    	}
//    	Log.v("MGRenderer", log);
    }
    
    public boolean handleKeyDown(int keyCode)
    {
    	return nativeKeyDown(keyCode);
    }
    
    public void handleOnPause(){
    	//do nothing
    	nativeOnPause();
    }
    
    public void handleOnResume(){
    	//do nothing
    	//nativeOnResume();
    }
    
    public static void setAnimationInterval(double interval){
    	animationInterval = (long)(interval * NANOSECONDSPERSECOND);
    }
    private static native void nativeTouchesBegin(int id, float x, float y);
    private static native void nativeTouchesEnd(int id, float x, float y);
    private static native void nativeTouchesMove(int[] id, float[] x, float[] y);
    private static native void nativeTouchesCancel(int[] id, float[] x, float[] y);
    private static native boolean nativeKeyDown(int keyCode);
    private static native void nativeRender();
    private static native void nativeOnPause();
    
    /////////////////////////////////////////////////////////////////////////////////
    // handle input method edit message
    /////////////////////////////////////////////////////////////////////////////////
    
    public void handleInsertText(final String text) {
    	nativeInsertText(text);
    }
    
    public void handleDeleteBackward() {
    	nativeDeleteBackward();
    }

	public String getContentText() {
		return nativeGetContentText();
	}
	
    private static native void nativeInsertText(String text);
    private static native void nativeDeleteBackward();
    private static native String nativeGetContentText();
}
//sample test code,please don't delete 
	 class Square {
		// Our vertices.
		private float vertices[] = {
			      -1.0f,  1.0f, 0.0f,  // 0, Top Left
			      -1.0f, -1.0f, 0.0f,  // 1, Bottom Left
			       1.0f, -1.0f, 0.0f,  // 2, Bottom Right
			       1.0f,  1.0f, 0.0f,  // 3, Top Right
			};

		// The order we like to connect them.
		private short[] indices = { 0, 1, 2, 0, 2, 3 };

		// Our vertex buffer.
		private FloatBuffer vertexBuffer;

		// Our index buffer.
		private ShortBuffer indexBuffer;

		public Square() {
			// a float is 4 bytes, therefore we 
			// multiply the number if
			// vertices with 4.
			ByteBuffer vbb 
			  = ByteBuffer.allocateDirect(vertices.length * 4);
			vbb.order(ByteOrder.nativeOrder());
			vertexBuffer = vbb.asFloatBuffer();
			vertexBuffer.put(vertices);
			vertexBuffer.position(0);

			// short is 2 bytes, therefore we multiply 
			//the number if
			// vertices with 2.
			ByteBuffer ibb 
			 = ByteBuffer.allocateDirect(indices.length * 2);
			ibb.order(ByteOrder.nativeOrder());
			indexBuffer = ibb.asShortBuffer();
			indexBuffer.put(indices);
			indexBuffer.position(0);
		}

		/**
		 * This function draws our square on screen.
		 * @param gl
		 */
		public void draw(GL10 gl) {
			// Counter-clockwise winding.
			gl.glFrontFace(GL10.GL_CCW); 
			// Enable face culling.
			gl.glEnable(GL10.GL_CULL_FACE); 
			// What faces to remove with the face culling.
			gl.glCullFace(GL10.GL_BACK); 

			// Enabled the vertices buffer for writing 
			//and to be used during
			// rendering.
			gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
			// Specifies the location and data format of 
			//an array of vertex
			// coordinates to use when rendering.
			gl.glVertexPointer(3, GL10.GL_FLOAT, 0, 
	                                 vertexBuffer);

			gl.glDrawElements(GL10.GL_TRIANGLES, indices.length,
					  GL10.GL_UNSIGNED_SHORT, indexBuffer);

			// Disable the vertices buffer.
			gl.glDisableClientState(GL10.GL_VERTEX_ARRAY); 
			// Disable face culling.
			gl.glDisable(GL10.GL_CULL_FACE); 
		}

	}
	
	class OpenGLRenderer {
		
		public OpenGLRenderer()
		{}
		// Initialize our square.
		Square square = new Square();
		private float angle;
		/*
		 * (non-Javadoc)
		 *
		 * @see
		 * android.opengl.GLSurfaceView.Renderer#onSurfaceCreated(javax.
	         * microedition.khronos.opengles.GL10, javax.microedition.khronos.
	         * egl.EGLConfig)
		 */
		public void onSurfaceCreated(GL10 gl, EGLConfig config) {
			// Set the background color to black ( rgba ).
			gl.glClearColor(0.0f, 0.0f, 0.0f, 0.5f);  // OpenGL docs.
			// Enable Smooth Shading, default not really needed.
			gl.glShadeModel(GL10.GL_SMOOTH);// OpenGL docs.
			// Depth buffer setup.
			gl.glClearDepthf(1.0f);// OpenGL docs.
			// Enables depth testing.
			gl.glEnable(GL10.GL_DEPTH_TEST);// OpenGL docs.
			// The type of depth testing to do.
			gl.glDepthFunc(GL10.GL_LEQUAL);// OpenGL docs.
			// Really nice perspective calculations.
			gl.glHint(GL10.GL_PERSPECTIVE_CORRECTION_HINT, // OpenGL docs.
	                          GL10.GL_NICEST);
			
			

		}

		/*
		 * (non-Javadoc)
		 *
		 * @see
		 * android.opengl.GLSurfaceView.Renderer#onDrawFrame(javax.
	         * microedition.khronos.opengles.GL10)
		 */
		public void onDrawFrame(GL10 gl) {
			// Clears the screen and depth buffer.
			gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
			// Replace the current matrix with the identity matrix
			gl.glLoadIdentity();
			// Translates 10 units into the screen.
			gl.glTranslatef(0, 0, -10); 

			// SQUARE A
			// Save the current matrix.
			gl.glPushMatrix();
			// Rotate square A counter-clockwise.
			gl.glRotatef(angle, 0, 0, 1);
			// Draw square A.
			square.draw(gl);
			// Restore the last matrix.
			gl.glPopMatrix();

			// SQUARE B
			// Save the current matrix
			gl.glPushMatrix();
			// Rotate square B before moving it, making it rotate around A.
			gl.glRotatef(-angle, 0, 0, 1);
			// Move square B.
			gl.glTranslatef(2, 0, 0);
			// Scale it to 50% of square A
			gl.glScalef(.5f, .5f, .5f);
			// Draw square B.
			square.draw(gl);			

			// SQUARE C
			// Save the current matrix
			gl.glPushMatrix();
			// Make the rotation around B
			gl.glRotatef(-angle, 0, 0, 1);
			gl.glTranslatef(2, 0, 0);
			// Scale it to 50% of square B
			gl.glScalef(.5f, .5f, .5f);
			// Rotate around it's own center.
			gl.glRotatef(angle*10, 0, 0, 1);
			// Draw square C.
			square.draw(gl);

			// Restore to the matrix as it was before C.
			gl.glPopMatrix();
			// Restore to the matrix as it was before B.
			gl.glPopMatrix();

			// Increse the angle.
			angle++;


		}

		/*
		 * (non-Javadoc)
		 *
		 * @see
		 * android.opengl.GLSurfaceView.Renderer#onSurfaceChanged(javax.
	         * microedition.khronos.opengles.GL10, int, int)
		 */
		public void onSurfaceChanged(GL10 gl, int width, int height) {
			// Sets the current view port to the new size.
			gl.glViewport(0, 0, width, height);// OpenGL docs.
			// Select the projection matrix
			gl.glMatrixMode(GL10.GL_PROJECTION);// OpenGL docs.
			// Reset the projection matrix
			gl.glLoadIdentity();// OpenGL docs.
			// Calculate the aspect ratio of the window
			GLU.gluPerspective(gl, 45.0f,
	                                   (float) width / (float) height,
	                                   0.1f, 100.0f);
			// Select the modelview matrix
			gl.glMatrixMode(GL10.GL_MODELVIEW);// OpenGL docs.
			// Reset the modelview matrix
			gl.glLoadIdentity();// OpenGL docs.
		}
	}