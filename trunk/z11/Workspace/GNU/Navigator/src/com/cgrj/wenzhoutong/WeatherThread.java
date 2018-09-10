package com.cgrj.wenzhoutong;

import com.z11.mobile.framework.*;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import org.ksoap2.serialization.SoapObject;

import android.util.Log;

public class WeatherThread extends MGWebServiceThread {
    public String WEATHER_INFO = DOWNLOAD_DIRECTORY+"Android.App/WEATHER_DATA.TXT";
    
    
    public String[] ky={"place_name","data_time"};
    public String[] rq={"温州","2011-4-21"};
    public WeatherThread(String city_name,String data_time)
    {
    	rq[0]=city_name;
    	rq[1]=data_time;
    }
 
    public void run(){
    	SoapObject  result=(SoapObject)getWebServiceRes("GetWeatherByPlace",ky,rq);
    	File weatherInfoFile = new File(WEATHER_INFO);
    	String log = "";
		Log.i("WeatherThread","save file: "+WEATHER_INFO);
//		if (weatherInfoFile.exists())
//			weatherInfoFile.delete();
		//if (weatherInfoFile.exists())
		//{
		//	weatherInfoFile.delete();
		//}
		if (!weatherInfoFile.exists())
		{
			try {
				weatherInfoFile.createNewFile();
			}
			catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		}
    	if(result!=null)
    	{
    		
			FileOutputStream fos;
			try {
				fos = new FileOutputStream(weatherInfoFile);
				if(result.getPropertyCount()==0)
				{
					log = "未查到相关信息";
					fos.write(log.getBytes());
				}
				else
				{
		    		for(int i=0;i<result.getPropertyCount();i++)
		    		{
		            	fos.write((result.getProperty(i).toString() + "\n")
								.getBytes());
		    		}
	    		}
	    		fos.close();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
    	}
    	else
    	{
			FileOutputStream fos;
			try {
				fos = new FileOutputStream(weatherInfoFile);
				log = "获取超时,或输入有误";
				fos.write(log.getBytes());
	    		fos.close();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
    	}
    	MGMessageCommunication.JavaToNativeCommandAndData(5, "");
	}
	
	//public static native void JavaToNativeCommand(int code );
}
