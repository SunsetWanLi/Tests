package com.z11.mobile.framework;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.graphics.Bitmap;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.ThumbnailUtils;
import android.net.Uri;
import android.provider.MediaStore.Images.Thumbnails;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.FrameLayout;
import android.widget.MediaController;
import android.widget.VideoView;

public class MGMediaPlayer {
	private final static String TAG = "MGMediaPlayer";
	private static Map<String, MGAudioPlayer> audio_player_map = new HashMap<String, MGAudioPlayer>();
	private static Map<String, MGAudioPlayer> audio_pause_map = new HashMap<String, MGAudioPlayer>();

	public static native void audioPlayerDidFinishPlaying(String url);

	public static native void videoPlayerDidFinishPlaying(String url);

	public static native void videoPlayerDidFinishPlayingByUser(String url);
	
	public static native void getPreviewData(byte[] bytes, int length);
	
	public static native void setAudioDuration(String file_id, int len);
	public static void pauseMediaWhenOnPause() {
		if (MGVideoPlayer.isVideoPlayerShowing())
			MGVideoPlayer.appMinimize();
		audio_pause_map.clear();
		for (Map.Entry<String, MGAudioPlayer> entry : audio_player_map
				.entrySet()) {
			if (entry.getValue().isPlaying()) {
				audio_pause_map.put(entry.getKey(), entry.getValue());
			}
			entry.getValue().pause();
		}
	}

	public static void resumeMediaWhenOnResume() {
		if (MGVideoPlayer.isVideoPlayerShowing())
			MGVideoPlayer.resume();
		for (Map.Entry<String, MGAudioPlayer> entry : audio_pause_map
				.entrySet()) {
			entry.getValue().start();
		}
	}

	public static void initVideo(String filePath) {
		MGVideoPlayer.initWithFilePath(filePath);
	}
	public static void playVideo() {
		MGVideoPlayer.play();
	}

	public static void stopVideo() {
		MGVideoPlayer.stop();
	}
	public static void getPreviewImage(String file_path, int w , int h){
		MGVideoPlayer.getPreview(file_path, w, h);
	}
	public static void initAudio(String player_id, String file_path) {
		MGAudioPlayer mediaPlayer = new MGAudioPlayer();
		mediaPlayer.reset();
		if (audio_player_map.containsKey(player_id))
			return;
		audio_player_map.put(player_id, mediaPlayer);

		// prepare files
		File file = new File(file_path);
		FileInputStream fis = null;
		FileDescriptor fd = null;
		long start_offset = 0;
		long length = 0;
		try {
			fis = new FileInputStream(file);
			fd = fis.getFD();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			// e.printStackTrace();
			try {
				String asset_url = file_path
						.substring(("/data/data/"
								+ MGActivity.getZ11PackageName() + "/files/Android.App")
								.length() + 1);
				AssetFileDescriptor fileDescriptor = MGActivity.activity_instance
						.getAssets().openFd(asset_url);
				fd = fileDescriptor.getFileDescriptor();
				start_offset = fileDescriptor.getStartOffset();
				length = fileDescriptor.getLength();
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		// set data source
		try {
			if (start_offset == 0) {
				mediaPlayer.setDataSource(fd);
			} else {
				mediaPlayer.setDataSource(fd, start_offset, length);
			}
			mediaPlayer.prepare();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	public static void setDuration(String player_id, String file_path) {
		if (audio_player_map.containsKey(player_id))
		{
			MGAudioPlayer mediaPlayer = audio_player_map.get(player_id);
			setAudioDuration(player_id,mediaPlayer.getDuration());
		}
		else
		{
			MGAudioPlayer mediaPlayer = new MGAudioPlayer();
			mediaPlayer.reset();
			File file = new File(file_path);
			FileInputStream fis = null;
			FileDescriptor fd = null;
			long start_offset = 0;
			long length = 0;
			try {
				fis = new FileInputStream(file);
				fd = fis.getFD();
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				// e.printStackTrace();
				try {
					String asset_url = file_path
							.substring(("/data/data/"
									+ MGActivity.getZ11PackageName() + "/files/Android.App")
									.length() + 1);
					AssetFileDescriptor fileDescriptor = MGActivity.activity_instance
							.getAssets().openFd(asset_url);
					fd = fileDescriptor.getFileDescriptor();
					start_offset = fileDescriptor.getStartOffset();
					length = fileDescriptor.getLength();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			// set data source
			try {
				if (start_offset == 0) {
					mediaPlayer.setDataSource(fd);
				} else {
					mediaPlayer.setDataSource(fd, start_offset, length);
					
				}
				mediaPlayer.prepare();
				setAudioDuration(player_id,mediaPlayer.getDuration());
			} catch (Exception e) {
				e.printStackTrace();
			}
			mediaPlayer.release();
		}
	}
	
	public static void prepareAudio(String player_id, float pan, float volume,
			float rate, int number_of_loops) {
		MGAudioPlayer mediaPlayer = audio_player_map.get(player_id);
		if (mediaPlayer == null)
			return;

		mediaPlayer.setVolume(volume, volume);
		// if number_of_loops < 0, setLooping.
		mediaPlayer.setLooping(number_of_loops < 0);
		mediaPlayer.number_of_loops = number_of_loops;

		mediaPlayer
				.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
					public void onCompletion(MediaPlayer arg0) {
						MGAudioPlayer player = (MGAudioPlayer) arg0;
						if (player == null) {
							Log.e(TAG, "player = null");
							return;
						}

						String player_id = null;
						for (Map.Entry<String, MGAudioPlayer> entry : audio_player_map
								.entrySet()) {
							if (entry.getValue() == player) {
								player_id = entry.getKey();
								break;
							}
						}
						if (player_id == null) {
							Log.e(TAG,
									"player_id = null, can not find it in audio_player_map.");
							return;
						}

						// loop over
						if (player.number_of_loops == 0) {
							audioPlayerDidFinishPlaying(player_id);
						} else {
							player.number_of_loops -= 1;
							// player.reset();
							player.start();
						}
					}
				});
		mediaPlayer.setOnErrorListener(new MediaPlayer.OnErrorListener() {
			public boolean onError(MediaPlayer player, int arg1, int arg2) {
				Log.e(TAG, "setOnErrorListener");
				return false;
			}
		});
	}

	public static void playAudio(String player_id) {
		MediaPlayer mediaPlayer = audio_player_map.get(player_id);
		if (mediaPlayer == null || mediaPlayer.isPlaying())
			return;
		mediaPlayer.start();
		if (MGActivity.callState != TelephonyManager.CALL_STATE_IDLE) {
			pauseMediaWhenOnPause();
		}
	}

	public static void stopAudio(String player_id) {
		MediaPlayer mediaPlayer = (MediaPlayer) audio_player_map.get(player_id);
		if (mediaPlayer == null)
			return;
		mediaPlayer.stop();

		MediaPlayer mediaPausePlayer = (MediaPlayer) audio_player_map
				.get(player_id);
		if (mediaPausePlayer != null) {
			audio_pause_map.remove(player_id);
		}
	}

	public static void pauseAudio(String player_id) {
		MediaPlayer mediaPlayer = (MediaPlayer) audio_player_map.get(player_id);
		if (mediaPlayer == null)
			return;
		mediaPlayer.pause();
	}

	public static void uninitAudio(String player_id) {
		MediaPlayer mediaPlayer = (MediaPlayer) audio_player_map.get(player_id);
		if (mediaPlayer == null)
			return;
		mediaPlayer.stop();
		mediaPlayer.release();
		audio_player_map.remove(player_id);
	}
}

class MGAudioPlayer extends MediaPlayer {
	// 0 means play once
	public int number_of_loops;

	public MGAudioPlayer() {
		number_of_loops = 0;
	}
}

class MGVideoPlayer {
	private static final String TAG = "MGVideoPlayer";
	private static MGVideoView videoView = null;
	private static boolean isPlayingWhenAppMinimize = false;
	private static boolean isVideoPlayerShowing = false;
	private static int currentPosition = -1; // 初始状态 ///

	public static String filePath = "";

	public static boolean isVideoPlayerShowing() {
		return isVideoPlayerShowing;
	}

	public static boolean isPlayingWhenAppMinimize() {
		return isPlayingWhenAppMinimize;
	}

	public static void appMinimize() {
		currentPosition = videoView.getCurrentPosition();
		if (videoView.isPlaying()) {
			videoView.pause();
			isPlayingWhenAppMinimize = true;
		} else
			isPlayingWhenAppMinimize = false;
	}
	
	public static void getPreview(String file_path , int w, int h){
		Bitmap bitmap=ThumbnailUtils.createVideoThumbnail(file_path,Thumbnails.MINI_KIND);  
        Bitmap export_bitmap=ThumbnailUtils.extractThumbnail(bitmap,w,h);  
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        export_bitmap.compress(Bitmap.CompressFormat.PNG, 100, out);
		byte[] bytes = out.toByteArray();
		MGMediaPlayer.getPreviewData(bytes,bytes.length);
	}
	
	public static void stop() {
		setVideoPlayerVisible(false);
		currentPosition = -1;
		isVideoPlayerShowing = false;
		isPlayingWhenAppMinimize = false;
	}

	public static void play() {
		stop();
		if (videoView == null) {
			Log.i(TAG, "videoView = null !!!");
			return;
		}
		setVideoPlayerVisible(true);
		videoView.requestFocus();

		if (isVideoPlayerShowing) {
			if (currentPosition != -1) {
				videoView.seekTo(currentPosition);
				currentPosition = -1;
			}
			if (isPlayingWhenAppMinimize)
				videoView.start();
			else
				videoView.pause();
		} else {
			isVideoPlayerShowing = true;
			videoView.start();
		}
	}
	
	public static void resume()
	{
		if (videoView == null) {
			Log.i(TAG, "videoView = null !!!");
			return;
		}
		setVideoPlayerVisible(true);
		videoView.requestFocus();

		if (isVideoPlayerShowing) {
			if (currentPosition != -1) {
				videoView.seekTo(currentPosition);
				currentPosition = -1;
			}
			if (isPlayingWhenAppMinimize)
				videoView.start();
			else
				videoView.pause();
		} else {
			isVideoPlayerShowing = true;
			videoView.start();
		}
	}

	public static void initWithFilePath(String filePath) {
		if (videoView == null) {
			FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(
					LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT, Gravity.CENTER);

			videoView = new MGVideoView(MGActivity.activity_instance);
			MGActivity.activity_instance.addContentView(videoView, params);
			videoView.setMediaController(new MediaController(
					MGActivity.activity_instance));

			videoView.setOnCompletionListener(new OnCompletionListener() {
				// @Override
				public void onCompletion(MediaPlayer mp) {
					MGMediaPlayer
							.videoPlayerDidFinishPlaying(MGVideoPlayer.filePath);
				}
			});
			setVideoPlayerVisible(false);
		}

		MGVideoPlayer.filePath = filePath;
		File file = new File(filePath);
		if (!file.exists()) {
			String parentPath = "/data/user/10/" + MGActivity.getZ11PackageName()
					+ "/files/Android.App";
			String asset_url = filePath.substring(parentPath.length() + 1);

			// String videoFile = "file:///android_asset/" + asset_url;
			String videoFile = "android.resource://"
					+ MGActivity.getZ11PackageName() + "/raw/" + asset_url;
			videoView.setVideoURI(Uri.parse(videoFile));
		} else {
			videoView.setVideoPath(filePath);
		}
	}

	private static void setVideoPlayerVisible(boolean flag) {
		if(videoView == null)
			return;
		
		if (flag) {
			videoView.setVisibility(View.VISIBLE);
			MGGLSurfaceView._this.setVisibility(View.GONE);
		} else {
			videoView.setVisibility(View.GONE);
			MGGLSurfaceView._this.setVisibility(View.VISIBLE);
		}
	}
}

class MGVideoView extends VideoView {
	private static final String TAG = "MGVideoView";

	public MGVideoView(Context context) {
		super(context);
	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {
		Log.i(TAG, "onKeyDown code = " + String.valueOf(keyCode));
		if(keyCode == KeyEvent.KEYCODE_VOLUME_UP||keyCode==KeyEvent.KEYCODE_VOLUME_DOWN){
			return false;
		}
		if (keyCode == KeyEvent.KEYCODE_BACK) { // 监控/拦截/屏蔽返回键
			MGVideoPlayer.stop();
			MGMediaPlayer.videoPlayerDidFinishPlayingByUser(MGVideoPlayer.filePath);
			return true;
		}
		return true;
	}
}
