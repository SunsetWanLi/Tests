package com.z11.mobile.framework;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.List;

import android.R.bool;
import android.R.integer;
import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.graphics.Bitmap.CompressFormat;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Toast;


/** A basic Camera preview class */
@SuppressLint("NewApi")
public class CameraPreview extends SurfaceView implements SurfaceHolder.Callback,Camera.PreviewCallback,Camera.AutoFocusCallback {
	private SurfaceHolder mHolder;
	private static Camera mCamera=null;
	private static boolean mCameraOpened=false;
	private final static String TAG = "MGMediaPlayer";
	private static CameraPreview preview_instance=null;
	private static Dialog textdialog = null;
	/** Check if this device has a camera */
	private boolean checkCameraHardware(Context context) {
		if (context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA)){
			// this device has a camera
			return true;
		} else {
			// no camera on this device
			return false;
		}
	}

	/** A safe way to get an instance of the Camera object. */
	public static Camera getCameraInstance(int position){
		int cameraCount = 0;
		int camIdx = 0;
        Camera.CameraInfo cameraInfo = new Camera.CameraInfo();
        cameraCount = Camera.getNumberOfCameras(); // get cameras number 
        
        int pos = Camera.CameraInfo.CAMERA_FACING_BACK;
        switch (position)
        {
        case 0:
        	;
        case 1:
        	break;
        case 2:
        	pos = Camera.CameraInfo.CAMERA_FACING_FRONT;
        	break;
        default:
        	pos = Camera.CameraInfo.CAMERA_FACING_BACK;
        	break;
        }
                
        for (camIdx = 0; camIdx < cameraCount;camIdx++ ) {  
            Camera.getCameraInfo( camIdx, cameraInfo ); // get camerainfo  
            if ( cameraInfo.facing == pos ) { 
                // 浠ｈ〃������澶寸����逛��锛����������瀹�涔���间袱涓�������涓�CAMERA_FACING_FRONT���缃����CAMERA_FACING_BACK���缃�  
               break;  
            }  
        }
		if(mCamera == null)
		{
			try {
				mCamera = Camera.open(camIdx); // attempt to get a Camera instance
			}
			catch (Exception e){
				
			}
		}
		return mCamera; // returns null if camera is unavailable*/
		//return null;
	}
	public CameraPreview(Context context) {
		super(context);
		preview_instance=this;
		
		// Install a SurfaceHolder.Callback so we get notified when the
		// underlying surface is created and destroyed.
		mHolder = getHolder();
		mHolder.addCallback(this);
		// deprecated setting, but required on Android versions prior to 3.0
		mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
	}
	public static void releaseCamera()
	{
		Log.i("CameraPreview","releaseCamera");
		mCameraOpened=false;

		if(preview_instance!=null)
		{
			preview_instance.setVisibility(View.INVISIBLE);
		}
		if (mCamera != null){
			mCamera.setPreviewCallback(null);
			mCamera.release();        // release the camera for other applications
			mCamera = null;
		}

	}
	public static void openCamera(int position)
	{
		Log.i("CameraPreview","openCamera");
		mCameraOpened=true;
		if(preview_instance!=null)
		{
			preview_instance.initCamera(position);
			preview_instance.setVisibility(View.VISIBLE);
		}
	}
	public void surfaceCreated(SurfaceHolder holder) {
		// The Surface has been created, now tell the camera where to draw the preview.
		//openCamera();
	}

	public void surfaceDestroyed(SurfaceHolder holder) {
		// empty. Take care of releasing the Camera preview in your activity.
		releaseCamera();
	}
	private void initCamera(int position)
	{
		Log.i("CameraPreview","initCamera");
		// If your preview can change or rotate, take care of those events here.
		// Make sure to stop the preview before resizing or reformatting it.

		if (mHolder.getSurface() == null){
			// preview surface does not exist
			return;
		}
		
		if(mCamera!=null)
		{
			// stop preview before making changes
			try {
				mCamera.stopPreview();
			} catch (Exception e){
				// ignore: tried to stop a non-existent preview
			}
		}

		if(mCamera == null)
		{
			getCameraInstance(position);
		}
		// set preview size and make any resize, rotate or
		// reformatting changes here

		// start preview with new settings
		try {
			Camera.Parameters params = mCamera.getParameters();
			Size size = params.getPreviewSize();
			List<Size> size_list = params.getSupportedPreviewSizes();
//			for (Size s:size_list)
//			{
//				if (s.width <= 800) {
//					size = s;
//					break;
//				}
//			}
			params.setPreviewSize(size.width, size.height);
			mCamera.setParameters(params);
			mCamera.setPreviewDisplay(mHolder);
			mCamera.setPreviewCallback(this);
			mCamera.startPreview();

		} catch (Exception e){
			if(textdialog == null){
				AlertDialog.Builder builder = new AlertDialog.Builder(MGActivity.activity_instance.context);  
		        builder.setTitle("提示"); 
		        builder.setMessage("摄像头异常，请检查应用是否有访问摄像头的权限，或重启设备后重试"); 

		        builder.setNeutralButton("确定", new DialogInterface.OnClickListener() {  
	                @Override  
	                public void onClick(DialogInterface dialog, int which) {  

	                      textdialog.dismiss();
	                }  
	            }); 
		        textdialog = builder.create();
			}
			  
	        textdialog.show();
			Log.d(TAG, "Error starting camera preview: " + e.getMessage());
			
			mCamera = null;
		}

		//Camera.Parameters parameters = mCamera.getParameters();
		//parameters.set("orientation", "landscape");
		//mCamera.setParameters(parameters);
		//Parameters parameters=mCamera.getParameters();
		//parameters.set("orientation", "portrait");
		//parameters.set("rotation", 90);  
		
		Log.i("CameraPreview","initCamera exit");
	}
	public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
		if(mCameraOpened)
			initCamera(0);
	}
	
	@Override
	public void onPreviewFrame(byte[] data, Camera camera) {
		// TODO Auto-generated method stub
		if(flag%30==0)
			mCamera.autoFocus(this);
		{
			//long ticks=System.currentTimeMillis();
			try {
				Camera.Parameters parameters = camera.getParameters();
				Size size = parameters.getPreviewSize();
				boolean rotate=MGActivity.activity_instance.screenWidth<MGActivity.activity_instance.screenHeight;
				if(rotate)
				{
					nativeOnPreviewFrame(data,data.length,size.width,size.height,90);
				}
				else {
					nativeOnPreviewFrame(data,data.length,size.width,size.height,0);
				}
				Log.i(TAG,"size: width="+size.width+"height"+size.height);
			} catch (Exception e) {
				Log.e("z11","camera get parameters failed");
			}
			
		}
		flag++;
	}
	private static int flag=0;
	private static native void nativeOnPreviewFrame(byte[] data,
			int length,int width,int height,int rotate);

	@Override
	public void onAutoFocus(boolean arg0, Camera arg1) {
		// TODO Auto-generated method stub
		Log.d("z11","Auto Focused");
	}
}