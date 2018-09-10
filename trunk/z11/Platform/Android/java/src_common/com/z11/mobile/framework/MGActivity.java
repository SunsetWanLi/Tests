package com.z11.mobile.framework;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.UUID;

import javax.security.auth.PrivateCredentialPermission;

import android.R.color;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.graphics.Rect;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.location.LocationManager;
import android.os.Build.VERSION;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.Build;
import android.provider.Settings.Secure;
//import android.support.v4.app.FragmentActivity;
import android.telephony.CellLocation;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.text.ClipboardManager;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewTreeObserver;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

class CustomRelativeLayout extends RelativeLayout {

	private static final String TAG = "CustomRelativeLayout";
	private WindowManager mWindowManager = null;
	private SoftInputMethodStatusListener mSoftInputMethodStatusListener;
	private int mStatusBarHeight = 0;
	
	public interface SoftInputMethodStatusListener {
		void onSoftInputMethodHidden(int total, int current);
		void onSoftInputMethodShown(int total, int current);
	}
	
	public CustomRelativeLayout(Context context, AttributeSet attrs) {
		super(context, attrs);
		mWindowManager = (WindowManager) context.getApplicationContext()
		.getSystemService(Context.WINDOW_SERVICE);
	}
	
	public void setSoftKeyboardStatusListener(SoftInputMethodStatusListener l) {
		mSoftInputMethodStatusListener = l;
	}
	
	@Override
	protected void onLayout(boolean changed, int l, int t, int r, int b) {
		super.onLayout(changed, l, t, r, b);
		int screenHeight = mWindowManager.getDefaultDisplay().getHeight();
		if(mStatusBarHeight == 0) {
			Rect outRect = new Rect();
			this.getWindowVisibleDisplayFrame(outRect);
			mStatusBarHeight = outRect.top;
		}
		int availableHeight = screenHeight - mStatusBarHeight;
	}

	@Override
	public void onSizeChanged(int w, int h, int oldw, int oldh) {
		super.onSizeChanged(w, h, oldw, oldh);
		// get the available height
		int screenHeight = mWindowManager.getDefaultDisplay().getHeight();
		if(mStatusBarHeight == 0) {
			Rect outRect = new Rect();
			this.getWindowVisibleDisplayFrame(outRect);
			mStatusBarHeight = outRect.top;
		}
		int availableHeight = screenHeight - mStatusBarHeight;
		
		// check whether the virtual keyboard is showing
		if (h < availableHeight) {
			if (mSoftInputMethodStatusListener != null) {
				Log.d(TAG,"SoftInputMethod is Shown >>>>>>");
				mSoftInputMethodStatusListener.onSoftInputMethodShown(availableHeight, h);
			}
		} else {
			if (mSoftInputMethodStatusListener != null) {
				Log.d(TAG,"SoftInputMethod is Hidden <<<<<<");
				mSoftInputMethodStatusListener.onSoftInputMethodHidden(availableHeight, h);
			}
		}
		if (!MGGLSurfaceView.isSoftInputShow) {
			MGActivity.screenWidth = w;
			MGActivity.screenHeight = h;
			Log.i("Layout Test:", "onSizeChanged Keyboard " + String.valueOf(MGActivity.screenWidth) + " " + String.valueOf(MGActivity.screenHeight));
		}
		if (!MGGLSurfaceView.isSoftInputShow) {
			MGActivity.nativeInit(w,h); // just initialize AppDelegate with first call, and always set MGDevice.
		}
	}
}

class CustomLinearLayout extends LinearLayout {
	
//	private Handler uiHandler = new Handler();
	
	public CustomLinearLayout(Context context) {
		super(context);
	}
	
	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		super.onMeasure(widthMeasureSpec, heightMeasureSpec);
//		Log.e("Layout Test:","onMeasure " + String.valueOf(widthMeasureSpec) + " " + String.valueOf(heightMeasureSpec));
//		if (MGGLSurfaceView.mainView != null && MGActivity.screenHeight != 0) {
//			MGGLSurfaceView.mainView.setBottom(MGActivity.screenHeight);// 通过自定义坐标来放置你的控件
//    	}
	}
	
	protected void onLayout(boolean changed, int l, int t, int r, int b) {
		super.onLayout(changed, l, t, r, b);
////		if (MGGLSurfaceView.mainView != null && MGActivity.screenHeight != 0) {
////			MGGLSurfaceView.mainView.setBottom(MGActivity.screenHeight);// 通过自定义坐标来放置你的控件
////    	}
//		//Log.e("Layout Test:", "onLayout " + String.valueOf(changed) + " " + String.valueOf(l) + " " + String.valueOf(t) + " " + String.valueOf(r) + " " + String.valueOf(b));
//		int w = r - l;
//		int h = b - t;
//		if (!MGGLSurfaceView.isSoftInputShow) {
//			MGActivity.screenWidth = w;
//			MGActivity.screenHeight = h;
//			//Log.e("Layout Test:", "onSizeChanged Keyboard " + String.valueOf(MGActivity.screenWidth) + " " + String.valueOf(MGActivity.screenHeight));
//		}
////		if (!MGGLSurfaceView.isSoftInputShow) {
////			MGActivity.nativeInit(w,h); // just initialize AppDelegate with first call, and always set MGDevice.
////		}
//		int diff_x = w - MGActivity.screenWidth;
//		int diff_y = h - MGActivity.screenHeight;
//		
//		if (MGGLSurfaceView.isSoftInputShow)
//		{
//			diff_y = (int) (w > h ? (-h * 0.7) : (-h / 3));
//		}
//		
//		// 设置页面平移以适应屏幕键盘弹出后输入框可见,传入的-diff_x和-diff_y，可以认为是键盘的大小w和h //
//		MGActivity.nativeResize(-diff_x, -diff_y);
	}
	
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {
		// onSizeChanged 调用场景 //
		/* 1. 应用启动或者恢复，此时没有键盘，应该将w、h赋值	isSoftInputShow == false;
		 * 2. 键盘弹出，此时不需要改变任何值				isSoftInputShow == true;
		 * 3. 键盘隐藏，此时应该将w、h赋值				isSoftInputShow == false;
		 * 4. 键盘显示时屏幕旋转，应该主动关闭键盘			isSoftInputShow == false;	目前没有这个场景的情况，后续要增加
		 * 5. 键盘隐藏时屏幕旋转，此时应该将w，h赋值		isSoftInputShow == false;
		 * */
		//super.onSizeChanged(w, h, oldw, oldh);
//		if (MGGLSurfaceView.mainView != null && MGActivity.screenHeight != 0) {
//			MGGLSurfaceView.mainView.setBottom(MGActivity.screenHeight);// 通过自定义坐标来放置你的控件
//    	}
//		
//		if (diff_y == 0) {
//			// 这里通过计算出来的键盘高度来判断键盘是否还在显示，主要用于键盘已经在显示是，应用内Activity切换造成的问题，其他的调用逻辑无法正确判断键盘是否还在 //
//			MGGLSurfaceView.isSoftInputShow = false;
//		}
		
//		uiHandler.post(new Runnable() {  
//            @Override  
//            public void run() {
//            	if (MGGLSurfaceView.mainView != null && MGActivity.screenHeight != 0) {
//        			MGGLSurfaceView.mainView.setBottom(MGActivity.screenHeight);// 通过自定义坐标来放置你的控件
//            	}
//            }  
//        });
//		Log.e("Layout Test:", "onSizeChanged " + String.valueOf(w) + " " + String.valueOf(h) + " " + String.valueOf(oldw) + " " + String.valueOf(oldh));
	}
	
}

@SuppressLint("NewApi")
public class MGActivity extends Activity implements CustomRelativeLayout.SoftInputMethodStatusListener {
	public static boolean assetsCoverEnable = true;
	public static Context context = null;
	public static MGActivity activity_instance = null;
	public static int screenWidth;
	public static int screenHeight;
	protected static String os_type;
	protected static String os_version;
	protected static String device_id;
	protected static String device_type;
	protected static int device_dpi;
	private static MGMusic backgroundMusicPlayer;
	private static MGSound soundPlayer;
	private static MGAccelerometer accelerometer;
	private static MGMediaPicker mediaPicker = null;
	private static MGLocationManager locationMannager = null;
	private static MGSensorManager sensorManager = null;
	private TelephonyManager telephonyManager = null;
	public static int callState = TelephonyManager.CALL_STATE_IDLE;
	private static boolean accelerometerEnabled = false;
	private static String packageName;
	private final static int HANDLE_SHOW_DIALOG = 0;
	public static final int HANDLE_SYSTEM_EVENT = 1;
	public static final int HANDLE_TIMER_EVENT = 2;
	
	public static native void nativeInit(int w, int h);
    public static native void nativeResize(int w, int h);

	private static native void nativeSetPaths(String apkPath, String sdcardPath);
	private static native void nativeSetClipboardString(String clip_str);
	protected static native void nativeForeignerSDKApiBack(String response);
	private static native String nativeGetClipboardString();

	private static native void nativeSetDeviceParameter(String os_type,
			String os_version, String device_id, String device_type,
			int device_dpi);
	
	private static native void nativeApplicationWillResignActive();
	private static native void nativeApplicationDidEnterBackground();
	private static native void nativeApplicationWillEnterForeground();
	private static native void nativeApplicationDidBecomeActive();
	
	private static native void nativeHandleSystemEvent();

	private Handler messageHandler;
	private CameraPreview mPreview;
	
	public int sysVersion = 0;
	public int need_adjust_status_bar_height = 0;
	public int status_bar_height = 0;
	private int theme = 0;
	private int virtual_key_height = 0;
	public boolean show_navi_bar = true;
	
	@Override
	public void onSoftInputMethodHidden(int total, int current) {
		//do something
		//Log.e("onSoftInputMethodHidden", String.valueOf(total) + " " + String.valueOf(current));
	}
	@Override
	public void onSoftInputMethodShown(int total, int current) {
	    //do something
		//Log.e("onSoftInputMethodShown", String.valueOf(total) + " " + String.valueOf(current));
	}

	class MessageHandler extends Handler {
		public MessageHandler(Looper looper) {
			super(looper);
		}

		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case HANDLE_SYSTEM_EVENT:
				try {
					// 这里应该调用C++层的GuiContainer::container->handleEvent() //
					nativeHandleSystemEvent();
				} catch (UnsatisfiedLinkError e) {
					e.printStackTrace();
				}
				break;
			case HANDLE_TIMER_EVENT:
				Log.i("MGTimer.nativeHandleEvent", "msg.obj="
						+ (Integer) msg.obj);
				MGTimer.nativeHandleEvent((Integer) msg.obj);
				break;
			case HANDLE_SHOW_DIALOG:
				showDialog(((DialogMessage) msg.obj).title,
						((DialogMessage) msg.obj).message);
				break;
			default:
				break;
			}
		}
	}
	
	private static boolean checkDeviceHasNavigationBar(Context context) {
        boolean hasNavigationBar = false;
        Resources rs = context.getResources();
        int id = rs.getIdentifier("config_showNavigationBar", "bool", "android");
        if (id > 0) {
            hasNavigationBar = rs.getBoolean(id);
        }
        boolean hasNavigationBar2 = false;
        try {
            Class systemPropertiesClass = Class.forName("android.os.SystemProperties");
            Method m = systemPropertiesClass.getMethod("get", String.class);
            String navBarOverride = (String) m.invoke(systemPropertiesClass, "qemu.hw.mainkeys");
            if ("1".equals(navBarOverride)) {
                hasNavigationBar2 = false;
            } else if ("0".equals(navBarOverride)) {
                hasNavigationBar2 = true;
            }
        } catch (Exception e) {
            Log.w("checkNaviBar", e);
        }
        return hasNavigationBar&&hasNavigationBar2;
    }
	
	private static int getNavigationBarHeight(Context context) {
        int navigationBarHeight = 0;
        Resources rs = context.getResources();
        int id = rs.getIdentifier("navigation_bar_height", "dimen", "android");
        if (id > 0 && checkDeviceHasNavigationBar(context)) {
            navigationBarHeight = rs.getDimensionPixelSize(id);
        }
        return navigationBarHeight;
    }

	@Override
	protected void onCreate(Bundle savedInstanceState) {

		Log.i("MGActivity", "onCreate");
		activity_instance = this;
		super.onCreate(savedInstanceState);

		context = getWindow().getContext();
		
		File sdDir = null; 
		String sdcard_path = "";
	    boolean sdCardExist = Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED);   //判断sd卡是否存在 
	    if   (sdCardExist)   
	    {                               
	    	sdDir = Environment.getExternalStorageDirectory();//获取根目录 
	    }else{
	    	sdDir = Environment.getRootDirectory();//获取根目录 
	    }
	    if (sdDir != null && sdDir.exists())
	    {
	    	sdcard_path = sdDir.getAbsolutePath();
	    } else {
	    	sdcard_path = "/";
	    }
	 // 得到路径
	    try {
	    	String sd_default = sdcard_path;
	    	if (sd_default.endsWith("/")) {
	    		sd_default = sd_default.substring(0, sd_default.length() - 1);
	    	}
	    	Runtime runtime = Runtime.getRuntime();
	    	Process proc = runtime.exec("mount");
	    	InputStream is = proc.getInputStream();
	    	InputStreamReader isr = new InputStreamReader(is);
	    	String line;
	    	String ext_sd_path = "";
	    	BufferedReader br = new BufferedReader(isr);
	    	while ((line = br.readLine()) != null) {
	    		if (line.contains("secure"))
	    			continue;
	    		if (line.contains("asec"))
	    			continue;
	    		if (line.contains("fat") && line.contains("/mnt/")) {
	    			String columns[] = line.split(" ");
	    			if (columns != null && columns.length > 1) {
	    				if (sd_default.trim().equals(columns[1].trim())) {
	    					continue;
	    				}
	    				ext_sd_path = columns[1];
	    			}
	    		} else if (line.contains("fuse") && line.contains("/mnt/")) {
	    			String columns[] = line.split(" ");
	    			if (columns != null && columns.length > 1) {
	    				if (sd_default.trim().equals(columns[1].trim())) {
	    					continue;
	    				}
	    				ext_sd_path = columns[1];
	    			}
	    		}
	    	}
	    	if (ext_sd_path.length() > 0) {
	    		sdcard_path += (";" + ext_sd_path);
	    	}
	    } catch (Exception e) {
	    	e.printStackTrace();
	    }

		setPackageName(this.getPackageName(), sdcard_path);

		os_type = "Android";
		os_version = android.os.Build.VERSION.RELEASE;
		// device_id = getUniqueID();
		device_id = Secure.getString(context.getContentResolver(), Secure.ANDROID_ID);
		
		/*----------------------------------------------------------*/
		TelephonyManager tm = (TelephonyManager)this.getSystemService(Context.TELEPHONY_SERVICE);
		if (tm != null) {
			String tmDevice, tmSerial;
			tmDevice = tm.getDeviceId();
			tmSerial = tm.getSimSerialNumber();
			if (tmDevice != null) {
				String androidId = Secure.getString(context.getContentResolver(), Secure.ANDROID_ID);
				UUID deviceUuid;
				if (tmSerial != null)
					deviceUuid = new UUID(androidId.hashCode(), ((long)tmDevice.hashCode() << 32) | tmSerial.hashCode());
				else
					deviceUuid = new UUID(androidId.hashCode(), (long)tmDevice.hashCode());
				device_id = deviceUuid.toString();
			}
		}
		
		/*----------------------------------------------------------*/
		
		device_type = Build.MODEL;
		DisplayMetrics dm = new DisplayMetrics();
		device_dpi = dm.densityDpi;
		Log.i("z11", "os_version=" + os_version + ", device_id=" + device_id
				+ ", device_type=" + device_type + ", device_dpi=" + device_dpi);
		nativeSetDeviceParameter(os_type, os_version, device_id, device_type,
				device_dpi);

		/*
		 * if (device_type.equals("Nexus S")) { screenHeight -= 81; screenWidth
		 * += 10; }
		 */

		accelerometer = new MGAccelerometer(this);
		mediaPicker = new MGMediaPicker(this);

		// init media player and sound player
		backgroundMusicPlayer = new MGMusic(this);
		soundPlayer = new MGSound(this);

		// init bitmap context
		MGBitmap.setContext(this);

		try {
			telephonyManager = (TelephonyManager) this
					.getSystemService(Context.TELEPHONY_SERVICE);
			telephonyManager.listen(new MyPhoneState(),
					PhoneStateListener.LISTEN_CALL_STATE);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
		
		// 得到当前线程的Looper实例，由于当前线程是UI线程也可以通过Looper.getMainLooper()得到
		Looper looper = Looper.myLooper();
		// 此处甚至可以不需要设置Looper，因为 Handler默认就使用当前线程的Looper
		messageHandler = new MessageHandler(looper);
		// 拷贝文件
		copyAssets2();
		MGAssetManager.setAssetManager(getAssets());

		initWindow();
		ActivityInfo activityInfo;
		sysVersion = Integer.parseInt(VERSION.SDK);
	    status_bar_height = 0;
	    theme = 0;
	    virtual_key_height = 0;
		try {
			activityInfo = getPackageManager().getActivityInfo(getComponentName(), PackageManager.GET_META_DATA);
			//String dataName=activityInfo.screenOrientation.metaData.getString("data_Name");
			int orientation = activityInfo.screenOrientation;
			theme = activityInfo.theme;
			WindowManager m = getWindowManager();    
		    Display d = m.getDefaultDisplay();  //为获取屏幕宽、高
		    if (theme == 0) {
		    	status_bar_height = getStatusBarHeight();
//		    	Rect frame = new Rect();
//		    	getWindow().getDecorView().getWindowVisibleDisplayFrame(frame);
//		    	status_bar_height = frame.top;
		    }
		    virtual_key_height = 0;
            Class c;
            int real_width = 0;
            int real_height = 0;
			if (sysVersion >= 14) { //Android 4.0
				try {
			    	c = Class.forName("android.view.Display");
			    	@SuppressWarnings("unchecked")
	                Method method = c.getMethod("getRealMetrics",DisplayMetrics.class);
			    	method.invoke(d, dm);
			    	// 默认以竖屏来定义width和height //
			    	real_width = dm.widthPixels;
			    	real_height = dm.heightPixels;
			    	if (real_width > real_height)
			    	{
			    		int tmp = real_height;
			    		real_height = real_width;
			    		real_width = tmp;
			    	}
			    	screenWidth = d.getWidth();
			    	screenHeight = d.getHeight();
			    	if (screenWidth > screenHeight)
			    	{
			    		int tmp = screenHeight;
			    		screenHeight = screenWidth;
			    		screenWidth = tmp;
			    	}
			    	virtual_key_height = real_width - screenWidth;
			    	if (virtual_key_height == 0)
			    		virtual_key_height = real_height - screenHeight;
			    	if (virtual_key_height == 0)
			    		virtual_key_height = getNavigationBarHeight(context);
			    	//Log.d("Virtual Key Size", "Size: " + virtual_key_height);
			    }catch(Exception e){
			    	e.printStackTrace();
			    }
			} else if (sysVersion < 14 && sysVersion >= 13) //Android 3.2
		    { 
				Display display = this.getWindowManager().getDefaultDisplay();
				c = Class.forName("android.view.Display");
				Method method = c.getMethod("getRealHeight");
				real_height = (Integer) method.invoke(display);
				method = c.getMethod("getRealWidth");
				real_width = (Integer) method.invoke(display);
				Rect rect= new Rect();  
				this.getWindow().getDecorView().getWindowVisibleDisplayFrame(rect);  
				status_bar_height = real_height - rect.bottom;
				if (real_width > real_height)
		    	{
		    		int tmp = real_height;
		    		real_height = real_width;
		    		real_width = tmp;
		    	}
				screenWidth = d.getWidth();
		    	screenHeight = d.getHeight();
		    	if (screenWidth > screenHeight)
		    	{
		    		int tmp = screenHeight;
		    		screenHeight = screenWidth;
		    		screenWidth = tmp;
		    	}
		    	status_bar_height = 0;
		    	virtual_key_height = getStatusBarHeight();
		    } else if (sysVersion < 13 && sysVersion >= 11) //Android 3.0 - 3.1
		    {
		    	Rect rect= new Rect();  
		    	this.getWindow().getDecorView().getWindowVisibleDisplayFrame(rect);
		    	real_width = rect.width();//屏幕宽度
		    	real_height = rect.height();//屏幕高度
		    	if (real_width > real_height)
		    	{
		    		int tmp = real_height;
		    		real_height = real_width;
		    		real_width = tmp;
		    	}
		    	status_bar_height = 0;
		    	virtual_key_height = getStatusBarHeight();
		    } else if (sysVersion < 11) //Android 2.x
		    {
		    	WindowManager wm = (WindowManager) this.getSystemService(Context.WINDOW_SERVICE);
		    	real_width = wm.getDefaultDisplay().getWidth();//屏幕宽度
		    	real_height = wm.getDefaultDisplay().getHeight();//屏幕高度
		    	if (real_width > real_height)
		    	{
		    		int tmp = real_height;
		    		real_height = real_width;
		    		real_width = tmp;
		    	}
		    }
			if (orientation == ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE) {
				screenWidth = real_height;
				if (sysVersion >= 19 && show_navi_bar == false) {	//Android 4.4
					screenHeight = real_width - status_bar_height;
				} else {
					screenHeight = real_width - virtual_key_height - status_bar_height;
				}
			} else if (orientation == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT) {
				screenWidth = real_width;
				if (sysVersion >= 19 && show_navi_bar == false) {	//Android 4.4
					screenHeight = real_height - status_bar_height;
				} else {
					screenHeight = real_height - virtual_key_height - status_bar_height;
				}
			} else {
				screenWidth = d.getWidth();
				screenHeight = d.getHeight() - status_bar_height;
			}
		} catch (NameNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InvocationTargetException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		nativeInit(screenWidth, screenHeight);
		mGLView = new MGGLSurfaceView(context);
		mGLView.setBackgroundColor(color.holo_blue_bright);
		MITextField mTextView = new MITextField(getApplicationContext());
		mGLView.setTextField(mTextView);
		// 加入一个Layout//
		{
			// Fra meLayout
			ViewGroup.LayoutParams framelayout_params = new ViewGroup.LayoutParams(
					ViewGroup.LayoutParams.FILL_PARENT,
					ViewGroup.LayoutParams.FILL_PARENT);
			CustomLinearLayout framelayout = new CustomLinearLayout(this);
			//framelayout.setSoftKeyboardStatusListener(this);
			framelayout.setLayoutParams(framelayout_params);
			framelayout.setOrientation(LinearLayout.HORIZONTAL);
			framelayout.setFocusable(false);
			framelayout.setFocusableInTouchMode(false);
			framelayout.setGravity(Gravity.CENTER_HORIZONTAL);

			// Cocos2dxEditText layout
			ViewGroup.LayoutParams edittext_layout_params = new ViewGroup.LayoutParams(
					ViewGroup.LayoutParams.FILL_PARENT,
					ViewGroup.LayoutParams.WRAP_CONTENT);
			TextView textField = mGLView.getTextField();
			textField.setLayoutParams(edittext_layout_params);

			// ...add to FrameLayout
			framelayout.addView(mGLView);
			
			{
				// Create our Preview view and set it as the content of our
				// activity.
				mPreview = new CameraPreview(this);
				FrameLayout preview = new FrameLayout(this);
				// FrameLayout
				ViewGroup.LayoutParams layout_params = new ViewGroup.LayoutParams(
						1, 1);
				preview.setLayoutParams(layout_params);
				preview.addView(mPreview);
				mPreview.setVisibility(View.INVISIBLE);
				framelayout.addView(preview);

			}

			// ...add to FrameLayout
			framelayout.addView(textField);

			// Set framelayout as the content view
			setContentView(framelayout);
			
			// 增加键盘布局改变监听，尝试获取键盘高度 //
//			
			framelayout.getViewTreeObserver().addOnGlobalLayoutListener(new ViewTreeObserver.OnGlobalLayoutListener() {

                @Override
                public void onGlobalLayout() {
                    // TODO Auto-generated method stub
                    Rect r = new Rect();
                    View rootview = activity_instance.getWindow().getDecorView(); 
                    rootview.getWindowVisibleDisplayFrame(r);
                    
                    //Log.d("View Display", "mGLView " + mGLView + ", DecorView " + rootview + ", Size: " + r);

                    //int screenHeight = rootview.getRootView().getHeight();
                    int heightDifference = screenHeight - (r.bottom - r.top);
                    //Log.d("Keyboard Size", "Size: " + heightDifference);
                    
                    
                    if (heightDifference == virtual_key_height || heightDifference == 0) {
                    	if(heightDifference==0){
                    		nativeResize(screenWidth, heightDifference);
                    	}
                    	if (sysVersion >= 19) //Android 4.4
                			need_adjust_status_bar_height = 0;
                    	onHeightChanged(-need_adjust_status_bar_height);
                    	return;
                	} else if (heightDifference + virtual_key_height == 0) {
                		if (sysVersion >= 19) //Android 4.4
                			need_adjust_status_bar_height = 0 + virtual_key_height;
                		onHeightChanged(-need_adjust_status_bar_height);
                		return;
                	}
                    //Log.d("On Adjust Size", "Size: " + need_adjust_status_bar_height);
                    nativeResize(screenWidth, heightDifference);
                    //boolean visible = heightDiff > screenHeight / 3;
                }
            });
		}
		
		if (sysVersion >= 19) { //Android 4.4
			int flag = 0;
			if (theme != 0) {
				flag |= View.SYSTEM_UI_FLAG_FULLSCREEN;		// hide status bar
			}
			if (show_navi_bar == false) {
				flag |= View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION;
				flag |= View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;		// hide navi bar
				flag |= View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;
			}
			activity_instance.getWindow().getDecorView().setSystemUiVisibility(flag);
		}
		// 设置键盘是HIDDEN，确保应用启动和重新进入时都保持键盘不可见，这样容易实现逻辑统一 //
		getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_PAN | WindowManager.LayoutParams.SOFT_INPUT_STATE_HIDDEN);
	}

	public void onHeightChanged(int height)
	{
		if(MGWebViewManager.getWebView()!=null){
			MGWebViewManager.getWebView().setTranslationY(-height);
		}
	}
	
	public MGGLSurfaceView mGLView;

	public int getStatusBarHeight() {
		Class<?> c = null;
		Object obj = null;
		Field field = null;
		int x = 0, sbar = 0;
		try {
			c = Class.forName("com.android.internal.R$dimen");
			obj = c.newInstance();
			field = c.getField("status_bar_height");
			x = Integer.parseInt(field.get(obj).toString());
			sbar = getResources().getDimensionPixelSize(x);
		} catch (Exception e) {
			e.printStackTrace();
			sbar = 48;
		}
		return sbar;
	}

	//private Boolean statusBarHide = true;

//	public void hideStatusBar(Boolean hide) {
//		statusBarHide = hide;
//	}

	private void initWindow() {
		// 隐藏标题
		requestWindowFeature(Window.FEATURE_NO_TITLE);
//		// 获得窗口对象
//		Window curWindow = this.getWindow();
//		// 定义全屏参数
//		int flag = WindowManager.LayoutParams.FLAG_FULLSCREEN;
//		// 设置Flag标示
//
//		// get frame size
////		DisplayMetrics dm = new DisplayMetrics();
////		getWindowManager().getDefaultDisplay().getMetrics(dm);
////		screenWidth = dm.widthPixels;
////		screenHeight = dm.heightPixels;
//
//		if (!statusBarHide) {
//			//screenHeight -= getStatusBarHeight();
//			curWindow.setFlags(0, WindowManager.LayoutParams.FLAG_FULLSCREEN);
//			flag = WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN;
//		} else {
//			curWindow.setFlags(0,
//					WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN);
//			flag = WindowManager.LayoutParams.FLAG_FULLSCREEN;
//			//if (device_type.equals("GT-P7500"))
//				//screenHeight -= getStatusBarHeight();
//		}
//		curWindow.setFlags(flag, flag);
	}

	public void onSendMessage(int msg, int id) {
		try {
			Message message = Message.obtain(messageHandler, msg, id);
			messageHandler.sendMessage(message);
		} catch (Exception e) {
			Log.e("z11", "on send message error");
		}
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		Log.i("MGActivity TAG", "onKeyDown code = " + String.valueOf(keyCode));
//		try {
//			MGMessageCommunication.JavaToNativeCommandAndData(7, "" + keyCode);
//		} catch (UnsatisfiedLinkError e) {
//			e.printStackTrace();
//		}
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			AlertDialog quit_dialog = new AlertDialog.Builder(this)
					.setTitle("提示")
					.setMessage("确定退出?")
					.setPositiveButton("确定",
							new DialogInterface.OnClickListener() {
								public void onClick(DialogInterface dialog,
										int whichButton) {
									setResult(RESULT_OK);// 确定按钮事件
									exitApplication();
								}
							})
					.setNegativeButton("取消",
							new DialogInterface.OnClickListener() {
								public void onClick(DialogInterface dialog,
										int whichButton) {
									// 取消按钮事件
								}
							}).create();
			if (quit_dialog != null) {
				quit_dialog.show();
			}
			return true;
		} else {
			return super.onKeyDown(keyCode, event);
		}
	}

	// c++也要调用此方法 ///
	public static void exitApplication() {
		MGActivity.activity_instance.onDestroy();
		MGActivity.activity_instance.finish();
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	public static String getCurrentLanguage() {
		String languageName = java.util.Locale.getDefault().getLanguage();
		return languageName;
	}
	
	public static void preserveEvent() {
		activity_instance.onSendMessage(HANDLE_SYSTEM_EVENT, 0);
	}

	public static void showMessageBox(String title, String message) {
		Message msg = new Message();
		msg.what = HANDLE_SHOW_DIALOG;
		msg.obj = new DialogMessage(title, message);
		activity_instance.messageHandler.sendMessage(msg);
	}

	public static MGLocationManager getLocationManager() {
		return locationMannager;
	}

	public static void startLocationService() {
		if (locationMannager == null) {
			locationMannager = new MGLocationManager(activity_instance);
			try {
				locationMannager.onCreate((LocationManager) context
						.getSystemService(Context.LOCATION_SERVICE));
			} catch (NullPointerException e) {
				e.printStackTrace();
			}
		}
		locationMannager.isStop = false;
		locationMannager.startLocationService();
	}

	public static void stopLocationService() {
		if (locationMannager != null) {
			locationMannager.onPause();
			locationMannager.isStop = true;
		}
	}

	private static void initSensorManager() {
		if (sensorManager == null) {
			sensorManager = new MGSensorManager();
			try {
				sensorManager.onCreate((SensorManager) context
						.getSystemService(Context.SENSOR_SERVICE));
			} catch (NullPointerException e) {
				e.printStackTrace();
			}
		}
	}
	public static void startHeadingService() {
		initSensorManager();
		if (sensorManager != null) {
			sensorManager.addSensorTag(1);					 
			sensorManager.startWithType(Sensor.TYPE_ORIENTATION);
		}
	}
	public static void stopHeadingService() {
		if (sensorManager != null) {
			sensorManager.removeSensorTag(1);						
			sensorManager.stopWithType(Sensor.TYPE_ORIENTATION);
		}
	}
	public static void startAccelerationService(float interval) {
		initSensorManager();
		if (sensorManager != null) {
			sensorManager.startWithType(Sensor.TYPE_ACCELEROMETER);
		}
	}
	public static void stopAccelerationService() {
		if (sensorManager != null) {
			sensorManager.stopWithType(Sensor.TYPE_ACCELEROMETER);
		}
	}
	public static void startRotationService(float interval) {
		initSensorManager();
		if (sensorManager != null) {
			sensorManager.startWithType(Sensor.TYPE_GYROSCOPE);
		}
	}
	public static void stopRotationService() {
		if (sensorManager != null) {
			sensorManager.stopWithType(Sensor.TYPE_GYROSCOPE);
		}
	}
	public static void startAttitudeService(float interval) {
		initSensorManager();
		if (sensorManager != null) {
			sensorManager.addSensorTag(2);
			sensorManager.startWithType(Sensor.TYPE_ORIENTATION);
		}
	}
	public static void stopAttitudeService() {
		if (sensorManager != null) {
			sensorManager.removeSensorTag(2);
			sensorManager.stopWithType(Sensor.TYPE_ORIENTATION);
		}
	}
	
	public static void enableAccelerometer() {
		accelerometerEnabled = true;
		accelerometer.enable();
	}

	public static void disableAccelerometer() {
		accelerometerEnabled = false;
		accelerometer.disable();
	}

	public static void playBackgroundMusic(String path, boolean isLoop) {
		backgroundMusicPlayer.playBackgroundMusic(path, isLoop);
	}

	public static void stopBackgroundMusic() {
		backgroundMusicPlayer.stopBackgroundMusic();
	}

	public static void pauseBackgroundMusic() {
		backgroundMusicPlayer.pauseBackgroundMusic();
	}

	public static void resumeBackgroundMusic() {
		backgroundMusicPlayer.resumeBackgroundMusic();
	}

	public static void rewindBackgroundMusic() {
		backgroundMusicPlayer.rewindBackgroundMusic();
	}

	public static boolean isBackgroundMusicPlaying() {
		return backgroundMusicPlayer.isBackgroundMusicPlaying();
	}

	public static float getBackgroundMusicVolume() {
		return backgroundMusicPlayer.getBackgroundVolume();
	}

	public static void setBackgroundMusicVolume(float volume) {
		backgroundMusicPlayer.setBackgroundVolume(volume);
	}

	public static int playEffect(String path, boolean isLoop) {
		return soundPlayer.playEffect(path, isLoop);
	}

	public static void stopEffect(int soundId) {
		soundPlayer.stopEffect(soundId);
	}

	public static float getEffectsVolume() {
		return soundPlayer.getEffectsVolume();
	}

	public static void setEffectsVolume(float volume) {
		soundPlayer.setEffectsVolume(volume);
	}

	public static void preloadEffect(String path) {
		soundPlayer.preloadEffect(path);
	}

	public static void unloadEffect(String path) {
		soundPlayer.unloadEffect(path);
	}

	public static void end() {
		backgroundMusicPlayer.end();
		soundPlayer.end();
	}

	public static String getZ11PackageName() {
		return packageName;
	}

	public static void terminateProcess() {
		android.os.Process.killProcess(android.os.Process.myPid());
	}
	
	public static void appLockScreenWhenIdle() {
		activity_instance.mGLView.setKeepScreenOn(false);
	}
	
	public static void appUnlockScreenWhenIdle() {
		activity_instance.mGLView.setKeepScreenOn(true);
	}
	
	public static void appShowSystemStatusBar() {
		int flag=WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN;
		activity_instance.getWindow().setFlags(flag, flag);
	}
	
	public static void appHideSystemStatusBar() {
		int flag=WindowManager.LayoutParams.FLAG_FULLSCREEN;
		activity_instance.getWindow().setFlags(flag, flag);
	}
	
	public static void foreignerSDKApiCallJni(String api) {
		if (activity_instance != null)
			activity_instance.foreignerSDKApiCall(api);
	}
	
	public void foreignerSDKApiCall(String api)
	{
		
	}
	
	public void foreignerSDKApiBack(String response)
	{
		nativeForeignerSDKApiBack(response);
	}

	@Override
	protected void onResume() {
		Log.i("MGActivity", "onResume");
		super.onResume();
		if (accelerometerEnabled) {
			accelerometer.enable();
		}
		if (sensorManager != null) {
			sensorManager.onResume();
		}
		if (locationMannager != null) {
			if (!locationMannager.isStop)
				locationMannager.startLocationService();
		}

		// resume background music
		resumeBackgroundMusic();
		mGLView.onResume();
		nativeApplicationDidBecomeActive();
	}

	@Override
	protected void onPause() {
		Log.i("MGActivity", "onPause");
		super.onPause();
		nativeApplicationWillResignActive();
		if (accelerometerEnabled) {
			accelerometer.disable();
		}
		if (sensorManager != null) {
			sensorManager.onPause();
		}
		if (locationMannager != null) {
			locationMannager.onPause();
		}

		// close MediaRecorder
		MGMediaRecorderManager.stop();

		// pause background music
		pauseBackgroundMusic();
		mGLView.onPause();
	}

	@Override
	protected void onStart() {
		Log.i("MGActivity", "onStart");
		nativeApplicationWillEnterForeground();
		super.onStart();
		ClipboardManager clipboarManager=(ClipboardManager)getSystemService(Context.CLIPBOARD_SERVICE);
		String msg = "";
		if (clipboarManager.hasText())
			msg = clipboarManager.getText().toString();
		nativeSetClipboardString(msg);
	}

	@Override
	protected void onRestart() {
		Log.i("MGActivity", "onRestart");
		super.onRestart();
	}

	@Override
	protected void onStop() {
		Log.i("MGActivity", "onStop");
		String msg = nativeGetClipboardString();
		ClipboardManager clipboarManager=(ClipboardManager)getSystemService(Context.CLIPBOARD_SERVICE);
		clipboarManager.setText(msg);
		super.onStop();
		nativeApplicationDidEnterBackground();
	}

	@Override
	protected void onSaveInstanceState(Bundle outState) {
		Log.i("MGActivity", "onSaveInstanceState");
		super.onSaveInstanceState(outState);
	}

	@Override
	protected void onRestoreInstanceState(Bundle savedInstanceState) {
		Log.i("MGActivity", "onRestoreInstanceState");
		super.onRestoreInstanceState(savedInstanceState);
	}

	// 当指定了android:configChanges="orientation"后,方向改变时onConfigurationChanged被调用
	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		Log.i("MGActivity", "onConfigurationChanged");
		super.onConfigurationChanged(newConfig);
		// switch (newConfig.orientation) {
		// case Configuration.ORIENTATION_PORTRAIT:
		// break;
		// case Configuration.ORIENTATION_LANDSCAPE:
		// break;
		// }
	}

	@SuppressWarnings("deprecation")
	@Override
	protected void onDestroy() {
		Log.i("MGActivity", "onDestroy");
		super.onDestroy();
		if (locationMannager != null)
			locationMannager.onDestroy();
	}

	private static final int REQUEST_MEDIAPICKER_START = 0x00;
	private static final int REQUEST_MEDIAPICKER_END = 0x08;

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (resultCode == Activity.RESULT_OK) {
			if (requestCode >= REQUEST_MEDIAPICKER_START
					&& requestCode <= REQUEST_MEDIAPICKER_END) {
				mediaPicker.onActivityResult(requestCode, resultCode, data);
			}
			else if(requestCode>=1000){
				MGWebViewManager.onActivityResult(requestCode, resultCode, data);
			}
		}
//		else if(resultCode == Activity.RESULT_CANCELED){
//			if(requestCode>=1000){
//				MGWebViewManager.onActivityResultCanceled(requestCode, resultCode, data);
//			}
//		}
	}

	protected void setPackageName(String packageName, String sdcardPath) {
		Log.w("packageName", packageName);
		MGActivity.packageName = packageName;

		String apkFilePath = "";
		String mainBundelPath ="";
		ApplicationInfo appInfo = null;
		PackageManager packMgmr = getApplication().getPackageManager();
		try {
			appInfo = packMgmr.getApplicationInfo(packageName, 0);
		} catch (NameNotFoundException e) {
			e.printStackTrace();
			throw new RuntimeException("Unable to locate assets, aborting...");
		}
		apkFilePath = appInfo.sourceDir;
		mainBundelPath = appInfo.dataDir;
		
		Log.w("apk path", apkFilePath);

		// add this link at the renderer class
		// nativeSetPaths(apkFilePath);
		// 设置当前程序的资源路径/data/data/com.z11.mobile.framework/files/Android.App
		// nativeSetPaths("/data/data/" + packageName + "/files/Android.App");
		if (sdcardPath.equals(""))
			sdcardPath = "/";
		nativeSetPaths(packageName + "," + apkFilePath + "," + mainBundelPath, sdcardPath);
	}

	private void showDialog(String title, String message) {
		Dialog dialog = new AlertDialog.Builder(this).setTitle(title)
				.setMessage(message)
				.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int whichButton) {

					}
				}).create();

		dialog.show();
	}

	private void copyAssets2() {
		String absolutePath = this.getFilesDir().getAbsolutePath();
		Log.i("this.getFilesDir().getAbsolutePath()", absolutePath);

		// make directory
		File file = new File(absolutePath + "/Android.App/");
		file.mkdir();

		file = new File(absolutePath + "/Android.App/documents/");
		file.mkdir();

		file = new File(absolutePath + "/Android.App/tmp/");
		file.mkdir();

		file = new File(absolutePath + "/Android.App/tmp/controls");
		file.mkdir();

		file = new File(absolutePath + "/Android.App/Resources/");
		file.mkdir();

		// copy readable files if assetsCoverEnable
		copyDirectory(absolutePath + "/Android.App/", "readable",
				assetsCoverEnable);
	}

	static {
		System.loadLibrary("z11");
	}

	private void copyDirectory(String outfile, String infile, boolean bCover) {
		try {
			if (!infile.contains(".")) {
				String[] files = getAssets().list(infile);
				if (files.length > 0) {
					File mWorkingPath = new File(outfile + infile);
					if (!mWorkingPath.exists()) {
						if (!mWorkingPath.mkdirs()) {
							Log.d("copyAssets", mWorkingPath
									+ "!mWorkingPath.mkdirs()");
						} else
							Log.d("copyAssets", "make dir=" + outfile + infile);

					} else {
						Log.d("copyAssets", "dir: " + outfile + infile
								+ " already exist");
					}
					for (int i = 0; i < files.length; i++) {
						if (infile.length() != 0)
							copyDirectory(outfile, infile + "/" + files[i],
									bCover);
						else
							copyDirectory(outfile, files[i], bCover);
					}
				}
			} else {
				File outFile = new File(outfile + infile);
				if (outFile.exists()) {
					if (bCover) {
						outFile.delete();
						outFile = new File(outfile + infile);
					} else {
						return;
					}
				}
				// Transfer bytes from in to out
				OutputStream out = new FileOutputStream(outFile);
				InputStream in = getAssets().open(infile);
				byte[] buf = new byte[1024];
				int len;
				while ((len = in.read(buf)) > 0) {
					out.write(buf, 0, len);
				}
				in.close();
				out.close();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}

class MyPhoneState extends PhoneStateListener {

	@Override
	public void onCallStateChanged(int state, String incomingNumber) {
		super.onCallStateChanged(state, incomingNumber);

		MGActivity.callState = state;
		switch (state) {
		case TelephonyManager.CALL_STATE_IDLE:
			// 再次进入时继续播放音乐
			MGMediaPlayer.resumeMediaWhenOnResume();
			break;
		case TelephonyManager.CALL_STATE_OFFHOOK:
			break;
		case TelephonyManager.CALL_STATE_RINGING:
			// pause all medias
			MGMediaPlayer.pauseMediaWhenOnPause();
			break;

		default:
			break;
		}
	}
};

class DialogMessage {
	public String title;
	public String message;

	public DialogMessage(String title, String message) {
		this.message = message;
		this.title = title;
	}
}
