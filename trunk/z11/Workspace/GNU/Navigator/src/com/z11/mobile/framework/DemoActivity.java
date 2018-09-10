package com.z11.mobile.framework;


import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.zj.pub.mcu.GetRtspAddr;


import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;

import android.location.LocationManager;

import com.cgrj.wenzhoutong.*;
public class DemoActivity extends Activity {
	private MGLocationMannager m_location_mannager;
	class MessageHandler extends Handler {

	    public MessageHandler(Looper looper) {

	        super(looper);

	    }

	    @Override

	    public void handleMessage(Message msg) {

	    	MGMessageCommunication.JavaToNativeCommandAndData(0,"");

	    }

	}
	public void copyFile(String outfile,String infile)
	{
		InputStream in ;
		OutputStream out;
		try {
			File outFile=new File(outfile);
			if(outFile.exists())
			{
				outFile.delete();
				outFile=new File(outfile);
//				return ;
			}
			out = new FileOutputStream(outFile);
			in = getAssets().open(infile);
			// Transfer bytes from in to out
			byte[] buf = new byte[1024];
			int len;
			while ((len = in.read(buf)) > 0) {
				out.write(buf, 0, len);
			}

			in.close();
			out.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}
	private void copyAssets() {
		String[] files;
		Log.i("this.getFilesDir().getAbsolutePath()",this.getFilesDir().getAbsolutePath());
		String absolutePath=this.getFilesDir().getAbsolutePath();
		File mWorkingPath = new File(absolutePath+"/Android.App");
		File mTempPath    = new File(absolutePath+"/tmp");
		File mConfigPath  = new File("/sdcard/温州通");
		try {
			files = this.getResources().getAssets().list("");
			if (!mWorkingPath.exists()) {
				if (!mWorkingPath.mkdirs()) {
					Log.i("copyAssets","!mWorkingPath.mkdirs()");
					return;
				}
			}
			if (!mTempPath.exists()) {
				if (!mTempPath.mkdirs()) {
					return ;
				}
			}
			if (!mConfigPath.exists()) {
				if (!mConfigPath.mkdirs()) {
					return;
				}
			}

			for (int i = 0; i < files.length; i++) {
				String fileName=files[i];
				if(fileName.compareTo("images") == 0
						|| fileName.compareTo("sounds") == 0
						|| fileName.compareTo("webkit") == 0
						|| fileName.compareTo("drawable-ldpi")==0
						|| fileName.compareTo("drawable-mdpi")==0
						|| fileName.compareTo("drawable-hdpi")==0)
				{
					continue;
				}
				copyFile(mWorkingPath+"/"+files[i],files[i]);
			}

			files = this.getResources().getAssets().list("drawable-ldpi");
			for (int i = 0; i < files.length; i++) {
				copyFile(mWorkingPath+"/"+files[i],"drawable-ldpi/"+files[i]);
			}
			
			//临时用
			copyFile("/sdcard/温州通/web_config.txt","web_config.txt");
			
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}

	}
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
    	Log.i("DemoActivity","onCreate");    	
    	_this=this;        
    	super.onCreate(savedInstanceState);
    	
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,   
        		WindowManager.LayoutParams.FLAG_FULLSCREEN);  
        m_location_mannager=new MGLocationMannager();
        m_location_mannager.onCreate((LocationManager)getSystemService(Context.LOCATION_SERVICE));

		// 拷贝文件
		copyAssets();
		
        mGLView = new DemoGLSurfaceView(this);
        setContentView(mGLView);
        
        MGWebServiceThread.DOWNLOAD_DIRECTORY=this.getFilesDir().getAbsolutePath()+"/";
		Log.i("DOWNLOAD_DIRECTORY",MGWebServiceThread.DOWNLOAD_DIRECTORY);
		
		//得到当前线程的Looper实例，由于当前线程是UI线程也可以通过Looper.getMainLooper()得到

        Looper looper = Looper.myLooper();

        //此处甚至可以不需要设置Looper，因为 Handler默认就使用当前线程的Looper

        messageHandler = new MessageHandler(looper);
		
//		test thread
		//WeatherThread td = new WeatherThread("温州","2011-4-21");
		//td.start();
		//TrafficThread t = new TrafficThread("浙C37022", "2009-12-01", "2011-4-29");
		//t.start();
    }
    void onSendMessage(String msg)
    {
    	  //创建一个Message对象，并把得到的天气信息赋值给Message对象

        Message message = Message.obtain();

        message.obj = msg;

        //通过Handler发布携带有天气情况的消息

        messageHandler.sendMessage(message);
    }
    @Override
    protected void onPause() {
    	Log.i("DemoActivity","onPause");        
    	super.onPause();
        m_location_mannager.onPause(); 
        mGLView.onPause();
    }

    @Override
    protected void onResume() {
    	Log.i("DemoActivity","onResume");        
    	super.onResume();
        m_location_mannager.onResume();
        mGLView.onResume();
    }
    @Override
    protected void onDestroy() {
    	Log.i("DemoActivity","onDestroy");        
    	super.onDestroy();
        
       // System.exit(0);        
    	//或者下面这种方式

        //android.os.Process.killProcess(android.os.Process.myPid()); 

    }
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        Log.i("TAG", "onKeyDown"+String.valueOf(keyCode));
        MGMessageCommunication.JavaToNativeCommandAndData(7, ""+keyCode);
        if(keyCode == KeyEvent.KEYCODE_BACK){
        	onDestroy();
        	System.exit(0);            
        	return true;
        }else{                 
        	return super.onKeyDown(keyCode, event);
        }
    }

    private GLSurfaceView mGLView;
    public static DemoActivity _this=null;
    private Handler messageHandler;
    
    static {
        System.loadLibrary("opengldevice");
    }
}


