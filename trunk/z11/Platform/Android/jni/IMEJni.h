#ifndef __ANDROID_IME_JNI_H__
#define __ANDROID_IME_JNI_H__

extern "C"

{
    extern void setKeyboardStateJNI(int bOpen);
	extern void imeNativeInsertJNI(const char *);
	extern void imeNativeDeleteBackward();
	extern const char *imeNativeGetContentText();
}

#endif // __ANDROID_IME_JNI_H__
