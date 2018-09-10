package com.z11.mobile.framework.location;

import java.util.Timer;
import java.util.TimerTask;
import android.app.Activity;
import android.content.Context;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

@SuppressWarnings("rawtypes")
public class GpsTask extends AsyncTask {

	public static class GpsData {
	    private double latitude = 0;
	    private double longitude = 0;
	    private float accuracy = 0;
	    private double altitude = 0;
	    private float bearing = 0;
	    private float speed = 0;
	    private long time = 0;

	    public double getLatitude() {
	        return latitude;
	    }
	    public void setLatitude(double latitude) {
	        this.latitude = latitude;
	    }
	    public double getLongitude() {
	        return longitude;
	    }
	    public void setLongitude(double longitude) {
	        this.longitude = longitude;
	    }
	    public float getAccuracy() {
	        return accuracy;
	    }
	    public void setAccuracy(float accuracy) {
	        this.accuracy = accuracy;
	    }
	    public double getAltitude() {
	        return altitude;
	    }
	    public void setAltitude(double altitude) {
	        this.altitude = altitude;
	    }
	    public float getBearing() {
	        return bearing;
	    }
	    public void setBearing(float bearing) {
	        this.bearing = bearing;
	    }
	    public float getSpeed() {
	        return speed;
	    }
	    public void setSpeed(float speed) {
	        this.speed = speed;
	    }
	    public long getTime() {
	        return time;
	    }
	    public void setTime(long time) {
	        this.time = time;
	    }
	}
	public interface GpsTaskCallBack {

	    public void gpsConnected(GpsData gpsdata);
	    
	    public void gpsConnectedTimeOut();
	}
	
    private GpsTaskCallBack callBk = null;
    private Activity context = null;
    private LocationManager locationManager = null;
    private LocationListener locationListener = null;
    private Location location = null;
    private long minTime = 1000;		//1s
	private float minDistance = 0;		//0m
	
    private boolean TIME_OUT = false;
    private boolean DATA_CONNTECTED = false;
    private long TIME_DURATION = 5000;
    private GpsHandler handler = null;
    private final int GPS_CONNECTED = 0;
    private final int GPS_CONNECTED_TIMEOUT = 1;
    private Timer timer = null;
            private class GpsHandler extends Handler {
        
                @Override
                public void handleMessage(Message msg) {
                    super.handleMessage(msg);
                    if(callBk == null)
                        return;
                    switch (msg.what) {
                    case GPS_CONNECTED:
                    	if (timer != null) {
                    		timer.cancel();
                    		timer = null;
                    	}
                        callBk.gpsConnected((GpsData)msg.obj);
                        break;
                    case GPS_CONNECTED_TIMEOUT:
                        callBk.gpsConnectedTimeOut();
                        break;
                    }
                }
                
            }

    public GpsTask(Activity context, GpsTaskCallBack callBk) {
        this.callBk = callBk;
        this.context = context;
        gpsInit();
    }

    public GpsTask(Activity context, GpsTaskCallBack callBk, long time_out) {
        this.callBk = callBk;
        this.context = context;
        this.TIME_DURATION = time_out;
        gpsInit();
    }

    private void gpsInit() {
        locationManager = (LocationManager) context
                .getSystemService(Context.LOCATION_SERVICE);
        handler = new GpsHandler();
        if(locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)) {
            
        } else {
        	//GPS不可用
            TIME_OUT = true;
        }
        locationListener = new LocationListener() {

            @Override
            public void onStatusChanged(String provider, int status,
                    Bundle extras) {
            }

            @Override
            public void onProviderEnabled(String provider) {
            }

            @Override
            public void onProviderDisabled(String provider) {
            }

            //位置改变才会调用
            @Override
            public void onLocationChanged(Location l) {
                DATA_CONNTECTED = true;
                Message msg = handler.obtainMessage();
                msg.what = GPS_CONNECTED;
                msg.obj = transData(l);
                handler.sendMessage(msg);
            }
        };
        locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, minTime, minDistance, locationListener);
    }

    @Override
    protected Object doInBackground(Object... params) {
        while (!TIME_OUT && !DATA_CONNTECTED) {
            location = locationManager
                    .getLastKnownLocation(LocationManager.GPS_PROVIDER);
            if (location != null && callBk != null) {
                //定位成功//
                Message msg = handler.obtainMessage();
                msg.what = GPS_CONNECTED;
                msg.obj = transData(location);
                handler.sendMessage(msg);
                break;
            }
        }
        return null;
    }

    @Override
    protected void onPreExecute() {
        super.onPreExecute();
        //Timer是自动另外开的一条线程
        timer = new Timer();
        timer.schedule(new TimerTask() {

            @Override
            public void run() {
                TIME_OUT = true;
            }
        }, TIME_DURATION);
    }

    @SuppressWarnings("unchecked")
    @Override
    protected void onPostExecute(Object result) {
        locationManager.removeUpdates(locationListener);
        //定位失败//
        if (TIME_OUT && callBk != null){
            handler.sendEmptyMessage(GPS_CONNECTED_TIMEOUT);
        }
        super.onPostExecute(result);
    }

    private GpsData transData(Location location) {
        GpsData gpsData = new GpsData();
        gpsData.setAccuracy(location.getAccuracy());
        gpsData.setAltitude(location.getAltitude());
        gpsData.setBearing(location.getBearing());
        gpsData.setLatitude(location.getLatitude());
        gpsData.setLongitude(location.getLongitude());
        gpsData.setSpeed(location.getSpeed());
        gpsData.setTime(location.getTime());
        return gpsData;
    }
}