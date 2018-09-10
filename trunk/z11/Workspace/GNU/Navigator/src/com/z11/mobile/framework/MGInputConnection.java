/**
 *调用输入法相关模块 *
 */
package com.z11.mobile.framework;

import android.util.Log;
import android.view.View;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.CompletionInfo;

public class MGInputConnection extends BaseInputConnection{
		private String inputString="";
	    public MGInputConnection(View targetView, boolean fullEditor) { 
	        super(targetView, fullEditor); 
	    }
	    @Override
	    public boolean commitCompletion (CompletionInfo text) {
	    	inputString=inputString+text.toString(); 
	        Log.d("MGInputConnection in commitCompletion", inputString);
			return true;
		}
	    @Override
	    public boolean commitText(CharSequence text, int newCursorPosition){ 
	        inputString=inputString+text.toString(); 
	        Log.d("MGInputConnection in commitText", inputString);
	        MGMessageCommunication.JavaToNativeCommandAndData(6,inputString);
	        inputString="";//清空
	        return true; 
	    }
	    
	    private static native void CommitText(String text);
	}