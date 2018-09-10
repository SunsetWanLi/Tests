package com.z11.mobile.framework;

import java.io.IOException;
import java.io.InputStream;

import android.content.res.AssetManager;

public class MGAssetManager {
	static AssetManager assetManager;
	public static void setAssetManager(AssetManager _assetManager)
	{
		assetManager = _assetManager;
	}
	public static byte[] readFileFromAssetManager(String asset_file_path)
	{
		byte[]  buffer =null;
		if(assetManager!=null)
		{
			try {
				InputStream in = assetManager.open(asset_file_path);
				//获取文件大小 //
				int lenght = in.available();
				buffer= new byte[lenght];
				in.read(buffer);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		return buffer;
	}
	//public native 
	//public static 
}
