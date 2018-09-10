package com.z11.mobile.framework;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.StringReader;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;


public class FileOperator {
    /** Called when the activity is first created. */
	public static byte[] readStringFromFile(String filepath) {
		byte[] ret = null;
		File cmdInfoFile = new File(filepath);
		if (!cmdInfoFile.exists()) {
			return ret;
		}
		FileInputStream fis;
		try {
			fis = new FileInputStream(cmdInfoFile);
			int len = (int)cmdInfoFile.length();
			if(len>0) {
				//byte[] buffer = new byte[len];
				ret = new byte[len];
				if (fis != null)
					fis.read(ret);
				//ret = EncodingUtils.getString(buffer, "ANSI");
			}
			if (fis != null)
				fis.close();
			
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}
		return ret;
	}
	public static boolean copyFile(String outfile, String infile) {
		boolean ret = true;
    	InputStream in;
		OutputStream out;
		try {
			File inputFile = new File(infile);
			//File inputFile = new File(Environment.getExternalStorageDirectory(), infile);
			if (!inputFile.exists()) {
				return false;
			}
			File outFile=new File(outfile);
			if(outFile.exists()) {
				outFile.delete();
				outFile.createNewFile();
//				return ;
			}
			out = new FileOutputStream(outFile);
			in = new FileInputStream(inputFile);
			// Transfer bytes from in to out
			byte[] buf = new byte[1024];
			int len;
			while ((len = in.read(buf)) > 0) {
				out.write(buf, 0, len);
			}

			in.close();
			out.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			ret = false;
		}
		return ret;
    }
	public static boolean writeStringToFile(String filename, String content) {
		return writeFile(filename, content, false);
	}
	
	public static boolean apendStringToFile(String filename, String content) {
		return writeFile(filename, content, true);
	}
	
	private static boolean writeFile(String filename, String content, boolean flag) {
		boolean ret = true;
		try {
			BufferedReader in = new BufferedReader(new StringReader(content));
		    PrintWriter out = new PrintWriter(new FileWriter(filename, flag));
		    String s;
		    while ((s = in.readLine()) != null){
		    	out.println(s);
		    }
		    out.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			ret = false;
		}
		return ret;
	}
	public static boolean createDir(String pathDir) {
		boolean ret = true;
		String[] splits = pathDir.split("/");
		String path = "";
		for (String dir:splits)
		{
			path = path+dir+"/";
			File file = new File(path);
			if (!file.exists())
			{
				if (!file.mkdir())
				{
					ret = false;
					Log.i("CommonFileOperator", "make dir failed:"+file.getAbsolutePath());
				}
			}
		}
		return ret;
	}
	public static Bitmap compressJPG(String infile, int quality) {
		Bitmap bitmap = null;
		//对图片进行压缩//
    	File inputFile = new File(infile);
    	if (!inputFile.exists())
    		return bitmap;
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inJustDecodeBounds = true;

        //获取这个图片的宽和高   //
        bitmap = BitmapFactory.decodeFile(infile,options);//此时返回bitmap为空//  

        options.inJustDecodeBounds =false;   
        //计算缩放比   //
        int beHeight = (int)(options.outHeight / (float)1024);
        int beWidth = (int)(options.outWidth / (float)1024);
        int be = beHeight > beWidth ? beHeight : beWidth;
        if (be <= 0)
        	be = 1;
        options.inSampleSize = be;
        //重新读入图片，注意这次要把options.inJustDecodeBounds设为false哦    //
        bitmap = BitmapFactory.decodeFile(infile,options);
		return bitmap;
	}
	public static boolean compressJPG(String outfile, String infile, int quality) {
		boolean ret = true;
		try {
			Bitmap bitmap = compressJPG(infile, quality);
	        int w = bitmap.getWidth();
	        int h = bitmap.getHeight();
	        System.out.println(w+" "+h);
	 
	        //保存//
	        File outputFile= new File(outfile);
	        if (outputFile.exists()) {
	        	outputFile.delete();
	        	try {
					outputFile.createNewFile();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					ret = false;
				}
	        }
	        try {   
	        	FileOutputStream out = new FileOutputStream(outputFile);   
	        	if(bitmap.compress(Bitmap.CompressFormat.JPEG, quality, out)) {
	        		out.flush();
	        		out.close();
	        	}   
	        } catch (Exception e) {   
	        	// TODO: handle exception
	        	e.printStackTrace();
	        	ret = false;
	        }
		} catch (NullPointerException e) {
			// TODO: handle exception
        	e.printStackTrace();
        	ret = false;
		}
        return ret;
     }

}
    