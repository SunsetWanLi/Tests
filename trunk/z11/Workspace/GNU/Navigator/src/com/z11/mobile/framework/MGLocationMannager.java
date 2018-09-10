package com.z11.mobile.framework;

import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.util.Log;

public class MGLocationMannager {
	private LocationManager locationManager;
	private String provider;
	public void onCreate(LocationManager _location_manager)
	{
        //DemoGLSurfaceView.ReloadMap(120.666,28.016,0.0001);
		
      locationManager = _location_manager;
      
      Criteria criteria = new Criteria();
      criteria.setAccuracy(Criteria.ACCURACY_FINE);
      criteria.setAltitudeRequired(false);
      criteria.setBearingRequired(false);
      criteria.setCostAllowed(true);
      criteria.setPowerRequirement(Criteria.POWER_LOW);
      provider = locationManager.getBestProvider(criteria, true);
      
      if(provider!=null)
      {
      	//Location location = locationManager.getLastKnownLocation(provider);
      	//updateWithNewLocation(location);
      	locationManager.requestLocationUpdates(provider, 5000, 10, locationListener);
      }
      else
      	Log.i("tag","provider==null");
	}
	public void onPause(){

//      locationManager.removeUpdates(locationListener);   
		
	}
	public  void onResume() {

//      locationManager.requestLocationUpdates(provider, 15000, 1, locationListener);
	}
	
	private final LocationListener locationListener = new LocationListener() {
    	public void onLocationChanged(Location location) {
    		updateWithNewLocation(location);
		}
		public void onProviderDisabled(String provider){
			updateWithNewLocation(null);
		}
		public void onProviderEnabled(String provider){ }
		public void onStatusChanged(String provider, int status, Bundle extras){ }
    };
    
    private void updateWithNewLocation(Location location) {
    	String latLongString;
		
		if (location != null) {
			double lat = location.getLatitude();
			double lng = location.getLongitude();
			latLongString = "latitude:" + lat + "\nlongitude:" + lng;
			MGEventHandler.ReloadMap(lng,lat,0.0001);
		} 
		else {
			latLongString = "Unable to get geographic information";	//"无法获取地理信息";
		}
		Log.i("tag",latLongString);
//		myLocationText.setText("您当前的位置是:\n" + latLongString);
    }

}
