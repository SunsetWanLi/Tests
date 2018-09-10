package com.z11.mobile.framework;

import android.util.Log;

public class MGMessageCommunication {  
	/* NativeToJavaCommandAndData 如果不需要回调java则可以在任意地点调用，否则可能会出错  
     * code = 0 send handle event
     * code = 1 Reload map
     * code = 2 ConnectVideoServer
     * code = 3 SetTrafficInfo
     * code = 4 SetWeatherInfo
     * code = 5 ReloadCity
     * code = 6 ToggleSoftInput
     * code = 7 HideSoftInput
     * */
    public static void NativeToJavaCommandAndData(int code,String data)
    {
    	Log.i("NativeToJavaCommandAndData","code="+code+"data:="+data);
    	MGEventHandler.handle(code, data);
    }
    /* JavaToNativeCommandAndData 如果不需要回调java则可以在任意地点调用，否则可能会出错  
     * code = 0 default ,handle event
     * code = 1 Reload map
     * code = 2 Add VideoButton
     * code = 3 clear all video button
     * code = 4 Reload Traffic
     * code = 5 Reload Weather
     * code = 6 Input Commit text
     * code = 7 KeyDown 
     * */
    public static native void JavaToNativeCommandAndData(int code ,String data);
    public static native int GetNativeCommand();
}
