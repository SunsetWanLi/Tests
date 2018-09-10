package com.z11.mobile.framework;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Handler;
import android.os.Message;
import android.text.InputType;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;
import android.hardware.Camera;


public class MGGLSurfaceView extends GLSurfaceView {

	static MGGLSurfaceView mainView;
	static boolean isSoftInputShow = false; 

	// private static final String TAG =
	// MGGLSurfaceView.class.getCanonicalName();
	private MGRenderer mRenderer;

	// private static final boolean debug = false;

	// /////////////////////////////////////////////////////////////////////////
	// for initialize
	// /////////////////////////////////////////////////////////////////////////
	public MGGLSurfaceView(Context context) {
		super(context);
		initView();

		_this = this;
		imm = (InputMethodManager) context
				.getSystemService(Context.INPUT_METHOD_SERVICE);
		this.setFocusable(true);
		this.setFocusableInTouchMode(true);
	}

	public MGGLSurfaceView(Context context, AttributeSet attrs) {
		super(context, attrs);
		initView();

		_this = this;
		imm = (InputMethodManager) context
				.getSystemService(Context.INPUT_METHOD_SERVICE);
		this.setFocusable(true);
		this.setFocusableInTouchMode(true);
	}

	protected void initView() {
		mRenderer = new MGRenderer();
		mRenderer.gl_view = this;
		setFocusableInTouchMode(true);
		setRenderer(mRenderer);
		mainView = this;
		mainView.setFocusable(true);
		mainView.setFocusableInTouchMode(true);
		mainView.requestFocus();

		textInputWraper = new TextInputWraper(this);

		handler = new Handler() {
			public void handleMessage(Message msg) {
				switch (msg.what) {
				case HANDLER_OPEN_IME_KEYBOARD:
					if (null != mTextField && mTextField.requestFocus()) {
						String tt=(String)msg.obj;
						String[] typeStrings = tt.split("&");
						String keyboard_type = "";
						String return_key_type = "";
						if(typeStrings.length>0){
							keyboard_type = typeStrings[0];
						}
						if(typeStrings.length>1){
							return_key_type = typeStrings[1];
						}
						
						if (mTextField != null) {
							if (keyboard_type.equals("textPhonetic")) {
								mTextField.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PHONETIC);
							} else if (keyboard_type.equals("text")) {
								mTextField.setInputType(InputType.TYPE_CLASS_TEXT);
							} else if (keyboard_type.equals("numberSigned")) {
								mTextField.setInputType(InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_SIGNED);
							} else if (keyboard_type.equals("textUri")) {
								mTextField.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_URI);
							} else if (keyboard_type.equals("number")) {
								mTextField.setInputType(InputType.TYPE_CLASS_NUMBER);
							} else if (keyboard_type.equals("phone")) {
								mTextField.setInputType(InputType.TYPE_CLASS_PHONE);
							} else if (keyboard_type.equals("textPersonName")) {
								mTextField.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PERSON_NAME);
							} else if (keyboard_type.equals("textEmailAddress")) {
								mTextField.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_EMAIL_ADDRESS);
							} else if (keyboard_type.equals("numberDecimal")) {
								mTextField.setInputType(InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_FLAG_DECIMAL);
							} else if (keyboard_type.equals("textShortMessage")) {
								mTextField.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_SHORT_MESSAGE);
							}
							
							if (return_key_type.equals("default")) {
								
							}else if(return_key_type.equals("go")){
								mTextField.setImeOptions(EditorInfo.IME_ACTION_GO);
							}else if(return_key_type.equals("google")){
								mTextField.setImeOptions(EditorInfo.IME_ACTION_SEARCH);
							}else if(return_key_type.equals("join")){
								mTextField.setImeOptions(EditorInfo.IME_ACTION_UNSPECIFIED);
							}else if(return_key_type.equals("next")){
								mTextField.setImeOptions(EditorInfo.IME_ACTION_NEXT);
							}else if(return_key_type.equals("route")){
								mTextField.setImeOptions(EditorInfo.IME_ACTION_UNSPECIFIED);
							}else if(return_key_type.equals("search")){
								mTextField.setImeOptions(EditorInfo.IME_ACTION_SEARCH);
							}else if(return_key_type.equals("send")){
								mTextField.setImeOptions(EditorInfo.IME_ACTION_SEND);
							}else if(return_key_type.equals("yahoo")){
								mTextField.setImeOptions(EditorInfo.IME_ACTION_UNSPECIFIED);
							}else if(return_key_type.equals("done")){
								mTextField.setImeOptions(EditorInfo.IME_ACTION_DONE);
							}else if(return_key_type.equals("call")){
								mTextField.setImeOptions(EditorInfo.IME_ACTION_UNSPECIFIED);
							}
						}
						mainView.requestFocus();
						mTextField.removeTextChangedListener(textInputWraper);
						mTextField.setText("");
						mTextField.addTextChangedListener(textInputWraper);
						mTextField.requestFocus();
						InputMethodManager imm = (InputMethodManager) mainView
								.getContext().getSystemService(
										Context.INPUT_METHOD_SERVICE);
						Log.i("tag", "" + mTextField.isFocused());
						Log.i("tag", "" + mainView.isFocused());
						Log.i("tag", "" + imm.showSoftInput(mTextField, 0));
						Log.d("GLSurfaceView", "showSoftInput");
						isSoftInputShow = true;
					}
					break;

				case HANDLER_CLOSE_IME_KEYBOARD:
					if (null != mTextField && mTextField.hasFocus() && mainView.requestFocus()) {
						mTextField.removeTextChangedListener(textInputWraper);
						InputMethodManager imm = (InputMethodManager) mainView
								.getContext().getSystemService(
										Context.INPUT_METHOD_SERVICE);
						imm.hideSoftInputFromWindow(
								mTextField.getWindowToken(), 0);
						Log.d("GLSurfaceView", "HideSoftInput");
						isSoftInputShow = false;
					}
					break;
				case HANDLER_AFTER_IME_KEYBOARD:
					if (isSoftInputShow) {
						mainView.requestFocus();
						queueEvent(new Runnable() {
							@Override
							public void run() {
								mRenderer.handleKeyDown(KeyEvent.KEYCODE_BACK);
							}
						});
						isSoftInputShow = false;
					}
					break;
				}
			}
		};

	}

	public void onPause() {
		queueEvent(new Runnable() {
			@Override
			public void run() {
				mRenderer.handleOnPause();
			}
		});
		isSoftInputShow = false;
		super.onPause();
	}

	public void onResume() {
		super.onResume();
	    nativeOnResume();
		//MGRenderer.bOnResume = true;
		// ��╋拷锟斤拷锟斤拷锟斤拷Render锟斤拷锟�onResume锟斤拷��э拷��わ拷锟�Render锟斤拷锟�onSurfaceChanged锟斤拷锟斤拷锟姐�★拷��锟斤拷 //
	}

	public static void ShowSoftInput(String keyboard_type) {
	   	Message msg = new Message();
    	msg.what = HANDLER_OPEN_IME_KEYBOARD;
    	msg.obj = keyboard_type;
    	handler.sendMessage(msg);
	}

	public static void HideSoftInput() {
		Message msg = new Message();
    	msg.what = HANDLER_CLOSE_IME_KEYBOARD;
    	handler.sendMessage(msg);
	}

	private static InputMethodManager imm = null;
	public static MGGLSurfaceView _this;
	InputConnection mIC;

	// /////////////////////////////////////////////////////////////////////////
	// for text input
	// /////////////////////////////////////////////////////////////////////////
	private final static int HANDLER_OPEN_IME_KEYBOARD = 2;
	private final static int HANDLER_CLOSE_IME_KEYBOARD = 3;
	private final static int HANDLER_AFTER_IME_KEYBOARD = 4;
	private static Handler handler;
	public static TextInputWraper textInputWraper;
	private static MITextField mTextField;
	public static native void nativeOnResume();

	public MITextField getTextField() {
		return mTextField;
	}

	public void setTextField(MITextField view) {
		mTextField = view;
		if (null != mTextField && null != textInputWraper) {
			// LinearLayout.LayoutParams linearParams =
			// (LinearLayout.LayoutParams) mTextField.getLayoutParams();
			// linearParams.height = 0;
			// mTextField.setLayoutParams(linearParams);
			mTextField.setImeOptions(EditorInfo.IME_FLAG_NO_EXTRACT_UI);
			mTextField.setBackgroundColor(0);
			mTextField.setFocusable(true);
			mTextField.setFocusableInTouchMode(true);
			mTextField.setMainView(this);
			mTextField.setRenderer(mRenderer);
			mTextField.setOnEditorActionListener(textInputWraper);
		}
	}

	public static void openIMEKeyboard() {
		Message msg = new Message();
		msg.what = HANDLER_OPEN_IME_KEYBOARD;
		msg.obj = mainView.getContentText();
		handler.sendMessage(msg);
	}

	private String getContentText() {
		return mRenderer.getContentText();
	}

	public void insertText(final String text) {
		mRenderer.handleInsertText(text);
	}

	public void deleteBackward() {
		mRenderer.handleDeleteBackward();
	}

	// /////////////////////////////////////////////////////////////////////////
	// for touch event
	// /////////////////////////////////////////////////////////////////////////

	public boolean onTouchEvent(final MotionEvent event) {
		final int pointerNumber = event.getPointerCount();
		final int[] ids = new int[pointerNumber];
		final float[] xs = new float[pointerNumber];
		final float[] ys = new float[pointerNumber];

		for (int i = 0; i < pointerNumber; i++) {
			ids[i] = event.getPointerId(i);
			xs[i] = event.getX(i);
			ys[i] = event.getY(i);
		}
		int action = event.getAction();
		switch (action & MotionEvent.ACTION_MASK) {
		case MotionEvent.ACTION_DOWN:
			// there are only one finger on the screen
			final int idDown = (action & MotionEvent.ACTION_POINTER_ID_MASK) >> MotionEvent.ACTION_POINTER_ID_SHIFT;// event.getPointerId(0);
			final float xDown = event.getX(idDown);
			final float yDown = event.getY(idDown);
//			Log.v("z11ActionTest", "ACTION_DOWN"+ids.length);
			mRenderer.handleActionDown(idDown, xDown, yDown);
			break;

		case MotionEvent.ACTION_POINTER_DOWN:
			final int idPointerDown = (action & MotionEvent.ACTION_POINTER_ID_MASK) >> MotionEvent.ACTION_POINTER_ID_SHIFT;
			final float xPointerDown = event.getX(idPointerDown);
			final float yPointerDown = event.getY(idPointerDown);
//			Log.v("z11ActionTest", "ACTION_POINTER_DOWN"+ids.length);
			mRenderer.handleActionDown(idPointerDown, xPointerDown, yPointerDown);
			break;

		case MotionEvent.ACTION_MOVE:
			//Log.v("z11ActionTest", "ACTION_MOVE"+ids.length);
			mRenderer.handleActionMove(ids, xs, ys);
			break;

		case MotionEvent.ACTION_POINTER_UP:
			final int idPointerUp = (action & MotionEvent.ACTION_POINTER_ID_MASK) >> MotionEvent.ACTION_POINTER_ID_SHIFT;
			final float xPointerUp = event.getX(idPointerUp);
			final float yPointerUp = event.getY(idPointerUp);
//			Log.v("z11ActionTest", "ACTION_POINTER_UP"+ids.length);
			mRenderer.handleActionUp(idPointerUp, xPointerUp, yPointerUp);
			break;

		case MotionEvent.ACTION_UP:
			// there are only one finger on the screen
			final int idUp = (action & MotionEvent.ACTION_POINTER_ID_MASK) >> MotionEvent.ACTION_POINTER_ID_SHIFT;// event.getPointerId(0);
			final float xUp = event.getX(idUp);
			final float yUp = event.getY(idUp);
//			Log.v("z11ActionTest", "ACTION_UP"+ids.length);
			mRenderer.handleActionUp(idUp, xUp, yUp);
			break;

		case MotionEvent.ACTION_CANCEL:
//			for(int i = 0; i < ids.length; ++i)
//			{
//				Log.v("z11ActionTest", "ACTION_CANCEL" + ids[i] + xs[i] + ys[i]);
//			}
			mRenderer.handleActionCancel(ids, xs, ys);
			break;
		}
		return true;
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		Log.i("MGGLSurfaceView TAG", "onKeyDown" + String.valueOf(keyCode));
		super.onKeyDown(keyCode, event);
		if(keyCode == KeyEvent.KEYCODE_VOLUME_UP||keyCode==KeyEvent.KEYCODE_VOLUME_DOWN){
			return false;
		}
		int asciiKey = event.getUnicodeChar(event.getMetaState());
		if ((asciiKey < 32 || asciiKey > 126) && asciiKey != 0x0A && asciiKey != 0x0D){
			asciiKey = keyCode;
		}
		return mRenderer.handleKeyDown(keyCode);
	}
	// Show an event in the LogCat view, for debugging
	public void onLayout() {
		Message msg = new Message();
		msg.what = HANDLER_AFTER_IME_KEYBOARD;
		handler.sendMessage(msg);
	}
}
