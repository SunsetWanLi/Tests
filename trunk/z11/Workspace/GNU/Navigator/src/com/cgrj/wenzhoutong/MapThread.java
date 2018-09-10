package com.cgrj.wenzhoutong;

import com.z11.mobile.framework.*;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;

import org.ksoap2.serialization.SoapObject;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

public class MapThread extends MGWebServiceThread implements FilenameFilter {
    public String[] ky={"x","y","scale","pic_w","pic_h","dpi"};
    public String[] rq={"120.666","28.016","0.0001","3","4","80"};
    public String IMAGE_DIRECTORY=DOWNLOAD_DIRECTORY+"Android.App/";
    public String TEMP_IMAGE_DIRECTORY=DOWNLOAD_DIRECTORY+"tmp/";
	public MapThread(double x,double y,double scale)
	{
		rq[0] = Double.toString(x).substring(0, 7);
		rq[1] = Double.toString(y).substring(0, 6);
		rq[2] = Double.toString(scale);
		//rq[3] = Integer.toString(width/80);
		//rq[4] = Integer.toString(height/80);
		//IMAGE_DIRECTORY="/data/data/com.z11.mobile.framework/files/Android.App/";
		//TEMP_IMAGE_DIRECTORY="/data/data/com.z11.mobile.framework/files/tmp/";
		rq[5] = "80";
		
	}
	
	public boolean accept(File dir,
            String name)
	{
		if(name.length()>=7)
		{
			if(name.substring(0, 3).compareTo("map")==0)
				return true;
		}
		return false;
	}
	
	public Bitmap parseRealString(String data) {
		Bitmap bmp = null;
		byte[] buf = new byte[data.length() / 2];
		for (int i = 0; i < buf.length; i++) {
			String item = data.substring(i * 2, (i + 1) * 2);
			buf[i] = (byte) Integer.parseInt(item, 16);
		}
		try {
			bmp = BitmapFactory.decodeByteArray(buf, 0, buf.length);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return bmp;
	}

	public void saveMyBitmap(String bitName, Bitmap mBitmap) throws IOException {
		if (null == mBitmap)
			return;

		File f = new File(bitName);
		if (!f.exists())
		{
			try {
				f.createNewFile();
			}
			catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		}
		FileOutputStream fOut = null;
		try {
			fOut = new FileOutputStream(f);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
		mBitmap.compress(Bitmap.CompressFormat.JPEG, 100, fOut);

		try {
			fOut.close();
		} catch (IOException e) {
			e.printStackTrace();
		}

		// 删除原有文件
		String[] deleteFiles=(new File(TEMP_IMAGE_DIRECTORY)).list(this);
		if(deleteFiles!=null)
		{
			for(int i=0;i<deleteFiles.length;i++)
			{
				Log.i("filter deleteFiles",TEMP_IMAGE_DIRECTORY+deleteFiles[i]);
				(new File(TEMP_IMAGE_DIRECTORY+deleteFiles[i])).delete();
			}
		}
		
		File tmp = new File(IMAGE_DIRECTORY+"map.jpg");
		if (tmp.exists())
		{
			tmp.delete();
		}
		f.renameTo(tmp);
		Log.i("Demo", "saveMyBitmap OK");
		MGMessageCommunication.JavaToNativeCommandAndData(1,"");
		//DemoGLSurfaceView._this.mRenderer.bRender = 1;
	}
	
	public void run(){
    	String ttx = null;
    	for (int i=0;i<1;i++){
    		Log.i("DemoActivity lat",rq[0]+","+rq[1]+","+rq[2]);
    		Log.i("TAG", "before getWebServiceRes");
    		Object ret=(getWebServiceRes("GetRealTimeTrafficByScaleAndCenter",ky,rq));
    		Log.i("TAG", "getWebServiceRes");
    		if(ret!=null)
    		{
    			Log.i("TAG", "getWebServiceRes successfuly");
        		ttx = ret.toString();
        		if(ttx!="NoData")
        		{
            	      try {
            			saveMyBitmap(IMAGE_DIRECTORY+"tempmap.jpg",parseRealString(ttx));
            		} catch (IOException e) {
            			// TODO Auto-generated catch block
            			e.printStackTrace();
            		}
        		}
        		else
        		{
        			DemoActivity._this.copyFile(IMAGE_DIRECTORY+"map.jpg", "drawable-ldpi/connect_failed.jpg");
        		}
    		}
    		else
    		{
    			DemoActivity._this.copyFile(IMAGE_DIRECTORY+"map.jpg", "drawable-ldpi/connect_failed.jpg");
    		}
     	}
    	String p1[]=new String[0];
    	String p2[]=new String[0];
    	SoapObject  result=(SoapObject)getWebServiceRes("ExistCamera",p1,p2);
    	MGMessageCommunication.JavaToNativeCommandAndData(3,"");
    	if(result!=null)
    	{
    		Object value=null;
    		for(int i=0;i<result.getPropertyCount();i++)
    		{
    			value= result.getProperty(i);
            	String res=  value.toString();
            	Log.i("TAG", "ExistCamera get res = "+res);
            	MGMessageCommunication.JavaToNativeCommandAndData(2, res);
            	Log.i("TAG", "after JavaToNativeCommandAndData 2");
    		}
    	}
    	Log.i("tag","run over");
     }
}
