#ifndef __ANDROID_CAMERA_JNI_H__
#define __ANDROID_CAMERA_JNI_H__

extern "C"

{
    extern  void cameraOpenCamera(int position);
    extern  void cameraReleaseCamera();
}

#endif // __ANDROID_CAMERA_JNI_H__