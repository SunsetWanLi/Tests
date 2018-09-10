package com.z11.mobile.framework;

import android.content.Context;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.KeyEvent;
import android.view.inputmethod.InputMethodManager;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;

class TextInputWraper implements TextWatcher, OnEditorActionListener {

	private static final Boolean debug = false;

	private void LogD(String msg) {
		if (debug)
			Log.d("TextInputFilter", msg);
	}

	private MGGLSurfaceView mMainView;
	//private String mText;
	//private String mOriginText;

	private Boolean isFullScreenEdit() {
		InputMethodManager imm = (InputMethodManager) mMainView.getTextField()
				.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
		return imm.isFullscreenMode();
	}

	public TextInputWraper(MGGLSurfaceView view) {
		mMainView = view;
	}

	public void setOriginText(String text) {
		//mOriginText = text;
	}

	@Override
	public void afterTextChanged(Editable s) {
		//Log.e("z11::InputMethod","afterTextChanged()");
		if (isFullScreenEdit()) {
			return;
		}

//		Log.e("z11::InputMethod","afterTextChanged:"+s);
//		//LogD("afterTextChanged: " + s);
//		int nModified = s.length() - mText.length();
//		if (nModified > 0) {
//			final String insertText = s.subSequence(mText.length(), s.length())
//					.toString();
//			mMainView.insertText(insertText);
//			Log.e("z11::InputMethod","insertText(" + insertText + ")");
//			//LogD("insertText(" + insertText + ")");
//		} else if(nModified < 0){
//			for (; nModified < 0; ++nModified) {
//				mMainView.deleteBackward();
//				Log.e("z11::InputMethod","deleteBackward");
//				//LogD("deleteBackward");
//			}
//		} else {
//			mMainView.deleteBackward();
//			Log.e("z11::InputMethod","deleteBackward");
//			final String insertText = s.subSequence(s.length() - 1, s.length())
//					.toString();
//			mMainView.insertText(insertText);
//			Log.e("z11::InputMethod","insertText(" + insertText + ")");
//		}
//		mText = s.toString();
		//s.clear();
	}

	@Override
	public void beforeTextChanged(CharSequence s, int start, int count,
			int after) {
		//Log.e("z11::InputMethod","beforeTextChanged()");
		//Log.e("z11::InputMethod", "beforeTextChanged(" + s + ")start: " + start + ",count: " + count
				//+ ",after: " + after);
		
		for (int i = 0; i < count; ++i) {
			mMainView.deleteBackward();
			//Log.e("z11::InputMethod","deleteBackward");
			//LogD("deleteBackward");
		}
		
	}

	@Override
	public void onTextChanged(CharSequence s, int start, int before, int count) {
		//Log.e("z11::InputMethod","onTextChanged()");
		//Log.e("z11::InputMethod", "onTextChanged(" + s + ")start: " + start + ",before: " + before
				//+ ",count: " + count);
		final String insertText = s.subSequence(start, start+count)
				.toString();
		mMainView.insertText(insertText);
		//Log.e("z11::InputMethod","insertText(" + insertText + ")");
	}

	@Override
	public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
		//Log.e("z11::EditorAction","onEditorAction()");
		mMainView.insertText("\n");
		return false;
	}
}