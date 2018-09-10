package com.cgrj.wenzhoutong;
import com.z11.mobile.framework.*;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;

import org.ksoap2.SoapEnvelope;
import org.ksoap2.serialization.SoapObject;
import org.ksoap2.serialization.SoapSerializationEnvelope;
import org.ksoap2.transport.HttpTransportSE;

import android.util.Log;

public class CityFetcherThread extends MGWebServiceThread {
	public String CITY_INFO = DOWNLOAD_DIRECTORY+"Android.App/CITY_DATA.TXT";
	
    public ArrayList<String> getWebServiceRes(String method , String[] key , String[] values){
        
        try{
         String SOAP_ACTION = webserviceNameSpace + method;
         SoapObject request  = new SoapObject(webserviceNameSpace, method);
   //      for(int i = 0 ;i<key.length;i++){
   //          request.addProperty(key[i], values[i]);
   //      }
         SoapSerializationEnvelope envelope=new SoapSerializationEnvelope(SoapEnvelope.VER11);
         envelope.bodyOut=request;
         envelope.dotNet=true;
         envelope.setOutputSoapObject(request);
         //HttpTransportSE ht = new HttpTransportSE(webserviceURL);
         MGAndroidHttpTransport ht = new MGAndroidHttpTransport(webserviceURL, 10000);
         ht.debug=true;
         Log.i("we","GETS");
         ht.call(SOAP_ACTION, envelope);
         Log.i("we","GETE");
     //    SoapPrimitive soap = (SoapPrimitive) envelope.getResponse(); 
         if (envelope.getResponse()!= null){
             SoapObject result=(SoapObject) envelope.bodyIn;
             SoapObject detail=(SoapObject) result.getProperty(method+"Result");
             ArrayList al = new ArrayList();
             Log.i("Tag", "here");
             try{
           	  Log.i("Tag", detail.getProperty(0).toString());
           	  for (int i=0;i<500;i++){
           		  Log.i("Check",""+i);
           		  al.add(detail.getProperty(i).toString());
           	  }
             }catch(Exception e){
           	  //return al;
            	 return al;
             }
             return al;
          }
         else 
       	  return null;
        }catch(Exception e){
       	 e.printStackTrace();
       	 return null;
        }
     }
    public void run(){
//  	 ArrayList cityRes=(ArrayList)getWebServiceRes("GetAllCity",null,null);
//  	 Log.i("BIS",""+cityRes.size());
//  	 File wfile = new File(WEATHER_INFO);
//  	 if (wfile.exists()) wfile.delete();
//  	 for (int i=0;i<cityRes.size();i++){
//  		 rq[0]=cityRes.get(i).toString();
//  		 rq[1]="";
//  		 getWebServiceResTwo("GetWeatherByPlace",ky,rq);
//  	 }
    	@SuppressWarnings("unchecked")
		ArrayList<String> cityRes=(ArrayList<String>)getWebServiceRes("GetAllCity",null,null);
    	
    	String log = "";
    	File cityinfo_file = new File(CITY_INFO);
//    	if (cityinfo_file.exists()) 
//    		cityinfo_file.delete();
    	if (!cityinfo_file.exists())
		{
			try {
				cityinfo_file.createNewFile();
			}
			catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		}
    	if (cityRes != null)
    	{
    		Log.i("BIS",""+cityRes.size());
	    	if(cityRes.size() != 0)
	    	{
				FileOutputStream fos;
				try {
					fos = new FileOutputStream(cityinfo_file);
		    		for(int i=0;i<cityRes.size();i++)
		    		{
		            	fos.write((cityRes.get(i).toString() + "\n")
								.getBytes());
		    		}
		    		fos.close();
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
	    	}
    	}
    	
    	MGMessageCommunication.JavaToNativeCommandAndData(8, "");
    }
}
