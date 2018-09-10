package com.z11.mobile.framework;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;
import android.app.ActivityManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo.State;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Environment;
import android.os.StatFs;
import android.os.SystemClock;
import android.telephony.TelephonyManager;
import android.util.Log;

/**
 * 手机设备信息获取类  
 * @author Hu DanYuan
 * @date 2012-2-3, 16:10
 */

/**
 * 获取失败返回值 String ====== "" long ====== 0
 */

public class DeviceInfoFactory {
	Context mContext = null;
	private long batteryInfo[] = { 0, 0 };

	public DeviceInfoFactory(Context context) {
		// TODO Auto-generated constructor stub
		mContext = context;
		mContext.registerReceiver(batteryReceiver, new IntentFilter(
				Intent.ACTION_BATTERY_CHANGED));
	}

	/**
	 * 获取手机识别码信息 description:
	 * 
	 * @param NONE
	 * @return: Sring[]类型 Sring[0]--MIEI Sring[1]--SimSSN Sring[2]--IMSI
	 */
	public String[] getIdentityNumber() {
		String[] IDNum = { "", "", "" };
		TelephonyManager telephoneManage = (TelephonyManager) mContext
				.getSystemService(Context.TELEPHONY_SERVICE);
		if (telephoneManage != null) {
			String tmpString = telephoneManage.getDeviceId();
			if (tmpString != null) {
				IDNum[0] = tmpString;
			}
			tmpString = telephoneManage.getSimSerialNumber();
			if (tmpString != null) {
				IDNum[1] = tmpString;
			}
			tmpString = telephoneManage.getSubscriberId();
			if (tmpString != null) {
				IDNum[2] = tmpString;
			}
		}
		return IDNum;
	}

	/**
	 * 获取手机识别码信息 description:
	 * 
	 * @param NONE
	 * @return: long[]类型 long[0]--totalLevel long[1]--currentLevel
	 */
	public long[] getBatteryInfo() {
		return batteryInfo;
	}

	private BroadcastReceiver batteryReceiver = new BroadcastReceiver() {
		/*
		 * intent.getIntExtra()中的电池信息status
		 * 电池状态值：BatteryManager.BATTERY_STATUS_XXXhealth
		 * 电池健康值：BatteryManager.BATTERY_HEALTH_XXXpresent 存在状态level 电池剩余容量scale
		 * 电池最大值；通常为100icon-small 图示IDplugged
		 * 链接的电源插座状态：BatteryManager.BATTERY_PLUGGED_XXXvoltage 电压；mVtemperature
		 * 电池表面的温度；0.1度为单位。如201为20.1度technology 电池类型，如Li-ion
		 */

		@Override
		public void onReceive(Context context, Intent intent) {
			batteryInfo[0] = intent.getIntExtra("scale", 0); // 获取总电量
			batteryInfo[1] = intent.getIntExtra("level", 0); // 获取当前电量
		}
	};

	/**
	 * 获取SDCard存储信息 description:
	 * 
	 * @param NONE
	 * @return: long[]类型 long[0]--total long[1]--avail
	 */
	public static long[] getSDCardMemory() {
		long[] SDCardMem = { 0, 0 };
		String state = Environment.getExternalStorageState();
		if (Environment.MEDIA_MOUNTED.equals(state)) {
			File sdcardDir = Environment.getExternalStorageDirectory();
			StatFs sf = new StatFs(sdcardDir.getPath());
			long bSize = sf.getBlockSize();
			long bCount = sf.getBlockCount();
			long availBlocks = sf.getAvailableBlocks();

			SDCardMem[0] = bSize * bCount;
			SDCardMem[1] = bSize * availBlocks;
		}
		return SDCardMem;
	}

	/**
	 * 获取可用内存 description:
	 * 
	 * @param NONE
	 * @return: long类型 单位：B
	 */
	public long getMemoryAvail() {
		long memoryAvail = 0;
		ActivityManager activityManager = (ActivityManager) mContext
				.getSystemService(Context.ACTIVITY_SERVICE);
		ActivityManager.MemoryInfo memoryInfo = new ActivityManager.MemoryInfo();
		activityManager.getMemoryInfo(memoryInfo);
		memoryAvail = memoryInfo.availMem;
		return memoryAvail;
	}

	/**
	 * 获取总的内存 description:
	 * 
	 * @param NONE
	 * @return: long类型 单位：B
	 */
	public static long getTotalMemory() {
		long memoryTotal = 0;
		String[] arrayOfString = null;
		try {
			FileReader fr = new FileReader("/proc/meminfo");
			BufferedReader localBufferedReader = new BufferedReader(fr, 8192);
			String str2 = localBufferedReader.readLine();
			if (str2 != null) {
				arrayOfString = str2.split("\\s+");
			}
			memoryTotal = Long.valueOf(arrayOfString[1]).intValue();

		} catch (IOException e) {
			e.printStackTrace();
		}
		return memoryTotal;
	}

	/**
	 * 获取本地MAC description:
	 * 
	 * @param NONE
	 * @return: String类型 "B4:07:F9:F5:FF:39"
	 */
	public String getMacAddress() {
		String WIFIMacInfo = "";
		WifiManager wifiManager = (WifiManager) mContext
				.getSystemService(Context.WIFI_SERVICE);
		if (wifiManager != null) {
			WifiInfo wifiInfo = wifiManager.getConnectionInfo();
			if (wifiInfo != null) {
				if (wifiInfo.getMacAddress() != null) {
					WIFIMacInfo = wifiInfo.getMacAddress();
				}
			}
		}
		return WIFIMacInfo;
	}

	/**
	 * 获取本地IP description:
	 * 
	 * @param NONE
	 * @return: String类型 "192.168.1.1"
	 */
	public static String getHostAddress() {
		String localIpAddress = null;
		try {
			for (Enumeration<NetworkInterface> en = NetworkInterface
					.getNetworkInterfaces(); en.hasMoreElements();) {
				NetworkInterface intf = en.nextElement();
				for (Enumeration<InetAddress> enumIpAddr = intf
						.getInetAddresses(); enumIpAddr.hasMoreElements();) {
					InetAddress inetAddress = enumIpAddr.nextElement();
					if (!inetAddress.isLoopbackAddress()) {
						if (inetAddress.getHostAddress() != null)
							localIpAddress = inetAddress.getHostAddress();
					}
				}
			}
		} catch (SocketException ex) {
			Log.e("WifiPreference IpAddress", ex.toString());
		}
		return localIpAddress;
	}

	/**
	 * 获取网络连接状态 description:
	 * 
	 * @param NONE
	 * @return: String[]类型 String[0]--mobile String[1]--wifi UNKNOWN--未插卡
	 *          CONNECTED--连接 DISCONNECTED--未连接
	 */
	public String[] getNetworkInfo() {
		String[] network = { "", "" };
		ConnectivityManager conMan = (ConnectivityManager) mContext
				.getSystemService(Context.CONNECTIVITY_SERVICE);
		if (conMan != null) {
			State mobile = conMan.getNetworkInfo(
					ConnectivityManager.TYPE_MOBILE).getState();
			network[0] = mobile.toString();
			State tempWifi = conMan.getNetworkInfo(
					ConnectivityManager.TYPE_WIFI).getState();
			network[1] = tempWifi.toString();
		}
		return network;
	}

	/**
	 * 获取开机运行时间 description:
	 * 
	 * @param NONE
	 * @return: long类型 单位：ms
	 */
	public static long getElapsedRealTime() {
		return SystemClock.elapsedRealtime();
	}

	//
	// private void genGPSInfo() {
	// GPS = Settings.System.getString(getContentResolver(),
	// Settings.System.LOCATION_PROVIDERS_ALLOWED);
	// // String[] arratyString = null;
	// // String gps = Settings.System.getString(getContentResolver(),
	// Settings.System.LOCATION_PROVIDERS_ALLOWED);
	// // if (!gps.equals(""))
	// // arratyString = gps.split(",");
	// // if (arratyString.length > 0)
	// // GPS[0] = arratyString[0];
	// // if (arratyString.length > 1)
	// // GPS[1] = arratyString[1];
	// //LocationManager location_manager =
	// (LocationManager)getSystemService(Context.LOCATION_SERVICE);
	// //Log.i("GPS", "" +
	// location_manager.isProviderEnabled(LocationManager.GPS_PROVIDER));
	// }
	// private void genVersion(){
	// String str1 = "/proc/version";
	// String str2;
	// String[] arrayOfString;
	// try {
	// FileReader localFileReader = new FileReader(str1);
	// BufferedReader localBufferedReader = new BufferedReader(
	// localFileReader, 8192);
	// str2 = localBufferedReader.readLine();
	// arrayOfString = str2.split("\\s+");
	// version[0]=arrayOfString[2];//KernelVersion
	// localBufferedReader.close();
	// }
	// catch (IOException e) {
	// }
	// version[1] = Build.VERSION.RELEASE;// firmware version
	// version[2] = Build.MODEL;//model
	// version[3] = Build.DISPLAY;//system version
	// }
	// public String formatSize(long size) {
	// String suffix = null;
	// float fSize=0;
	//
	// if (size >= 1024) {
	// suffix = "KB";
	// fSize=size / 1024;
	// if (fSize >= 1024) {
	// suffix = "MB";
	// fSize /= 1024;
	// }
	// if (fSize >= 1024) {
	// suffix = "GB";
	// fSize /= 1024;
	// }
	// } else {
	// fSize = size;
	// }
	// java.text.DecimalFormat df = new java.text.DecimalFormat("#0.00");
	// StringBuilder resultBuffer = new StringBuilder(df.format(fSize));
	// if (suffix != null)
	// resultBuffer.append(suffix);
	// return resultBuffer.toString();
	// }
	//
	// private void genCpuInfo() {
	// ProcessBuilder cmd;
	// try {
	// String[] args = { "/system/bin/cat", "/proc/cpuinfo" };
	// cmd = new ProcessBuilder(args);
	// Process process = cmd.start();
	// InputStream in = process.getInputStream();
	// byte[] re = new byte[1024];
	// while (in.read(re) != -1) {
	// //System.out.println(new String(re));
	// CPUInfo = CPUInfo + new String(re);
	// //CPUInfo.toString();
	// }
	// in.close();
	// }
	// catch (IOException ex) {
	// ex.printStackTrace();
	// }
	// }
	//
	// private void genCpuStat() {
	// ProcessBuilder cmd;
	// int cpuNum = 0;
	// try {
	// String[] args = { "/system/bin/cat", "/proc/stat" };
	// cmd = new ProcessBuilder(args);
	// Process process = cmd.start();
	// InputStream in = process.getInputStream();
	// InputStreamReader isr=new InputStreamReader(in);
	// BufferedReader bfr=new BufferedReader(isr,8192);
	// //StringBuffer sbf=new StringBuffer();
	//
	// //byte[] re = new byte[1024];
	// //while (in.read(re) != -1) {
	// String str = "";
	// while ((str=bfr.readLine()) != null) {
	// String[] arrayOfString = str.split("\\s+");
	// if (arrayOfString[0].substring(0, 3).equals("cpu")) {
	// Double total = Double.parseDouble(arrayOfString[1]) +
	// Double.parseDouble(arrayOfString[2]) +
	// Double.parseDouble(arrayOfString[3]) +
	// Double.parseDouble(arrayOfString[4]);
	// Double usage = Double.parseDouble(arrayOfString[1]) +
	// Double.parseDouble(arrayOfString[2]) +
	// Double.parseDouble(arrayOfString[3]);
	// CPURate[cpuNum] = usage / total;
	// java.text.DecimalFormat df = new java.text.DecimalFormat("#0.0000");
	// StringBuilder resultBuffer = new
	// StringBuilder(df.format(CPURate[cpuNum]));
	// CPURate[cpuNum] = Double.parseDouble(resultBuffer.toString());
	// cpuNum++;
	// }
	// else
	// break;
	// //CPUStat = CPUStat + new String(re);
	// CPUStat = CPUStat + new String(str);
	// }
	// bfr.close();
	// isr.close();
	// in.close();
	// }
	// catch (IOException ex) {
	// ex.printStackTrace();
	// }
	// }
	//
	// private void getRunningProcessInfo(Context context) {
	// StringBuffer serviceInfo = new StringBuffer();
	// final ActivityManager activityManager = (ActivityManager) context
	// .getSystemService(Context.ACTIVITY_SERVICE);
	// List<ActivityManager.RunningAppProcessInfo> processes =
	// activityManager.getRunningAppProcesses();
	//
	// Iterator<RunningAppProcessInfo> l = processes.iterator();
	// while (l.hasNext()) {
	// RunningAppProcessInfo si = (RunningAppProcessInfo) l.next();
	//
	// //serviceInfo.append(si.pid).append(",");
	// serviceInfo.append(si.processName).append(" ");
	// }
	// runningProcess = serviceInfo.toString();
	// runningProcess = runningProcess.substring(0, runningProcess.length()-1);
	// String[] arraryString = runningProcess.split("\\s");
	// runningProcessNum = arraryString.length;
	// }
}
