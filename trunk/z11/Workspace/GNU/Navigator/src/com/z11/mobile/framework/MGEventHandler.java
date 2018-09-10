package com.z11.mobile.framework;

import android.util.Log;
import android.view.inputmethod.InputMethodManager;

import com.cgrj.wenzhoutong.CityFetcherThread;
import com.cgrj.wenzhoutong.MapThread;
import com.cgrj.wenzhoutong.TrafficThread;
import com.cgrj.wenzhoutong.WeatherThread;

public class MGEventHandler {
	public static void handle(int code ,String data)
	{
		String[] splits=data.split("%");
    	switch(code)
    	{
    	case 0:
    		DemoActivity._this.onSendMessage(data);//¥¶¿ÌHandleEvnet
    		break;
    	case 1:
    	{
    		double x,y,scale;
    		if(splits.length==3)
    		{
    			x=Double.parseDouble(splits[0]);
    			y=Double.parseDouble(splits[1]);
    			scale=Double.parseDouble(splits[2]);
        		MGEventHandler.ReloadMap(x, y, scale);
    			
    		}
    	}
    		break;
    	case 2:
    		MGEventHandler.ConnectVideoServer(data);
    		break;
    	case 3:
    	{
    		if(splits.length==3)
    		{
    			MGEventHandler.SetTrafficInfo(splits[0],splits[1],splits[2]);
    		}
    	}
    		break;
    	case 4:
    	{
    		if(splits.length==2)
    		{
    			MGEventHandler.SetWeatherInfo(splits[0],splits[1]);
    		}
    	}
    	break;
    	case 5:
    		MGEventHandler.ReloadCity();
    		break;
    	case 6:
    		DemoGLSurfaceView.ToggleSoftInput();
    		break;
    	case 7:
    		DemoGLSurfaceView.HideSoftInput();
    		break;
    	default:
    		Log.d("MGMessageCommunication","Unknow command code");
    		break;
    	}
	}
    public static void ReloadMap(double x,double y,double scale){
     	Log.i("DGL","ReloadMap "+x+" , "+y+" , "+scale);
     	MapThread t = new MapThread(x, y, scale);
 		t.start();
    }
    
    public static void ConnectVideoServer(String puid)
    {
    	if(DemoActivity._this!=null)
    		ConnectVideoServer.connectVideoServer(puid);
    }
    
    public static void SetTrafficInfo(String plate, String sTime, String eTime)
	{
		Log.i("TAG","SetTrafficInfo " + plate + " , " + sTime + " , " + eTime);
		TrafficThread t = new TrafficThread(plate, sTime, eTime);
 		t.start();
	}
    
    public static void SetWeatherInfo(String city_name,String data_time)
	{
		Log.i("TAG","SetWeatherInfo " + city_name + " , " + data_time);
		WeatherThread t = new WeatherThread(city_name, data_time);
 		t.start();
	}
    public static void ReloadCity()
    {
    	Log.i("TAG","ReloadCity ");
    	CityFetcherThread t = new CityFetcherThread();
 		t.start();
    }
    

}
