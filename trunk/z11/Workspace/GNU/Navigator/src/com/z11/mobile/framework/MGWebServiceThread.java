/**
 * WebService线程，所以用到webservice的模块都要继承此类
 */
package com.z11.mobile.framework;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import org.apache.http.util.EncodingUtils;
import org.ksoap2.SoapEnvelope;
import org.ksoap2.serialization.SoapObject;
import org.ksoap2.serialization.SoapSerializationEnvelope;

import android.util.Log;

public class MGWebServiceThread extends Thread {
	//public String WEB_INFO = "/sdcard/温州通/web_config.txt";
	public String WEB_INFO = DOWNLOAD_DIRECTORY + "Android.App/web_config.txt";
	public static String webserviceURL = "http://192.168.2.244:80/IProService.asmx";
	//public static String webserviceURL = "http://122.228.224.200/IProService.asmx";
	//public static String webserviceURL = "http://cgrjhm.gicp.net/IProService.asmx";
	//public String webserviceURL = "http://122.224.74.82:8095/Test/IProService.asmx";
	//webservice命名空间
	public static String webserviceNameSpace = "http://tempuri.org/";
	public static int width = 240;
	public static int height = 320;
	
	public static String DOWNLOAD_DIRECTORY="/sdcard/Android.App/";
	
	public MGWebServiceThread()
	{
//		if (webserviceURL == null)
		{
			File webInfoFile = new File(WEB_INFO);
			if (!webInfoFile.exists())
			{ 
				try {
					webInfoFile.createNewFile();
				}
				catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
			}
			FileInputStream fos;
			try {
				fos = new FileInputStream(webInfoFile);
				int len = (int)webInfoFile.length();
				if(len>0)
				{
					byte[] buffer = new byte[len];
					fos.read(buffer);
					webserviceURL = EncodingUtils.getString(buffer, "ANSI");
				}
				fos.close();
				
			} catch (Exception e) {
				// TODO Auto-generated catch block
				webserviceURL = "http://cgrjhm.gicp.net/IProService.asmx";
				e.printStackTrace();
			}
		}
	}
	
	public static void SetPixelSize(int _width,int _height)
	{
		width=_width;
		height=_height;
	}
    public Object  getWebServiceRes(String method , String[] key , String[] values){
        
        try{
         String SOAP_ACTION = webserviceNameSpace + method;
         SoapObject request  = new SoapObject(webserviceNameSpace, method);
         for(int i = 0 ;i<key.length;i++){
             request.addProperty(key[i], values[i]);
         }
         SoapSerializationEnvelope envelope=new SoapSerializationEnvelope(SoapEnvelope.VER11);
         envelope.bodyOut=request;
            envelope.dotNet=true;
            envelope.setOutputSoapObject(request);
            MGAndroidHttpTransport ht = new MGAndroidHttpTransport(webserviceURL, 20000);
            ht.debug=true;
           Log.i("getWebServiceRes","GETStart");
            ht.call(SOAP_ACTION, envelope);
            Log.i("getWebServiceRes","GETEnd");
            Object  soap =  envelope.getResponse();
            if(soap==null)
            	Log.i("getWebServiceRes","soap==null");
            else
            	Log.i("getWebServiceRes",soap.toString());
            	
            return soap;
        }catch(Exception e){
         e.printStackTrace();
         return null;
        }
     } 
}