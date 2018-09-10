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

package com.z11.mobile.framework;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningTaskInfo;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.Log;
import android.widget.Toast;

class MGMediaPickerType {
	MGMediaPickerType(){}
	public int sourceType;
	public int mediaQuality;
	public int mediaLimit;
	public boolean allows_edit;
}
public class MGMediaPicker {	
	public static final int MGMediaPickerSourceTypeImage = 0;
	public static final int MGMediaPickerSourceTypePhotoLibrary = 1;
	public static final int MGMediaPickerSourceTypeSavedPhotoAlbum = 2;
	public static final int MGMediaPickerSourceTypeSound = 3;
	public static final int MGMediaPickerSourceTypeSoundLibrary = 4;
	public static final int MGMediaPickerSourceTypeVideo = 5;
	public static final int MGMediaPickerSourceTypeVideoLibrary = 6;

    private static final int REQUEST_CAPTURE_IMAGE = 0;// 照相的requestCode//
    private static final int REQUEST_VIEW_PHOTO_LIB = 1;// 照相的requestCode//
    private static final int REQUEST_VIEW_PHOTO_ALBUM = 2;// 照相的requestCode//
    private static final int REQUEST_CAPTURE_SOUND = 3;// 录音的requestCode//
    private static final int REQUEST_VIEW_SOUND = 4;// 录音的requestCode//
    private static final int REQUEST_CAPTURE_VIDEO = 5;// 摄像的照相的requestCode//
    private static final int REQUEST_VIEW_VIDEO = 6;// 摄像的照相的requestCode//
	private static final int REQUEST_CLIP_PHOTO = 7; //裁剪照片的requestCode//
	private static final int REQUEST_CLIP_END_PHOTO = 8; //裁剪完成的requestCode//
	
	private static Activity activity_instance = null;
	private static MGMediaPickerType mediaPickerType;
	private static String contentTypeString = "";
	private static String sourceTypeString = "";
	private static int mediaQualityInt = 0;
	private static int requestCodeInt = 0;
	
	public MGMediaPicker(MGActivity activity) {
		activity_instance = activity;
	}
	
    public static void openMedia() {
    	mediaPickerType = getPickerType();
    	translatePickerType();
    	open();
    }
    public static void closeMedia() {
    	
    }
    private static void translatePickerType() {
    	translateSourceType();
    	translateMediaQuality();
    }
    private static void translateSourceType() {
    	switch (mediaPickerType.sourceType) {
		case MGMediaPickerSourceTypeImage:
			sourceTypeString = MediaStore.ACTION_IMAGE_CAPTURE;
			requestCodeInt = REQUEST_CAPTURE_IMAGE;
			break;
		case MGMediaPickerSourceTypePhotoLibrary:
			sourceTypeString = Intent.ACTION_GET_CONTENT;
			contentTypeString = "image/*";
			requestCodeInt = REQUEST_VIEW_PHOTO_LIB;
			break;
		case MGMediaPickerSourceTypeSavedPhotoAlbum:
			sourceTypeString = Intent.ACTION_GET_CONTENT;
			contentTypeString = "image/*";
			requestCodeInt = REQUEST_VIEW_PHOTO_ALBUM;
			break;
		case MGMediaPickerSourceTypeSound:
			sourceTypeString = "ACTION_AUDIO_CAPTURE";
			requestCodeInt = REQUEST_CAPTURE_SOUND;
			break;
		case MGMediaPickerSourceTypeSoundLibrary:
			sourceTypeString = Intent.ACTION_GET_CONTENT;
			contentTypeString = "audio/amr";
			requestCodeInt = REQUEST_CAPTURE_SOUND;
			break;
		case MGMediaPickerSourceTypeVideo:
			sourceTypeString = MediaStore.ACTION_VIDEO_CAPTURE;
			requestCodeInt = REQUEST_CAPTURE_VIDEO;
			break;
		case MGMediaPickerSourceTypeVideoLibrary:
			sourceTypeString = Intent.ACTION_GET_CONTENT;
			contentTypeString = "video/*";
			requestCodeInt = REQUEST_VIEW_VIDEO;
			break;
		default:
			break;
		}
    }
    private static void translateMediaQuality() {
    	switch (mediaPickerType.sourceType) {
		case 0://MGMediaPickerCameraQualityHigh//
			mediaQualityInt = 2;
			break;
		case 1://MGMediaPickerCameraQualityMedium//
			mediaQualityInt = 1;	
			break;
		case 2://MGMediaPickerCameraQualityLow//
			mediaQualityInt = 0;
			break;
		default:
			break;
		}
    }

	private static String strImageFilePath = "";// 照片文件路径//
	private static String cropFilePath = "";// 临时照片文件路径//
	private static final int BITS_PER_PIXEL = 32;
	private static final int BYTES_PER_PIXEL = BITS_PER_PIXEL / 8;

	public static void open() {
		Intent intent = new Intent(MGMediaPicker.sourceTypeString);
		// File envFile = Environment.getExternalStorageDirectory(); //
		// /mnt/sdcard
		// envFile = Environment.getDownloadCacheDirectory(); // /cache
		// envFile = Environment.getDataDirectory(); // /date
		// envFile = Environment.getRootDirectory(); // /system
		
		File sdDir = null; 
	    strImageFilePath = getCameraDir();
	    sdDir = new File(strImageFilePath);
	    if (!sdDir.exists())
	    	FileOperator.createDir(strImageFilePath);
	    strImageFilePath += System.currentTimeMillis() + ".jpg";
	    cropFilePath = getCameraDir()+System.currentTimeMillis() + "tmp.jpg";
	    File tmpFile = new File(cropFilePath);
	    if(tmpFile.exists()){
	    	tmpFile.delete();
	    }
	    
		if (MGMediaPicker.sourceTypeString
				.equals(MediaStore.ACTION_IMAGE_CAPTURE)) {
			intent.putExtra(MediaStore.EXTRA_VIDEO_QUALITY,
					MGMediaPicker.mediaQualityInt);
			File file = new File(strImageFilePath);
			if (file.exists())
				file.delete();
		    intent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(file));
		} else if (MGMediaPicker.sourceTypeString
				.equals(MediaStore.ACTION_VIDEO_CAPTURE)) {
			intent.putExtra(MediaStore.EXTRA_VIDEO_QUALITY,
					MGMediaPicker.mediaQualityInt);
		} else if (MGMediaPicker.sourceTypeString
				.equals("ACTION_AUDIO_CAPTURE")) {
			intent = new Intent(MediaStore.Audio.Media.RECORD_SOUND_ACTION);
		} else if (MGMediaPicker.sourceTypeString
				.equals(Intent.ACTION_GET_CONTENT)) {
			intent.setType(MGMediaPicker.contentTypeString);
			if(Build.VERSION.SDK_INT >= 19){
				intent.setAction(Intent.ACTION_PICK);
				intent.setData(android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
			}
			// Intent wrapperIntent = Intent.createChooser(intent, "请选择以下方式打开");
			// activity_instance.startActivityForResult(wrapperIntent, request);
			if (MGMediaPicker.contentTypeString.equals("audio/amr")) {
				// 部分手机不支持//
				// intent.setClassName("com.android.soundrecorder",
				// "com.android.soundrecorder.SoundRecorder");
			} else if (MGMediaPicker.contentTypeString.equals("image/*")) {

			} else if (MGMediaPicker.contentTypeString.equals("video/*")) {

			}
		}
		try {
			activity_instance.startActivityForResult(intent,
					MGMediaPicker.requestCodeInt);
			oldFiles = getFiles();
		} catch (Exception e) {
			Toast.makeText(activity_instance.getApplicationContext(), "操作失败", Toast.LENGTH_LONG).show();
		}
	}
	
	private static List<String> oldFiles = null;
	private Comparator<File> comparator = new Comparator<File>(){
		public int compare(File f1, File f2) {
			if (f1 == null || f2 == null) {//先比较null 
				if (f1 == null) {         
					return -1;
				} else {          
					return 1;
				}
			} else {
				if (f1.isFile() && f2.isFile()) {
					if (f1.lastModified() < f2.lastModified()) {
						return -1;
					} else if (f1.lastModified() == f2.lastModified()) {
						return 0;
					} else {
						return 1;
					}
				} else {
					return -1;
				}
			}
		}
	};
	private static String getCameraDir() {
		String dir = "";
		File sdDir = null;
		boolean sdCardExist = Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED);   //判断sd卡是否存在 
	    if   (sdCardExist)   
	    {                               
	    	sdDir = Environment.getExternalStorageDirectory();//获取根目录 
	    	dir = sdDir.toString();
	    } else {
	    	sdDir = Environment.getRootDirectory();//获取根目录 
	    	dir = sdDir.toString();
	    }
	    
	    dir += "/" + activity_instance.getPackageName() + "/";
		return dir;
	}
	private File getLastFileInDCIM() {
		String dir = getCameraDir();
		File cameraDir = new File(dir);
		File lastImageFile = null;
		if (cameraDir.exists()) {
			File[] files = cameraDir.listFiles();
			List<File> listFiles = new ArrayList<File>();
			for (File file : files) {
				listFiles.add(file);
			}
			Collections.sort(listFiles, comparator);
			if(listFiles.size()>0)
				lastImageFile = listFiles.get(listFiles.size()-1);
		}
		return lastImageFile;
	}
	private static List<String> getFiles() {
		List<String> retList = null;
		String dir = getCameraDir();
		File cameraDir = new File(dir);
		if (cameraDir.exists()) {
			String[] files = cameraDir.list();
			retList = new ArrayList<String>();
			for (String file : files) {
				retList.add(file);
			}
		}
		return retList;
	}
	private void delTmpFileInDCIM() {
		if (oldFiles != null) {
			File file = getLastFileInDCIM();
			if (file != null && file.exists()) {
				String lastImage = file.getName();
				if (!oldFiles.contains(lastImage)) {
					File imageFile = new File(getCameraDir() + "/" + lastImage);
					if (imageFile.exists()) {
						imageFile.delete();
					}
				}
			}
		}
	}

	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		if (resultCode == Activity.RESULT_OK) {
			// String envFile =
			// Environment.getExternalStorageDirectory().getAbsolutePath();
			Uri uri = null;
			Cursor cursor = null;
			String file = strImageFilePath;
			int path_index;

			String arg0 = "";
			int type = requestCode;
			switch (requestCode) {
			
			case REQUEST_CLIP_PHOTO:
				try {
					Uri cropUri = Uri.fromFile(new File(cropFilePath));
					if(cropUri==null){
						return;
					}
					String uripath = cropUri.getScheme();
//					Toast.makeText(activity_instance.getApplicationContext(), uripath, Toast.LENGTH_LONG).show();
					if(uripath.equals("file")){
						file = cropUri.getPath();
					}
					else{
						cursor = activity_instance.getContentResolver().query(cropUri, null, null, null,
								null);
						cursor.moveToFirst();
						path_index = cursor.getColumnIndexOrThrow(arg0);
						// String imgNo = cursor.getString(0); // 编号//
						file = cursor.getString(path_index); // 文件路径//
					}
					
					File image_file = new File(file);
					if(image_file.exists()){
						didFinishPickingMediaWithFile(REQUEST_CAPTURE_IMAGE, file);
						
					}
					else{
						byte[] bytes = null;
						Bitmap bitmap = FileOperator.compressJPG(file, 100);
						ByteArrayOutputStream out = new ByteArrayOutputStream();
						bitmap.compress(Bitmap.CompressFormat.JPEG, 100, out);
						bytes = out.toByteArray();
						
						didFinishPickingImageWithInfo(bytes, bytes.length);
					}
				} catch (Exception e) {
					// TODO: handle exception
				}
			break;
			case REQUEST_CAPTURE_IMAGE:
				
				if(mediaPickerType.allows_edit){
					cropImageUri(Uri.fromFile(new File(file)),500,500,REQUEST_CLIP_PHOTO);
				}
				else{
					try {
						File image_file = new File(file);
						if (!image_file.exists()) {
							Bundle extras = data.getExtras();
							Bitmap bmp = (Bitmap) extras.get("data");
							ByteArrayOutputStream out = new ByteArrayOutputStream();
							// 考虑到z11加载图片不能太大，压缩处理//
							bmp.compress(Bitmap.CompressFormat.JPEG, 100, out);
							byte[] bytes = out.toByteArray();
							didFinishPickingImageWithInfo(bytes, bytes.length);
//							delTmpFileInDCIM();
						} else {
							Bitmap bitmap = FileOperator.compressJPG(file, 100);
							ByteArrayOutputStream out = new ByteArrayOutputStream();
							bitmap.compress(Bitmap.CompressFormat.JPEG, 100, out);
							byte[] bytes = out.toByteArray();
							try {
								didFinishPickingImageWithInfoAndFile(file, bytes,
										bytes.length);
								delTmpFileInDCIM();
							} catch (UnsatisfiedLinkError e) {
								// TODO: handle exception
								e.printStackTrace();
							}
						}
					} catch (Exception e) {
						// TODO: handle exception
						e.printStackTrace();
					}
				}
				
				break;

			case REQUEST_CAPTURE_SOUND:
				{
					uri = data.getData();
					cursor = activity_instance.getContentResolver().query(uri, null, null,
							null, null);
					if (cursor.moveToNext()) {
						/* _data：文件的绝对路径 ，_display_name：文件名 */
						file = cursor.getString(cursor.getColumnIndex("_data"));
					}
				}

				try {
					didFinishPickingMediaWithFile(
							MGMediaPicker.MGMediaPickerSourceTypeSound, file);
					delTmpFileInDCIM();
				} catch (UnsatisfiedLinkError e) {
					// TODO: handle exception
					e.printStackTrace();
				}
				break;

			case REQUEST_CAPTURE_VIDEO:
				{
					uri = data.getData();
					cursor = activity_instance.getContentResolver().query(uri, null, null,
							null, null);
					if (cursor.moveToNext()) {
						/* _data：文件的绝对路径 ，_display_name：文件名 */
						file = cursor.getString(cursor.getColumnIndex("_data"));
					}
				}

				try {
					didFinishPickingMediaWithFile(
							MGMediaPicker.MGMediaPickerSourceTypeVideo, file);
					delTmpFileInDCIM();
				} catch (UnsatisfiedLinkError e) {
					// TODO: handle exception
					e.printStackTrace();
				}
				break;

			case REQUEST_VIEW_PHOTO_LIB:
				arg0 = MediaStore.Images.Media.DATA;
				type = MGMediaPicker.MGMediaPickerSourceTypePhotoLibrary;
				break;
			case REQUEST_VIEW_PHOTO_ALBUM:
				arg0 = MediaStore.Images.Media.DATA;
				type = MGMediaPicker.MGMediaPickerSourceTypeSavedPhotoAlbum;
				break;
			case REQUEST_VIEW_SOUND:
				arg0 = MediaStore.Audio.Media.DATA;
				type = MGMediaPicker.MGMediaPickerSourceTypeSoundLibrary;
				break;
			case REQUEST_VIEW_VIDEO:
				arg0 = MediaStore.Video.Media.DATA;
				type = MGMediaPicker.MGMediaPickerSourceTypeVideoLibrary;
				break;
			default:
				break;
			}
			if (requestCode == REQUEST_VIEW_PHOTO_LIB
					|| requestCode == REQUEST_VIEW_PHOTO_ALBUM
					|| requestCode == REQUEST_VIEW_SOUND
					|| requestCode == REQUEST_VIEW_VIDEO) {
				try {
					uri = data.getData();
					String uripath = uri.getScheme();
					if(uripath.equals("file")){
						file = uri.getPath();
					}
					else{
						cursor = activity_instance.getContentResolver().query(uri, null, null, null,
								null);
						cursor.moveToFirst();
						path_index = cursor.getColumnIndexOrThrow(arg0);
						// String imgNo = cursor.getString(0); // 编号//
						file = cursor.getString(path_index); // 文件路径//
					}
					
					if(mediaPickerType.allows_edit){
						cropImageUri(Uri.fromFile(new File(file)),500,500,REQUEST_CLIP_PHOTO);
					}
					else{
						
						byte[] bytes = null;
						if(requestCode==REQUEST_VIEW_PHOTO_ALBUM||requestCode==REQUEST_VIEW_PHOTO_LIB){
							//tmp_Bitmap = getimage(file);
							Bitmap bitmap = FileOperator.compressJPG(file, 100);
							ByteArrayOutputStream out = new ByteArrayOutputStream();
							bitmap.compress(Bitmap.CompressFormat.JPEG, 100, out);
							bytes = out.toByteArray();
						}
						
						try {
							if(requestCode==REQUEST_VIEW_PHOTO_ALBUM||requestCode==REQUEST_VIEW_PHOTO_LIB){
								didFinishPickingImageWithInfo(bytes, bytes.length);
							}else{
								didFinishPickingMediaWithFile(type, file);
							}
							
						} catch (UnsatisfiedLinkError e) {
							// TODO: handle exception
							e.printStackTrace();
						}
					}
				} catch (Exception e) {
					// TODO: handle exception
					e.printStackTrace();
				}
			}
		} else {
			try {
				didCancelPickingMedia(resultCode);
			} catch (UnsatisfiedLinkError e) {
				// TODO: handle exception
				e.printStackTrace();
			}
		}
	}
     
	@SuppressLint("InlinedApi")
	private void cropImageUri(Uri uri, int outputX, int outputY, int requestCode){
		    Intent intent = new Intent("com.android.camera.action.CROP");
		    intent.setDataAndType(uri, "image/*");
		    intent.putExtra("crop", "true");
		    intent.putExtra("aspectX", 1);
		    intent.putExtra("aspectY", 1);
		    intent.putExtra("outputX", outputX);
		    intent.putExtra("outputY", outputY);
		    intent.putExtra("scale", true);
		    intent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(new File(cropFilePath)));
		    intent.putExtra("return-data", false);
		    intent.putExtra("outputFormat", Bitmap.CompressFormat.JPEG.toString());
		    intent.putExtra("noFaceDetection", true); // no face detection
		    MGActivity.activity_instance.startActivityForResult(intent, requestCode);
		}
	
    public static native MGMediaPickerType getPickerType();
    
    public static native void didFinishPickingImageWithInfoAndFile(
			String filepath, byte[] data, int length);

	public static native void didFinishPickingImageWithInfo(byte[] data,
			int length);

	public static native void didFinishPickingMediaWithFile(int type,
			String filepath);

	public static native void didCancelPickingMedia(int type);
}
