package com.z11.mobile.framework;

import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import android.R.integer;
import android.R.xml;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;

public class MGSensorManager implements SensorEventListener{
	private final double GravityAcceleration = 9.81;
    private SensorManager mSensorManager = null;
    //Heading
    private double mMagneticHeading = 0;
    private double mTrueHeading = 0;
    private double mAccuracy = 0;
    private double mHeadingX = 0;
    private double mHeadingY = 0;
    private double mHeadingZ = 0;
    //Gyroscope
    private double mRotationX = 0;
    private double mRotationY = 0;
    private double mRotationZ = 0;
    //Accelerometer
    private double mAccelerationX = 0;
    private double mAccelerationY = 0;
    private double mAccelerationZ = 0;
    private int sensor_tag = 0;
    void addSensorTag(int val)
    {
    	sensor_tag|=val;
    }
    void removeSensorTag(int val)
    {
    	if((sensor_tag&val)!=0)
    	{
    		sensor_tag^=val;
    	}
    }
	private static Map<String, MGAudioPlayer> audio_pause_map = new HashMap<String, MGAudioPlayer>();
	public void onCreate(SensorManager sensorManager) {
		mSensorManager = sensorManager;
	}
	
	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// TODO Auto-generated method stub
		mAccuracy = accuracy;
	}
	
	@Override
	public void onSensorChanged(SensorEvent event) {
		// TODO Auto-generated method stub
		float[] value = event.values;
		int sensorType = event.sensor.getType();
		mAccuracy = event.accuracy;
		switch (sensorType) 
		{
			//方向传感器//
			case Sensor.TYPE_ORIENTATION:
				mHeadingX = value[SensorManager.DATA_X];
				mHeadingY = value[SensorManager.DATA_Y];
				mHeadingZ = value[SensorManager.DATA_Z];
				mTrueHeading = mHeadingX;
				try {
					if((sensor_tag&1)!=0)
					{
						nativeUpdateWithNewHeading(mMagneticHeading, mTrueHeading, mAccuracy, mHeadingX, mHeadingY, mHeadingZ);
					}
					if((sensor_tag&2)!=0)
					{
						double yaw,pitch,roll;
						yaw=mHeadingX;
						pitch=mHeadingY;
						roll=mHeadingZ;
						if(pitch>90)
						{
							pitch=180-pitch;
						}
						else if(pitch<-90)
						{
							pitch=-180-pitch;
						}
						nativeUpdateWithNewAttitude(yaw, pitch, roll);
					}
				} catch (UnsatisfiedLinkError e) {
					// TODO: handle exception
					e.printStackTrace();
				}
				break;
			//磁场传感器// 
			case Sensor.TYPE_MAGNETIC_FIELD:
				break;
			//温度传感器//  
			case Sensor.TYPE_TEMPERATURE:
				break;
			//光传感器//  
			case Sensor.TYPE_LIGHT:
				break;
			//陀螺仪传感器// 
			case Sensor.TYPE_GYROSCOPE:
				mRotationX = value[SensorManager.DATA_X];
				mRotationY = value[SensorManager.DATA_Y];
				mRotationZ = value[SensorManager.DATA_Z];
				try {
					nativeUpdateWithNewRotation(mRotationX, mRotationY, mRotationZ);
				} catch (UnsatisfiedLinkError e) {
					// TODO: handle exception
					e.printStackTrace();
				}
				break;
			//加速度传感器// 
			case Sensor.TYPE_ACCELEROMETER:
				mAccelerationX = value[SensorManager.DATA_X];
				mAccelerationY = value[SensorManager.DATA_Y];
				mAccelerationZ = value[SensorManager.DATA_Z];
				try {
					nativeUpdateWithNewAcceleration(-mAccelerationX / GravityAcceleration, -mAccelerationY / GravityAcceleration, -mAccelerationZ / GravityAcceleration);
				} catch (UnsatisfiedLinkError e) {
					// TODO: handle exception
					e.printStackTrace();
				}
				break;
			//接近传感器//
			case Sensor.TYPE_PROXIMITY:
				float distance=value[0];
				Log.e("Distance", ""+distance);
				break;
			//压力传感器//
			case Sensor.TYPE_PRESSURE:
				break;
			default:
				break;
		}		
	}
	public void onResume() {
	}
	public void onPause() {
	}
	public void startWithType(int sensorType) {
		try {
			mSensorManager.registerListener(this, 
	        		mSensorManager.getDefaultSensor(sensorType), 
	        		SensorManager.SENSOR_DELAY_GAME);
		} catch (NullPointerException e) {
			// TODO: handle exception
			e.printStackTrace();
		}
	}
	public void stopWithType(int sensorType) {
		try {
			Sensor sensor = mSensorManager.getDefaultSensor(sensorType);
			boolean tag=true;
			if(sensorType==Sensor.TYPE_ORIENTATION&&sensor_tag!=0)
			{
				tag=false;
			}
			if(tag)
			{
				mSensorManager.unregisterListener(this, sensor);
			}
		} catch (NullPointerException e) {
			// TODO: handle exception
			e.printStackTrace();
		}
	}
	public static native void nativeUpdateWithNewAttitude(double yaw,double pitch,double roll);
	public static native void nativeUpdateWithNewHeading(double magnetic_heading, double true_heading, double heading_accuracy, double x, double y, double z);
	public static native void nativeUpdateWithNewRotation(double x, double y, double z);
	public static native void nativeUpdateWithNewAcceleration(double x, double y, double z);
}
