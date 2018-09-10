package com.z11.mobile.framework;

import java.util.HashMap;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;

import android.media.MediaPlayer;
import android.util.Log;

public class MGTimer {
	//private static Timer mTimer;
	//private static  TimerTask    mTimerTask ;
	static Map<Integer , Timer> timer_map=new HashMap<Integer,Timer>();
	static Map<Integer , TimerTask> timer_task_map=new HashMap<Integer,TimerTask>();
	public static  void startTimer(final int id,int delay,int period)
	{
		Log.i("startTimer", "id="+id+",delay="+delay+",period="+period);
		if(timer_map.containsKey(id))
			return ;
        Timer    mTimer = new Timer();  
        TimerTask  mTimerTask = new TimerTask() {  
                @Override  
                public void run() {  
            		MGActivity.activity_instance.onSendMessage(MGActivity.HANDLE_TIMER_EVENT,id);
                }  
            };  
  
        if(mTimer != null && mTimerTask != null )  
        {
        	if(period!=0)
                mTimer.schedule(mTimerTask, delay,period); 
        	else
                mTimer.schedule(mTimerTask, delay); 
        		
        }
        timer_map.put(id, mTimer);
        timer_task_map.put(id, mTimerTask);
	}
	public static void stopTimer(int id)
	{  
		Log.i("stopTimer","id="+id);
		if(!timer_map.containsKey(id))
			return ;
		Timer mTimer=(Timer)timer_map.get(id);
		
	        if (mTimer != null) {  
	            mTimer.cancel();  
	            mTimer = null;  
		        timer_map.remove(id);
	        }  
	  TimerTask mTimerTask=timer_task_map.get(id);
	        if (mTimerTask != null) {  
	            mTimerTask.cancel();  
	            mTimerTask = null;  
	            timer_task_map.remove(id);
	        }     
	    }  
	public static native void nativeHandleEvent(int i);
}
