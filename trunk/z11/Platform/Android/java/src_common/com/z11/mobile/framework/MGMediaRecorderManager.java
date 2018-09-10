package com.z11.mobile.framework;

import java.io.File;
import java.io.IOException;
import android.media.MediaRecorder;
import android.os.Environment;
import android.util.Log;

public class MGMediaRecorderManager {
	public static MediaRecorder mRecorder = null;

	private static String sdcardDir = null;
	private static String systemDir = null;
	private static float m_max_channel_value = (float) 32767.0;
	public static void stop() {
		if (mRecorder != null) {
			mRecorder.stop();
			mRecorder.release();
			mRecorder = null;
		}
	}

	public static void start(String full_path) {
		try {
			//initDir();
			if (mRecorder != null) {
				Log.i("mRecorder", "please stop first!!!");
				return;
			}
			mRecorder = new MediaRecorder();
			mRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
			mRecorder.setOutputFormat(MediaRecorder.OutputFormat.DEFAULT);
			mRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.DEFAULT);

//			String full_path = systemDir + file_name;
//			Log.i("file_full_path", full_path);
			File recordFile = new File(getPathPrefix(full_path));

			if (!recordFile.exists())
				recordFile.mkdirs();
			mRecorder.setOutputFile(full_path);

			mRecorder.prepare();
			mRecorder.start();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public static native void nativeUpdateMeters(float value);
	public static void updateMeters(){
		if(mRecorder != null){
			nativeUpdateMeters((float)((float)mRecorder.getMaxAmplitude()/m_max_channel_value));
		}
	}
	private static void initDir() {
		if (systemDir != null)
			return;

		String sub_path = File.separator + "Android.App" + File.separator;
		boolean sdCardExist = Environment.getExternalStorageState().equals(
				android.os.Environment.MEDIA_MOUNTED); // 判断sd卡是否存在
		if (sdCardExist) {
			sdcardDir = Environment.getExternalStorageDirectory()
					+ File.separator + "android" + File.separator + "data"
					+ File.separator
					+ MGActivity.activity_instance.getPackageName()
					+ File.separator + "files" + sub_path;
			Log.i("sdcardDir", sdcardDir);
		}

		systemDir = MGActivity.activity_instance.getFilesDir().toString()
				+ sub_path;
		Log.i("systemDir", systemDir);
	}

	private static String getPathPrefix(String file_path) {
		String file_path_prefix = file_path.substring(0,
				file_path.lastIndexOf(File.separatorChar));
		return file_path_prefix;
	}
}