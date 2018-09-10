package com.z11.mobile.framework;

import java.io.IOException;
import java.net.InetAddress;
import java.net.UnknownHostException;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiManager.WifiLock;

import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.util.Log;
import java.util.List;

public class MGNetworkReachability {

	private static WifiManager wifi = (WifiManager) MGActivity.activity_instance
			.getSystemService(Context.WIFI_SERVICE);

	private static native void nativeWifiScanResults(String scanResult);

	private static BroadcastReceiver receiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context arg0, Intent arg1) {
			// TODO Auto-generated method stub
			String ret = "";
			List<ScanResult> results = wifi.getScanResults();
			for (int i = 0; i < results.size(); i++) {
				ret += results.get(i).BSSID + "," + results.get(i).SSID + ","
						+ results.get(i).capabilities + ","
						+ String.valueOf(results.get(i).frequency) + ","
						+ String.valueOf(results.get(i).level);
				ret += ";";
			}
			nativeWifiScanResults(ret);
		}
	};

	public static int isWifiNetworkEnabled() {
		//Log.i("z11", "isWifiNetworkEnabled");
		int ret = 0;
		ConnectivityManager conMan = (ConnectivityManager) MGActivity.activity_instance
				.getSystemService(Context.CONNECTIVITY_SERVICE);
		// wifi
		NetworkInfo wifiNetInfo = conMan
				.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
		if (wifiNetInfo != null && wifiNetInfo.isConnected())
			ret = 1;
		return ret;
	}

	public static int isCarrierNetworkEnabled() {
		//Log.i("z11", "isCarrierNetworkEnabled");
		int ret = 0;
		ConnectivityManager conMan = (ConnectivityManager) MGActivity.activity_instance
				.getSystemService(Context.CONNECTIVITY_SERVICE);
		// mobile 3G Data Network
		NetworkInfo mobileNetInfo = conMan
				.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);
		if (mobileNetInfo != null && mobileNetInfo.isConnected())
			ret = 1;
		return ret;
	}

	public static int reachabilityWithHostName(String host) {
		int ret = 0;
		try {
			InetAddress address = InetAddress.getByName(host);
			if (address != null) {
				ret = reachabilityWithAddress(address.getHostAddress());
			}
		} catch (UnknownHostException e) {
			e.printStackTrace();
		}
		return ret;
	}

	public static int reachabilityWithAddress(String ipAddress) {
		int ret = 0;
		int status = -1;
		try {
			Process process = Runtime.getRuntime().exec(
					"ping -c 1 -w 4 " + ipAddress);
			try {
				if (process != null)
					status = process.waitFor();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		if (status == 0) {
			ConnectivityManager conMan = (ConnectivityManager) MGActivity.activity_instance
					.getSystemService(Context.CONNECTIVITY_SERVICE);
			NetworkInfo wifiNetInfo = conMan
					.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
			NetworkInfo mobileNetInfo = conMan
					.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);
			if (wifiNetInfo != null && wifiNetInfo.isConnected())
				ret = 1;
			else if (mobileNetInfo != null && mobileNetInfo.isConnected())
				ret = 2;
		}
		return ret;
	}

	public static void refreshWifiStatus() {
		Log.i("z11", "refreshWifiStatus");
		try {
			MGActivity.activity_instance
					.registerReceiver(receiver, new IntentFilter(
							WifiManager.SCAN_RESULTS_AVAILABLE_ACTION));
			WifiManager wifi = (WifiManager) MGActivity.activity_instance
					.getSystemService(Context.WIFI_SERVICE);
			wifi.startScan();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static int ipToInt(String addr) {
		String[] addrArray = addr.split("\\.");
		int num = 0;
		for (int i = 0; i < addrArray.length; i++) {
			int power = 3 - i;
			num += ((Integer.parseInt(addrArray[i]) % 256 * Math
					.pow(256, power)));
		}
		return num;
	}
}
