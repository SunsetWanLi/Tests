package com.z11.mobile.framework;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.text.SimpleDateFormat;
import java.util.Date;

import org.apache.http.util.EncodingUtils;

import android.R.bool;
import android.R.integer;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.ContentValues;
import android.content.DialogInterface;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.Paint;
import android.net.Uri;
import android.os.Environment;
import android.os.Message;
import android.provider.MediaStore;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.webkit.DownloadListener;
import android.webkit.JsResult;
import android.webkit.ValueCallback;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebSettings.LayoutAlgorithm;
import android.webkit.WebSettings.PluginState;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.webkit.WebSettings.RenderPriority;
import android.widget.FrameLayout;
import android.widget.Toast;

@SuppressLint("NewApi")

public class MGWebViewManager {
	private static final String TAG = "MGWebViewManager";
	private static WebView mWebView = null;
	private static boolean is_visible = false;
	private static boolean is_in_loading = false;
	private static ValueCallback<Uri> mUploadMessage = null;
	private static String mCameraPhotoPath;

	private static native void nativeSetPaths(String apkPath);

	private static native boolean shouldStartLoadWithRequest(String request);

	private static native void didStartLoad();

	private static native void didFinishLoad();

	private static native void didFailLoadWithError(int err_code,
			String description);
	
	private static native void didRecievedTitle(String title);

	
	public static void webViewOpenURL(String url, String post_body, float x, float y,
			float width, float height) {
		destroyWebView();
		// AbsoluteLayout.LayoutParams params = new AbsoluteLayout.LayoutParams(
		// (int) width, (int) height, (int) x, (int) y);
		FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(
				(int) width, (int) height);
		params.gravity = Gravity.TOP;
		params.leftMargin = (int) x;
		params.topMargin = (int) y;
		if (MGActivity.activity_instance.sysVersion >= 19)
			params.topMargin += MGActivity.activity_instance.need_adjust_status_bar_height;

		mWebView = new WebView(MGActivity.activity_instance);
		MGActivity.activity_instance.addContentView(mWebView, params);
		mWebView.setVisibility(View.INVISIBLE);
		
		initWebSettings();
		

		mWebView.setWebViewClient(new WebViewClient() {
			@Override
			public boolean shouldOverrideUrlLoading(final WebView view,
					final String url) {
				Log.i(TAG, "shouldStartLoadWithRequest:" + url);
				boolean ret = shouldStartLoadWithRequest(url);
				if(ret){
					view.loadUrl(url);
				}
				return true;
			}

			@Override
			public void onPageStarted(WebView view, String url, Bitmap favicon) {
				super.onPageStarted(view, url, favicon);
				is_in_loading = true;
				didStartLoad();
			}

			@Override
			public void onPageFinished(WebView view, String url) {
				super.onPageFinished(view, url);
				is_in_loading = false;
				didFinishLoad();
			}

			@Override
			public void onReceivedError(WebView view, int errorCode,
					String description, String failingUrl) {
				is_in_loading = false;
				didFailLoadWithError(errorCode, description);
			}
			
		});

		mWebView.setDownloadListener(new DownloadListener() {
			@Override
			public void onDownloadStart(String url, String userAgent,
					String contentDisposition, String mimetype,
					long contentLength) {
			}
		});

		
		
		mWebView.setWebChromeClient(new WebChromeClient() {
			@Override
			public void onProgressChanged(WebView view, int progress) {// 载入进度改变而触发
				if (progress == 100) {
				}
				super.onProgressChanged(view, progress);
			}
			
			@Override
			public void onReceivedTitle(WebView view, String title) { 
	            // 设置当前网页的标题栏
				didRecievedTitle(title);
	            super.onReceivedTitle(view, title); 
	        } 
			

			public void openFileChooser(ValueCallback<Uri> uploadFile, String acceptType, String capture) {
		        openFileChooser(uploadFile,acceptType);
			}
			
			public void openFileChooser(ValueCallback<Uri> uploadFile, String acceptType) { 
                if (mUploadMessage != null) return;
                mUploadMessage = uploadFile;
                MGActivity.activity_instance.startActivityForResult(createDefaultOpenableIntent(),1000);
            }
                 
             // For Android < 3.0
         	public void openFileChooser(ValueCallback<Uri> uploadFile) {
                openFileChooser( uploadFile, "" );
         	}
         	
        });
        
        

		// load init url
		if(post_body.isEmpty()){
//			mWebView.loadUrl("http://image.baidu.com");
			mWebView.loadUrl(url);
		}
		else{
			mWebView.postUrl(url, EncodingUtils.getBytes(post_body, "UTF-8"));
		}
	}
	
	public static boolean isVisible()
	{
		return is_visible;
	}
	public static WebView getWebView()
	{
		return mWebView;
	}
	public static void webViewShow() {
		if (mWebView != null)
		{
			mWebView.setVisibility(View.VISIBLE);
			is_visible = true;
		}
	}
	
	public static void webViewHide() {
		if (mWebView != null)
		{
			mWebView.setVisibility(View.INVISIBLE);
			is_visible = false;
		}
	}
	
	public static void webViewClose() {
		if (mWebView != null) {
			destroyWebView();
			is_visible = false;
		}
	}

	public static void webViewZoomEnabled(Boolean enabled) {
		if (mWebView != null)
		{
			mWebView.getSettings().setSupportZoom(enabled);
		}
	}
	
	@SuppressLint("NewApi")
	public static Boolean canGoBack(){
		if(mWebView != null)
		{
			return mWebView.canGoBack();
		}
		return false;
	}
	
	public static Boolean canGoForward(){
		if(mWebView != null)
		{
			return mWebView.canGoForward();
		}
		return false;
	}
	
	public static void goBack(){
		if(canGoBack()){
			mWebView.goBack();
		}
	}
	
	public static void goForward(){
		if(canGoForward()){
			mWebView.goForward();
		}
	}
	
	public static void reload() {
		if(mWebView != null){
			mWebView.reload();
		}
	}
	
	public static void stopLoading() {
		if(mWebView != null){
			mWebView.stopLoading();
		}
	}
	
	public static boolean isInLoading(){
		return is_in_loading;
	}
	
	public static void setWebViewLayerType(int arg0, Paint paint) {
		if (mWebView != null)
		{
			mWebView.setLayerType(arg0, paint);
		}
	}
	
	private static void destroyWebView() {
		if (mWebView != null) {
			try {
				mWebView.getClass().getMethod("onPause").invoke(mWebView,(Object[])null);
			} catch (IllegalAccessException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IllegalArgumentException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (InvocationTargetException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (NoSuchMethodException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			((FrameLayout) mWebView.getParent()).removeView(mWebView);
			mWebView.destroy();
			mWebView = null;
		}
	}

	@SuppressWarnings("deprecation")
	private static void initWebSettings() {

		mWebView.requestFocus();
		mWebView.setHorizontalScrollBarEnabled(false);
		mWebView.setVerticalScrollBarEnabled(false);
		
		WebSettings webSettings = mWebView.getSettings();
		webSettings.setAppCacheEnabled(false);
		webSettings.setJavaScriptCanOpenWindowsAutomatically(true);
		webSettings.setCacheMode(WebSettings.LOAD_NO_CACHE);
		webSettings.setDomStorageEnabled(true);
		webSettings.setAllowFileAccess(true);
		webSettings.setJavaScriptEnabled(true);// 可用JS
		webSettings.setLightTouchEnabled(false);
		webSettings.setPluginState(PluginState.ON);
		// webSettings.setCacheMode(WebSettings.LOAD_NO_CACHE);
		//webSettings.setLayoutAlgorithm(LayoutAlgorithm.SINGLE_COLUMN);
		webSettings.setSupportMultipleWindows(true);
		webSettings.setUseWideViewPort(true);
		webSettings.setLoadWithOverviewMode(true); 
		webSettings.setRenderPriority(RenderPriority.HIGH);
		webSettings.setAllowUniversalAccessFromFileURLs(true);// 允许访问本地文件
	}
	
	
	private static Intent createDefaultOpenableIntent() {
        // Create and return a chooser with the default OPENABLE
        // actions including the camera, camcorder and sound
        // recorder where available.
          Intent i = new Intent(Intent.ACTION_GET_CONTENT);
          i.addCategory(Intent.CATEGORY_OPENABLE);
          i.setType("*/*");

         Intent chooser = createChooserIntent(createCameraIntent());
         chooser.putExtra(Intent.EXTRA_INTENT, i);
         return chooser;
     }
     
     private static Intent createChooserIntent(Intent... intents) {
    	 Intent chooser = new Intent(Intent.ACTION_CHOOSER);
         chooser.putExtra(Intent.EXTRA_INITIAL_INTENTS, intents);
         chooser.putExtra(Intent.EXTRA_TITLE, "File Chooser");
        return chooser;
      }

     private static Intent createCameraIntent() {
    	 /*
    	 Intent cameraIntent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
    	 File externalDataDir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM);
    	 File cameraDataDir = new File(externalDataDir.getAbsolutePath() +File.separator + "browser-photos");
    	 cameraDataDir.mkdirs();
    	 String mCameraFilePath = cameraDataDir.getAbsolutePath() + File.separator +System.currentTimeMillis() + ".jpg";
    	 cameraIntent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(new File(mCameraFilePath)));
    	 return cameraIntent;
    	 */
    	 
    	Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);

  		imagePaths = Environment.getExternalStorageDirectory().getPath()
  				+ "/fuiou_wmp/temp/"
  				+ (System.currentTimeMillis() + ".jpg");
  		// 必须确保文件夹路径存在，否则拍照后无法完成回调
  		File vFile = new File(imagePaths);
  		if (!vFile.exists()) {
  			File vDirPath = vFile.getParentFile();
  			vDirPath.mkdirs();
  		} else {
  			if (vFile.exists()) {
  				vFile.delete();
  			}
  		}
  		cameraUri = Uri.fromFile(vFile);
  		intent.putExtra(MediaStore.EXTRA_OUTPUT, cameraUri);
  		return intent;
     }

     private static Intent createCamcorderIntent() {
    	 return new Intent(MediaStore.ACTION_VIDEO_CAPTURE);
     }

     private static Intent createSoundRecorderIntent() {
    	 return new Intent(MediaStore.Audio.Media.RECORD_SOUND_ACTION);
     }
     
/*
     public static void onActivityResult(int requestCode, int resultCode, Intent data) {
           if(requestCode==1000) {  
              if (null == mUploadMessage) {
            	  return;  
              }
              else{
                  mUploadMessage.onReceiveValue(result);  
                  mUploadMessage = null;
              }          
           }
     }
      */
     
     public final static boolean checkSDcard() {
 		boolean flag = Environment.getExternalStorageState().equals(
 				Environment.MEDIA_MOUNTED);
 		
 		return flag;
 	}
 	static String compressPath = "";
 	
 	protected final static void selectImage() {
 		if (!checkSDcard())
 			return;
 		String[] selectPicTypeStr = { "camera","photo" };
 		new AlertDialog.Builder(MGActivity.activity_instance.getApplicationContext())
 				.setItems(selectPicTypeStr,
 						new DialogInterface.OnClickListener() {
 							@Override
 							public void onClick(DialogInterface dialog,
 									int which) {
 								switch (which) {
 								// 手机拍摄
 								case 0:
 									openCarcme();
 									break;
 								// 手机相册
 								case 1:
 									chosePic();
 									break;
 								default:
 									break;
 								}
// 								compressPath = Environment
// 										.getExternalStorageDirectory()
// 										.getPath()
// 										+ "/fuiou_wmp/temp";
// 								new File(compressPath).mkdirs();
// 								compressPath = compressPath + File.separator
// 										+ "compress.jpg";
 							}
 						}).show();
 	}
 	
 	static String imagePaths = null;
 	static Uri  cameraUri = null;
 	/**
 	 * 打开照相机
 	 */
 	private static void openCarcme() {
 		Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);

 		imagePaths = Environment.getExternalStorageDirectory().getPath()
 				+ "/fuiou_wmp/temp/"
 				+ (System.currentTimeMillis() + ".jpg");
 		//  必须确保文件夹路径存在，否则拍照后无法完成回调
 		File vFile = new File(imagePaths);
 		if (!vFile.exists()) {
 			File vDirPath = vFile.getParentFile();
 			vDirPath.mkdirs();
 		} else {
 			if (vFile.exists()) {
 				vFile.delete();
 			}
 		}
 		cameraUri = Uri.fromFile(vFile);
 		intent.putExtra(MediaStore.EXTRA_OUTPUT, cameraUri);
 		MGActivity.activity_instance.startActivityForResult(intent, 1000);
 	}

 	/**
 	 * 拍照结束后
 	 */
 	private static void afterOpenCamera() {
 		File f = new File(imagePaths);
 		addImageGallery(f);
// 		File newFile = FileUtils.compressFile(f.getPath(), compressPath);
 	}

 	/** 解决拍照后在相册中找不到的问题 */
 	private static void addImageGallery(File file) {
 		ContentValues values = new ContentValues();
 		values.put(MediaStore.Images.Media.DATA, file.getAbsolutePath());
 		values.put(MediaStore.Images.Media.MIME_TYPE, "image/jpeg");
 		MGActivity.activity_instance.getContentResolver().insert(
 				MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
 	}

 	/**
 	 * 本地相册选择图片
 	 */
 	private static void chosePic() {
// 		FileUtils.delFile(compressPath);
 		Intent innerIntent = new Intent(Intent.ACTION_GET_CONTENT); // "android.intent.action.GET_CONTENT"
 		String IMAGE_UNSPECIFIED = "image/*";
 		innerIntent.setType(IMAGE_UNSPECIFIED); // 查看类型
 		Intent wrapperIntent = Intent.createChooser(innerIntent, null);
 		MGActivity.activity_instance.startActivityForResult(wrapperIntent, 1001);
 	}

 	/**
 	 * 选择照片后结束
 	 * 
 	 * @param data
 	 */
 	private static Uri afterChosePic(Intent data) {

 		// 获取图片的路径：
 		String proj = MediaStore.Images.Media.DATA;
 		// 好像是android多媒体数据库的封装接口，具体的看Android文档
 		Uri uri = null;
 		String file = "";
 		uri = data.getData();
 		Cursor cursor = null;
 		
		String uripath = uri.getScheme();
		if(uripath.equals("file")){
			file = uri.getPath();
		}
		else{
			cursor = MGActivity.activity_instance.getContentResolver().query(uri, null, null, null,
					null);
			cursor.moveToFirst();
			int path_index = cursor.getColumnIndexOrThrow(proj);
			// String imgNo = cursor.getString(0); // 编号 //
			file = cursor.getString(path_index); // 文件路径 //
		}
 		
 		if(file!=null||!file.equals("")){
 			return Uri.fromFile(new File(file));
 		}
 		
 		return null;
 	}



 	/**
 	 * 返回文件选择
 	 */
 	protected static void onActivityResult(int requestCode, int resultCode,
 			Intent intent) {
// 		if (requestCode == FILECHOOSER_RESULTCODE) {
// 			if (null == mUploadMessage)
// 				return;
// 			Uri result = intent == null || resultCode != RESULT_OK ? null
// 					: intent.getData();
// 			mUploadMessage.onReceiveValue(result);
// 			mUploadMessage = null;
// 		}
 		
 		if (null == mUploadMessage)
 			return;
 		
 		Uri uri = null;
 		if(intent==null){
 			afterOpenCamera();
 	 		uri = cameraUri;
 		}
 		else{
 			uri = afterChosePic(intent);
 		}
 		
 
 		mUploadMessage.onReceiveValue(uri);
 		mUploadMessage = null;
 		cameraUri = null;
 		imagePaths = null;
 	}
}