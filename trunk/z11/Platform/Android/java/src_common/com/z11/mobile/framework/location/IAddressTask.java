package com.z11.mobile.framework.location;

import java.io.BufferedReader;
import java.io.InputStreamReader;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.json.JSONArray;
import org.json.JSONObject;

import android.app.Activity;
import android.content.Context;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.telephony.TelephonyManager;
import android.telephony.cdma.CdmaCellLocation;
import android.telephony.gsm.GsmCellLocation;

public abstract class IAddressTask {

    protected Activity context;
    
    public IAddressTask(Activity context) {
        this.context = context;
    }
    
    public abstract HttpResponse execute(JSONObject params) throws Exception;
    
    public MLocation doWifiPost() throws Exception {
        return transResponse(execute(doWifi()));
    }
    
    public MLocation doMobilePost() throws Exception  {
        return transResponse(execute(doMobile()));
    }
    
    public MLocation doGpsPost(double lat, double lng) throws Exception {
        return transResponse(execute(doGps(lat, lng)));
    }

    /** {
	  * 	"location":
	  * 	{
	  * 		"latitude":30.2738975,
	  * 		"longitude":120.1332207,
	  * 		"address":
	  * 		{
	  * 			"country":"中国",
	  * 			"country_code":"CN",
	  * 			"region":"浙江省",
	  * 			"city":"杭州市",
	  * 			"street":"黄姑山路"
	  * 		},
	  * 		"accuracy":859.0
	  * 	},
	  * 	"access_token":"2:IkSHg1WofUTDn4-D:2Uvoi0C2HLkKGYRz"
	  * }
	  **/
    private MLocation transResponse(HttpResponse response) {
        MLocation location = null;
        try {
        	if (response.getStatusLine().getStatusCode() == HttpStatus.SC_OK) {
                location = new MLocation();
                HttpEntity entity = response.getEntity();
                BufferedReader br;
                try {
                    br = new BufferedReader(new InputStreamReader(
                            entity.getContent()));
                    StringBuffer sb = new StringBuffer();
                    String result = br.readLine();
                    while (result != null) {
                        sb.append(result);
                        result = br.readLine();
                    }
                    JSONObject json = new JSONObject(sb.toString());
                    JSONObject lca = json.getJSONObject("location");

                    location.Access_token = json.getString("access_token");
                    if (lca != null) {
                        if(lca.has("accuracy"))
                            location.Accuracy = lca.getDouble("accuracy");
                        if(lca.has("longitude"))
                            location.Longitude = lca.getDouble("longitude");
                        if(lca.has("latitude"))
                            location.Latitude = lca.getDouble("latitude");
                        if(lca.has("address")) {
                            JSONObject address = lca.getJSONObject("address");
                            if (address != null) {
                                if(address.has("region"))
                                    location.Region = address.getString("region");
                                if(address.has("street_number"))
                                    location.Street_number = address.getString("street_number");
                                if(address.has("country_code"))
                                    location.Country_code = address.getString("country_code");
                                if(address.has("street"))
                                    location.Street = address.getString("street");
                                if(address.has("city"))
                                    location.City = address.getString("city");
                                if(address.has("country"))
                                    location.Country = address.getString("country");
                            }
                        }
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                    location = null;
                }
            }
		} catch (NullPointerException e) {
			// TODO: handle exception
			e.printStackTrace();
			location = null;
		}
        return location;
    }

    private JSONObject doGps(double lat, double lng) throws Exception {
        JSONObject holder = new JSONObject();
        holder.put("version", "1.1.0");
        holder.put("host", "maps.google.com");
        holder.put("address_language", "zh_CN");
        holder.put("request_address", true);
        
        JSONObject data = new JSONObject();
        data.put("latitude", lat);
        data.put("longitude", lng);
        holder.put("location", data);

        return holder;
    }
    
    /** MCC，Mobile Country Code，移动国家代码（中国的为460）；
	  * MNC，Mobile Network Code，移动网络号码（中国移动为00，中国联通为01）；
	  * LAC，Location Area Code，位置区域码；
	  * CID，Cell Identity，基站编号，是个16位的数据（范围是0到65535）
	  **/
    private JSONObject doMobile() throws Exception {
    	//获得GSM相关信息
        TelephonyManager tm = (TelephonyManager) context
                .getSystemService(Context.TELEPHONY_SERVICE);
        int cid = 0;
        int lac = 0;
        int mcc = 0;
        int mnc = 0;
        int phoneType = tm.getPhoneType();
        JSONObject holder = new JSONObject();
        if (phoneType == TelephonyManager.PHONE_TYPE_GSM
        		|| phoneType == TelephonyManager.NETWORK_TYPE_GPRS              // GSM网  
                || phoneType == TelephonyManager.NETWORK_TYPE_EDGE  
                || phoneType == TelephonyManager.NETWORK_TYPE_HSDPA) {
        	holder.put("radio_type", "gsm");
        	GsmCellLocation gcl = (GsmCellLocation) tm.getCellLocation();
        	if (gcl == null)
        		return null;
        	cid = gcl.getCid();
            lac = gcl.getLac();
            mcc = Integer.valueOf(tm.getNetworkOperator().substring(0,3));
            mnc = Integer.valueOf(tm.getNetworkOperator().substring(3,5));
        } else if (phoneType == TelephonyManager.PHONE_TYPE_CDMA
        			|| phoneType == TelephonyManager.NETWORK_TYPE_1xRTT  
        			|| phoneType == TelephonyManager.NETWORK_TYPE_EVDO_0  
        			|| phoneType == TelephonyManager.NETWORK_TYPE_EVDO_A
        			/*|| phoneType == TelephonyManager.NETWORK_TYPE_EVDO_B*/) {
        	holder.put("radio_type", "cdma");
            CdmaCellLocation ccl = (CdmaCellLocation) tm.getCellLocation();
            if (ccl == null)
            	return null;
            cid = ccl.getBaseStationId();
            lac = ccl.getNetworkId();
            mcc = Integer.valueOf(tm.getNetworkOperator().substring(0,3));
            mnc = Integer.valueOf(tm.getNetworkOperator().substring(3,5));
        } else {
        	return null;
        }
        
        holder.put("version", "1.1.0");
        holder.put("host", "maps.google.com");
//        if (mcc == 460)
//			holder.put("address_language", "zh_CN"); 
//		else
//			holder.put("address_language", "en_US");
//        holder.put("request_address", true);
        
        //请求参数
        JSONArray array = new JSONArray();
        JSONObject data = new JSONObject();
        data.put("cell_id", cid);
        data.put("location_area_code", lac);
        data.put("mobile_country_code", mcc);
        data.put("mobile_network_code", mnc);
        array.put(data);
        holder.put("cell_towers", array);
        
        return holder;
    }
    
    private JSONObject doWifi() throws Exception {
    	//获得wifi相关信息
        WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        WifiInfo wifiInfo = wifiManager.getConnectionInfo();
        if (wifiInfo == null)
        	return null;
        if(wifiInfo.getBSSID() == null) {
            throw new Exception("mac address is null");
        }
//        int strength = WifiManager.calculateSignalLevel(wifiInfo.getRssi(),5);
        
        JSONObject holder = new JSONObject();
        holder.put("version", "1.1.0");
        holder.put("host", "maps.google.com");
//        holder.put("address_language", "zh_CN");
//        holder.put("request_address", true);
        
        JSONArray array = new JSONArray();
        JSONObject data = new JSONObject();
        data.put("mac_address", wifiInfo.getBSSID());
        data.put("signal_strength", wifiInfo.getRssi());  
        data.put("age", 0);
        array.put(data);
        holder.put("wifi_towers", array);
        return holder;
    }

    public static class MLocation {
    	public long time;
        public String Access_token;
        public double Latitude;
        public double Longitude;
        public double Accuracy;
        public String Region;
        public String Street_number;
        public String Country_code;
        public String Street;
        public String City;
        public String Country;

        public String toDesc() {
            StringBuffer buffer = new StringBuffer();
            buffer.append("Access_token:" + Access_token + "\n");
            buffer.append("Region:" + Region + "\n");
            buffer.append("Accuracy:" + Accuracy + "\n");
            buffer.append("Latitude:" + Latitude + "\n");
            buffer.append("Longitude:" + Longitude + "\n");
            buffer.append("Country_code:" + Country_code + "\n");
            buffer.append("Country:" + Country + "\n");
            buffer.append("City:" + City + "\n");
            buffer.append("Street:" + Street + "\n");
            buffer.append("Street_number:" + Street_number + "\n");
            return buffer.toString();
        }
        @Override
		public String toString() {
			StringBuffer buffer = new StringBuffer();
			buffer.append(Longitude + ",");
			buffer.append(Latitude + ",");
			buffer.append("0.0" + ",");
			buffer.append("0.0" + ",");
			buffer.append("0.0" + ",");
			buffer.append(Accuracy);
			return buffer.toString();
		}
    }
}