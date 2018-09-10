package com.z11.mobile.framework;

import java.io.File;
import java.io.RandomAccessFile;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import com.z11.mobile.framework.location.AddressTask;
import com.z11.mobile.framework.location.GpsTask.GpsData;
import com.z11.mobile.framework.location.IAddressTask.MLocation;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.location.LocationProvider;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.telephony.CellLocation;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.telephony.gsm.GsmCellLocation;
import android.util.Log;
import android.widget.Toast;

//1.要想获取位置信息,需要在手机设置中将"位置与安全"设为使能.//
//2.LocationManager提供了两个Provider(至少目前看来是两个):GPS_PROVIDER  & NETWORK_PROVIDER.可以在"位置与安全"里设置用哪个Provider来获取位置信息(使用GPS卫星OR使用无线网络).//
//3.在开发中可以用 isProviderEnabled(String Provider) 来判断哪一个Provider是使能的.//
//4.有人说用 getBestProvider(Criteria,boolean)来获取最佳Provider来使用,其实这个函数以精确度为首要判定标准,所以一般返回都是gps(经测试,似乎只有gps不使能,才返回network).//
//5.完全可以两个Provider一起使用,并且用一个Listener.//
public class MGLocationManager {
	private static Activity activity_instance = null;
	private static final String LOCAL_TAG = "MGLocationManager";
	private LocationManager mLocationManager=null;
	private Location mLastLocation = null;
	private final long minTime = 1000;		//1s
	private final float minDistance = 1;		//1m
	private final long sleepTime = 5000;		//5s
	private final long LOCATION_OUTOF_TIME = 5 * 60 * 1000;
	
	private static final int LOCATE_WITH_NETWORK = 1;
	private static String mDecimalFormat = "0.0000000";
	
	//private Set<String> unavailableProviderSet;
	//private Set<String> totalProviderSet;
	public Boolean isStop = false;
	private Boolean isPause = false;
	private Toast toast = null;
	
	private int connectionType = -1;
	private final int ConnectionTypeWifi = 0;
	private final int ConnectionTypeMobile = 1;
	
	private MLocation tmpLocation = null;
	private MLocation mNetworkLastLocation = null;
	
	private static Boolean hasShownDialog = false;
	
	public MGLocationManager(Activity activity) {
		activity_instance = activity;
		//totalProviderSet = new HashSet<String>();
		//unavailableProviderSet = new HashSet<String>();
	}
	
	public void onCreate(LocationManager _location_manager) {
		mLocationManager = _location_manager;
	}

	public void startLocationService()
	{
		isStop = false;
//		writeLogToSD("Start Location Service.");
		if (mLocationManager != null) {
			if (!mLocationManager.isProviderEnabled(android.location.LocationManager.GPS_PROVIDER)) {
				if(!hasShownDialog)
					openLocationDialog();
			}
			
			Location gps_last_location = mLocationManager.getLastKnownLocation(android.location.LocationManager.GPS_PROVIDER);
			Location net_last_location = mLocationManager.getLastKnownLocation(android.location.LocationManager.NETWORK_PROVIDER);
			
			if (gps_last_location != null)
			{
//				writeLogToSD("Last Location from Provider GPS : " + gps_last_location.toString());
				mLastLocation = gps_last_location;
			}
			if (net_last_location != null)
			{
//				writeLogToSD("Last Location from Provider NETWORK : " + net_last_location.toString());
				if (mLastLocation != null)
				{
					if (mLastLocation.getTime() < net_last_location.getTime())
					{
						mLastLocation = net_last_location;
//						writeLogToSD("Got Last Location from Provider NETWORK : " + mLastLocation.toString());
					} else {
//						writeLogToSD("Got Last Location from Provider GPS : " + mLastLocation.toString());
					}
				} else {
					mLastLocation = net_last_location;
//					writeLogToSD("Got Last Location from Provider NETWORK : " + mLastLocation.toString());
				}
			} else {
//				if (mLastLocation != null)
//					writeLogToSD("Got Last Location from Provider GPS : " + mLastLocation.toString());
			}
			
			if (mLocationManager.isProviderEnabled(android.location.LocationManager.NETWORK_PROVIDER)) {
				mLocationManager.requestLocationUpdates(
						android.location.LocationManager.NETWORK_PROVIDER, minTime, minDistance, mLocationListener);
//				writeLogToSD("Request Location Updates with Provider : " + android.location.LocationManager.NETWORK_PROVIDER);
			}
			if (mLocationManager.isProviderEnabled(android.location.LocationManager.GPS_PROVIDER)) {
				mLocationManager.requestLocationUpdates(
						android.location.LocationManager.GPS_PROVIDER, minTime, minDistance, mLocationListener);
//				writeLogToSD("Request Location Updates with Provider : " + android.location.LocationManager.GPS_PROVIDER);
			}
		}
		//根据上次位置进行定位//
		Boolean isUpdate = false;
		if (mLastLocation != null) {
			long time = mLastLocation.getTime();
			long curr_time = System.currentTimeMillis();
			if ((curr_time - time) <= LOCATION_OUTOF_TIME) {
				new AsyncTask<Void, Void, String>() {
					@Override
					protected String doInBackground(Void... arg0) {
						// TODO Auto-generated method stub
						updateWithNewLocation(mLastLocation);
						return null;
					}
		            
		        }.execute();
				isUpdate = true;
			}
		}
		
		//上次位置不可用进行wifi或apn定位//
		if (!isUpdate) 
		{
			toast = Toast.makeText(activity_instance.getApplicationContext(), 
					"正在定位...", Toast.LENGTH_LONG);//.show();
//			toast.show();
//			writeLogToSD("Locate With Network.");
			locateWithNetwork();
		}
		
		startNetLocation();
		
		IntentFilter filter = new IntentFilter();
		filter.addAction(WifiManager.SUPPLICANT_STATE_CHANGED_ACTION);
		activity_instance.registerReceiver(connectionReceiver, filter);
		
		TelephonyManager telephonyManager =(TelephonyManager) activity_instance.getSystemService(Context.TELEPHONY_SERVICE);
		telephonyManager.listen(celllistener, PhoneStateListener.LISTEN_CELL_LOCATION); // 基站位置的变化
	}
	
	//基站定位//
	private void do_mobile() {
        //异步
        new AsyncTask<Void, Void, String>() {

            @Override
            protected String doInBackground(Void... params) {
                MLocation location = null;
//                writeLogToSD("Begin Mobile Locate.");
                try {
                    //具体操作
                    location = new AddressTask(activity_instance, AddressTask.DO_MOBILE).doMobilePost(); 
                } catch (Exception e) {
                    e.printStackTrace();
                    location = null;
                }
//                if (location == null)
//                	writeLogToSD("End Mobile Locate without Result.");
//                else
//                	writeLogToSD("End Mobile Locate with Result : " + location.toString());
                tmpLocation = location;
                if(location == null)
                    return null;
                return location.toString();
            }

            //异步操作前执行dialog显示
            @Override
            protected void onPreExecute() {
//            	toast.show();
                super.onPreExecute();
            }

            //异步操作后显示相关信息并且关闭dialog
            @Override
            protected void onPostExecute(String result) {
//            	writeLogToSD("Mobile onPostExecute : " + result);
                if(result == null){
                	nativeUpdateFailWithError("基站定位失败了");
                	mNetworkLastLocation = null;
                } else {
                	if (mLastLocation != null) {
                		String old = getStringFromLocation(mLastLocation);
                		nativeUpdateWithNewLocation(result, old);
                	} else {
                		String old = result;
                		if (mNetworkLastLocation != null) {
                			old = mNetworkLastLocation.toString();
                		}
                		nativeUpdateWithNewLocation(result, old);
                	}
                	mNetworkLastLocation = tmpLocation;
                }
                try {
                    toast.cancel();
				} catch (NullPointerException e) {
					// TODO: handle exception
				}
                super.onPostExecute(result);
            }
            
        }.execute();
    }

	//gps定位,获取文字描述信息//
    private void do_gps(final GpsData gpsdata) {
//    	writeLogToSD("do_gps : " + gpsdata.toString());
        new AsyncTask<Void, Void, String>() {

            @Override
            protected String doInBackground(Void... params) {
                MLocation location = null;
                try {
                    //gps定位具体操作
                    location = new AddressTask(activity_instance,
                            AddressTask.DO_GPS).doGpsPost(gpsdata.getLatitude(),
                                    gpsdata.getLongitude());
                } catch (Exception e) {
                    e.printStackTrace();
                    location = null;
                }
                if(location == null)
                    return null;
                return location.toString();
            }

            @Override
            protected void onPreExecute() {
//            	toast.show();
                super.onPreExecute();
            }

            @Override
            protected void onPostExecute(String result) {
            	if(result.length() == 0){
                	nativeUpdateFailWithError("GPS信息获取错误");
                }else {
                }
            	try {
                    toast.cancel();
				} catch (NullPointerException e) {
					// TODO: handle exception
				}
                super.onPostExecute(result);
            }
            
        }.execute();
    }

    //wifi定位//
    private void do_wifi() {
        new AsyncTask<Void, Void, String>() {
            @Override
            protected String doInBackground(Void... params) {
                MLocation location = null;
//                writeLogToSD("Begin Wifi Locate.");
                try {
                    location = new AddressTask(activity_instance,
                            AddressTask.DO_WIFI).doWifiPost();
                } catch (Exception e) {
                    e.printStackTrace();
                    location = null;
                }
//                if (location == null)
//                	writeLogToSD("End Wifi Locate without Result.");
//                else
//                	writeLogToSD("End Wifi Locate with Result : " + location.toString());
                tmpLocation = location;
                if(location == null)
                    return null;
                return location.toString();
            }

            @Override
            protected void onPreExecute() {
//            	toast.show();
                super.onPreExecute();
            }

            @Override
            protected void onPostExecute(String result) {
//            	writeLogToSD("Wifi onPostExecute : " + result);
                if(result == null){
                	nativeUpdateFailWithError("Wifi定位失败了");
                	mNetworkLastLocation = null;
                } else {
                	if (mLastLocation != null) {
                		String old = getStringFromLocation(mLastLocation);
                		nativeUpdateWithNewLocation(result, old);
                	} else {
                		String old = result;
                		if (mNetworkLastLocation != null) {
                			old = mNetworkLastLocation.toString();
                		}
                		nativeUpdateWithNewLocation(result, old);
                	}
                	mNetworkLastLocation = tmpLocation;
                }
                try {
                    toast.cancel();
				} catch (NullPointerException e) {
					// TODO: handle exception
				}
                super.onPostExecute(result);
            }
        }.execute();
    }
		
	public void onPause(){
		isPause = true;
//		writeLogToSD("onPause and Remove Listener about Location.");
		try {
			activity_instance.unregisterReceiver(connectionReceiver);
			TelephonyManager telephonyManager =(TelephonyManager) activity_instance.getSystemService(Context.TELEPHONY_SERVICE);
			telephonyManager.listen(null, PhoneStateListener.LISTEN_CELL_LOCATION); // 基站位置的变化
			if (mLocationManager != null)
				mLocationManager.removeUpdates(mLocationListener);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void onDestroy() {
		isStop = true;
		isPause = true;
//		writeLogToSD("onDestroy and Remove Listener about Location.");
		try {
			activity_instance.unregisterReceiver(connectionReceiver);
			TelephonyManager telephonyManager =(TelephonyManager) activity_instance.getSystemService(Context.TELEPHONY_SERVICE);
			telephonyManager.listen(null, PhoneStateListener.LISTEN_CELL_LOCATION); // 基站位置的变化
			if (mLocationManager != null)
				mLocationManager.removeUpdates(mLocationListener);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private final LocationListener mLocationListener = new MyLocationListner();
	private class MyLocationListner implements LocationListener {
		public void onLocationChanged(Location location) {
//			writeLogToSD("Location Provider onLocationChanged : " + location.toString());
			stopNetLocation();
			updateWithNewLocation(location);
		}
		public void onProviderDisabled(String provider) {
//			writeLogToSD("Location Provider onProviderDisabled : " + provider);
			if (!hasAvailableProvider()) {
//				writeLogToSD("not hasAvailableProvider and startNetLocation.");
				startNetLocation();
				nativeUpdateFailWithError(provider + " disabled.");
    		}
		}
		public void onProviderEnabled(String provider) {
//			writeLogToSD("Location Provider onProviderEnabled : " + provider);
			if (hasAvailableProvider()) {
//				writeLogToSD("hasAvailableProvider and stopNetLocation.");
    			stopNetLocation();
    		}
		}
		public void onStatusChanged(String provider, int status, Bundle extras) {
//			writeLogToSD("Location Provider onStatusChanged : " + provider);
			updateWithNewStatus(provider, status, extras);
		}
    };
    
    private void updateWithNewLocation(Location location) {
    	String locationString;
    	String lastLocationString;
		//Log.v(LOCAL_TAG, "updateWithNewLocation");
    	if (mLastLocation == null) {
    		mLastLocation = location;
    	}
		if (location != null && mLastLocation != null) {
			Log.v(LOCAL_TAG, "longitude:"+location.getLongitude() + "\nlatitude:"+location.getLatitude());
			locationString = getStringFromLocation(location);
			lastLocationString = getStringFromLocation(mLastLocation);
			try {
				nativeUpdateWithNewLocation(locationString, lastLocationString);
			} catch (UnsatisfiedLinkError e) {
				// TODO: handle exception
				e.printStackTrace();
			}
		}
		mLastLocation = location;
    }
    private void updateWithNewStatus(String provider, int status, Bundle extras) {
    	if (mLocationManager == null)
    		return;
    	switch(status)
    	{
    	case LocationProvider.AVAILABLE:
//    		writeLogToSD("Location Provider update Status AVAILABLE : " + provider);
    		if (hasAvailableProvider()) {
//    			writeLogToSD("hasAvailableProvider and stopNetLocation.");
    			stopNetLocation();
    		}
    		break;
    	case LocationProvider.OUT_OF_SERVICE:
//    		writeLogToSD("Location Provider update Status OUT_OF_SERVICE : " + provider);
    		if (!hasAvailableProvider()) {
//    			writeLogToSD("not hasAvailableProvider and startNetLocation.");
    			startNetLocation();
    		}
    		nativeUpdateFailWithError(provider + " out of service.");
    		break;
    	case LocationProvider.TEMPORARILY_UNAVAILABLE:
//    		writeLogToSD("Location Provider update Status TEMPORARILY_UNAVAILABLE : " + provider);
    		if (!hasAvailableProvider()) {
//    			writeLogToSD("not hasAvailableProvider and startNetLocation.");
    			startNetLocation();
    		}
    		nativeUpdateFailWithError(provider + " temporarily unavailable.");
    		break;
		default:
//			writeLogToSD("Location Provider update Status default : " + provider);
			break;
    	}
    }
    private void locateWithNetwork() {
    	if (MGNetworkReachability.isWifiNetworkEnabled() == 1) {
//    		writeLogToSD("do_wifi().");
			do_wifi();
		}
    	if (mNetworkLastLocation == null && MGNetworkReachability.isCarrierNetworkEnabled() == 1) {
//    		writeLogToSD("do_mobile()");
			do_mobile();
    	}
    }
    private Boolean isNetLocationEnabled = false;
    private void startNetLocation() {
    	isNetLocationEnabled = true;
    }
    private void stopNetLocation() {
    	isNetLocationEnabled = false;
    }
    
    private String lastBSSID = "";
    private BroadcastReceiver connectionReceiver = new BroadcastReceiver() {	   
    	@Override
    	public void onReceive(Context context, Intent intent) {
    		if(intent.getAction().equals(WifiManager.SUPPLICANT_STATE_CHANGED_ACTION))
    		{
//    			writeLogToSD("WifiManager.SUPPLICANT_STATE_CHANGED_ACTION");
	    		if (!isStop && !isPause && isNetLocationEnabled) {
//	    			writeLogToSD("Ready to do_wifi.");
	        		if (MGNetworkReachability.isWifiNetworkEnabled() == 1) {
	        			WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
		    	        WifiInfo wifiInfo = wifiManager.getConnectionInfo();
		    	        if (wifiInfo != null) {
		    	        	String bssid = wifiInfo.getBSSID();
		    	        	if (bssid != null && lastBSSID != bssid) {
		    	        		lastBSSID = bssid;
//		    	        		writeLogToSD("Begin do_wifi.");
		        				do_wifi();
		    	        	}
		    	        } else {
//		    	        	writeLogToSD("wifiInfo is null.");
		    	        }
	        		} else {
//	        			writeLogToSD("Wifi not available now.");
	        		}
	        	}
    		}
    	}
    };
    
    private PhoneStateListener celllistener = new PhoneStateListener() {
        @Override
        public void onCellLocationChanged(CellLocation location) {
            super.onCellLocationChanged(location);
            // 判断 location的类型 是GsmCellLocation  还是  CdmaCellLocation
            // 最后 根据你的业务 需求 实现你的代码
            updateLocationOnCellLocationChanged();
        }
    };
    
    public void updateLocationOnCellLocationChanged() {
//    	writeLogToSD("Mobile updateLocationOnCellLocationChanged.");
    	if (!isStop && !isPause && isNetLocationEnabled) {
//    		writeLogToSD("Ready to do_mobile.");
    		if (MGNetworkReachability.isCarrierNetworkEnabled() == 1) {
//    	        writeLogToSD("Begin do_mobile.");
    			do_mobile();
    		} else {
//    			writeLogToSD("Mobile not available now.");
    		}
    	}
    }
	
    private Boolean hasAvailableProvider() {
    	Boolean ret = true;
    	ret = mLocationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
    	if (!ret)
    	{
//    		writeLogToSD(LocationManager.GPS_PROVIDER + " is not available.");
    		ret = mLocationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);
    	}
//    	if (!ret)
//    	{
//    		writeLogToSD(LocationManager.NETWORK_PROVIDER + " is not available.");
//    	}
    	return ret;
    }
    
    private String getStringFromLocation(Location location) {
    	if (location == null)
    		return mDecimalFormat+","+mDecimalFormat;
    	double locationLng = location.getLongitude();
    	double locationLat = location.getLatitude();
		java.text.DecimalFormat df = new java.text.DecimalFormat("#"+mDecimalFormat);
		StringBuilder resultX = new StringBuilder(df.format(locationLng));
		StringBuilder resultY = new StringBuilder(df.format(locationLat));
		
		String altitude = location.getAltitude()+"";
		String speed = location.getSpeed()+"";
		String bearing = location.getBearing()+"";
		String accuracy = location.getAccuracy()+"";
		String timestamp = location.getTime()+"";
		return resultX.toString()+","+resultY.toString()+","+
			altitude+","+speed+","+bearing+","+accuracy+","+timestamp;
	}

	private void openLocationDialog() {
		if (activity_instance == null)
			return;

		AlertDialog dialog = new AlertDialog.Builder(activity_instance)
				.setTitle("位置服务不可用")
				.setMessage("打开GPS卫星来获得位置信息")
				.setPositiveButton("设置", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int whichButton) {
						//activity_instance.setResult(Activity.RESULT_OK);// 确定按钮事件//
						activity_instance.startActivityForResult(new Intent(
								Settings.ACTION_LOCATION_SOURCE_SETTINGS), 0);
					}
				})
				.setNegativeButton("跳过", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int whichButton) {
						// 取消按钮事件//
					}
				}).create();
		dialog.show();
		
		hasShownDialog = true;
	}
    
    public static native void nativeUpdateWithNewLocation(String to_location, String from_location);
    public static native void nativeUpdateFailWithError(String error);
    
//    /**
//     * 写文件到sd卡上
//     * 
//     * @param context
//     */
//    public void writeLogToSD(String context) {
//    //使用RandomAccessFile 写文件 还是蛮好用的..推荐给大家使用...
//    	String sdStatus = Environment.getExternalStorageState();
//    	if (!sdStatus.equals(Environment.MEDIA_MOUNTED)) {
//    		Log.d("TestFile", "SD card is not avaiable/writeable right now.");
//    		return;
//    	}
//    	try {
//    		String pathName = "/sdcard/";
//    		String fileName = "AccidentReminder.log";
//    		File path = new File(pathName);
//    		File file = new File(pathName + fileName);
//    		if (!path.exists()) {
//    			Log.d("TestFile", "Create the path:" + pathName);
//    			path.mkdir();
//    		}
//    		if (!file.exists()) {
//    			Log.d("TestFile", "Create the file:" + fileName);
//    			file.createNewFile();
//    		}
//    		
//    		SimpleDateFormat formatter = new SimpleDateFormat("yyyy年MM月dd日 HH:mm:ss : ");     
//    		Date curDate = new Date(System.currentTimeMillis());//获取当前时间     
//    		String time_str = formatter.format(curDate);
//    		time_str = time_str + context + "\r\n";
//    		RandomAccessFile raf = new RandomAccessFile(file, "rw");
//    		raf.seek(file.length());
//    		raf.write(time_str.getBytes());
//    		raf.close();
//    	} catch (Exception e) {
//    		Log.e("TestFile", "Error on writeFilToSD.");
//    	}
//    }
}
