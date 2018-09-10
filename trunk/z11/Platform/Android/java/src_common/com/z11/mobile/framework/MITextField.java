package com.z11.mobile.framework;

import android.content.Context;
import android.text.Editable;
import android.view.KeyEvent;
import android.widget.EditText;
import android.widget.TextView;

class MITextField extends EditText {
	private MGGLSurfaceView mainView;
	private MGRenderer mRenderer;
	
	public MITextField(Context context) {
		super(context);
		// TODO Auto-generated constructor stub
	}
	
	public void setMainView(MGGLSurfaceView view) {
		mainView = view;
	}
	public void setRenderer(MGRenderer renderer) {
		mRenderer = renderer;
	}
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_DEL) {
			this.removeTextChangedListener(MGGLSurfaceView.textInputWraper);
			super.onKeyDown(keyCode, event);
			this.addTextChangedListener(MGGLSurfaceView.textInputWraper);
		}
//		if (keyCode == KeyEvent.KEYCODE_BACK) {
//			mainView.requestFocus();
//		}
		if(keyCode == KeyEvent.KEYCODE_VOLUME_UP||keyCode==KeyEvent.KEYCODE_VOLUME_DOWN){
			return false;
		}
		int asciiKey = event.getUnicodeChar(event.getMetaState());
		if ((asciiKey < 32 || asciiKey > 126) && asciiKey != 0x0A && asciiKey != 0x0D)
			asciiKey = keyCode;
		return mRenderer.handleKeyDown(asciiKey);
	}
}
