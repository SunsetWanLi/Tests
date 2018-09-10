package com.cgrj.wenzhoutong;
import com.z11.mobile.framework.*;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import org.ksoap2.serialization.SoapObject;

import android.util.Log;


public class TrafficThread extends MGWebServiceThread {
	public String TRAFFIC_INFO = DOWNLOAD_DIRECTORY+"Android.App/TRAFFIC_DATA.TXT";

	public String[] ky = { "license_plate", "start_time", "end_time" };
	public String[] rq = { "", "", "" };
	//private static TrafficThread m_traffic_thread;

	public TrafficThread(String plate, String sTime, String eTime) {
		rq[0] = plate;
		rq[1] = sTime;
		rq[2] = eTime;
		//m_traffic_thread = this;
	}
	
	public void run() {
		SoapObject result = (SoapObject) getWebServiceRes(
				"GetTrafficViolationByLicensePlate", ky, rq);
		File trafficInfoFile = new File(TRAFFIC_INFO);
		String log = "";
		//if (trafficInfoFile.exists())
			//trafficInfoFile.delete();
		if (!trafficInfoFile.exists())
		{
			try {
				trafficInfoFile.createNewFile();
			}
			catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		}
		
		if (result != null) {
			
			FileOutputStream fos;
			try {
				fos = new FileOutputStream(trafficInfoFile);
				if(result.getPropertyCount()==0)
				{
					log = "未查到相关信息";
					fos.write(log.getBytes());
				}
				else
				{
					for (int i = 0; i < result.getPropertyCount(); i++) {
						SoapObject each = (SoapObject) result.getProperty(i);
						for (int j = 0; j < each.getPropertyCount(); j++) {
							fos.write((each.getProperty(j).toString() + "\n")
									.getBytes());
						}
					}
				}
				fos.close();
			} catch (IOException e) {
				// IO失败
				e.printStackTrace();
			}
		}
		else
		{
			FileOutputStream fos;
			try {
				fos = new FileOutputStream(trafficInfoFile);
				log = "连接不到服务器,或输入有误";
				fos.write(log.getBytes());
				fos.close();
			} catch (IOException e) {
				// IO失败
				e.printStackTrace();
			}
		}
		Log.i("GetTrafficViolationByLicensePlate",
				"GetTrafficViolationByLicensePlate run over");
		MGMessageCommunication.JavaToNativeCommandAndData(4, "");
		//JavaToNativeCommand(1);
	}
	
	//public static native void JavaToNativeCommand(int code );
}
