package com.z11.mobile.framework;

import java.io.File;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

public class MGIntent {

	public static void openFile(String path) {
		Intent intent = new Intent();
		if (path.indexOf("http") >= 0) {
			intent.setAction("android.intent.action.VIEW");
			Uri content_url = Uri.parse(path);
			intent.setData(content_url);
		} else {

			intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
			intent.setAction(android.content.Intent.ACTION_VIEW);

			String type = getType(path);
			File file = new File(path);
			if (file.exists()) {
				Uri uri = Uri.fromFile(file);
				intent.setDataAndType(uri, type);
			} else {
				Log.e("openFile: file not exist", path);
			}
		}

		try {
			MGActivity.activity_instance.startActivity(intent);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void openRemoteApp(String identity, String url) {
		Intent intent = null;
		if (url.indexOf("http") >= 0) {
			intent = new Intent();
			intent.setAction("android.intent.action.VIEW");
			Uri content_url = Uri.parse(url);
			intent.setData(content_url);
		} else {
			intent = new Intent("android.intent.action.VIEW", Uri.parse(url));
		}

		try {
			MGActivity.activity_instance.startActivity(intent);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private static String getType(String path) {
		String type = "";
		/* 取得扩展名 */
		String end = path.substring(path.lastIndexOf(".") + 1, path.length())
				.toLowerCase();

		/* 依扩展名的类型决定MimeType */
		if (end.equals("m4a") || end.equals("mp3") || end.equals("mid")
				|| end.equals("xmf") || end.equals("ogg") || end.equals("wav")
				|| end.equals("flac") || end.equals("ape")) {
			type = "audio/*";
		} else if (end.equals("3gp") || end.equals("mp4") || end.equals("rmvb")
				|| end.equals("avi") || end.equals("flv") || end.equals("mkv")
				|| end.equals("rm")) {
			type = "video/*";
		} else if (end.equals("jpg") || end.equals("gif") || end.equals("png")
				|| end.equals("jpeg") || end.equals("bmp")) {
			type = "image/*";
		} else if (end.equals("html") || end.equals("htm") || end.equals("php")
				|| end.equals("jsp")) {
			type = "text/html";
		} else if (end.equals("txt") || end.equals("java") || end.equals("c")
				|| end.equals("cpp") || end.equals("py") || end.equals("xml")
				|| end.equals("json") || end.equals("log")) {
			type = "text/plain";
		} else if (end.equals("chm")) {
			type = "application/x-chm";
		} else if (end.equals("pdf")) {
			type = "application/pdf";
		} else if (end.equals("doc") || end.equals("docx")) {
			type = "application/msword";
		} else if (end.equals("xls") || end.equals("xlsx")) {
			type = "application/vnd.ms-excel";
		} else if (end.equals("ppt") || end.equals("pptx")) {
			type = "application/vnd.ms-powerpoint";
		} else if (end.equals("jar") || end.equals("zip") || end.equals("rar")
				|| end.equals("gz") || end.equals("apk") || end.equals("img")
				|| end.equals("7z")) {
			type = "application/vnd.android.package-archive";
		} else {
			type = "*/*";
		}
		return type;
	}
}