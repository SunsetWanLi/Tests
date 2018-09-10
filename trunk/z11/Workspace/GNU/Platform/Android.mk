LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PATH =	$(Z11_PATH)/Platform
LOCAL_MODULE:= Platform

LOCAL_CFLAGS += -O3  -DANDROID_NDK \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \


LOCAL_SRC_FILES:= \
	Android/jni/AssetManagerJni.cpp \
	Android/jni/IMEJni.cpp \
	Android/jni/IntentJni.cpp \
	Android/jni/JniHelper.cpp \
	Android/jni/LocationJni.cpp \
	Android/jni/MediaJni.cpp \
	Android/jni/MediaPlayerJni.cpp \
	Android/jni/MediaRecorderJni.cpp \
	Android/jni/MessageJni.cpp \
	Android/jni/SensorJni.cpp \
	Android/jni/SystemInfoJni.cpp \
	Android/jni/TouchesJni.cpp \
	Android/jni/WebViewJni.cpp \
	Android/MGNetworkReachability.cpp \
	Android/MGTimer_android.cpp \
	NetworkStatus/MGWifiStatusManager.cpp \
	Android/MGFileUtils.cpp \
	../ExternalLib/support/zip_support/zip.cpp \
	../ExternalLib/support/zip_support/unzip.cpp \
	../ExternalLib/support/zip_support/ioapi.cpp \
	Android/MGEGLView_android.cpp \
	Android/MGApplication_android.cpp \
	Android/jni/CameraJni.cpp \

LOCAL_STATIC_LIBRARIES := CommonBase wchar_static
# define the macro to compile through support/zip_support/ioapi.c                
LOCAL_CFLAGS += -DUSE_FILE32API
include $(BUILD_STATIC_LIBRARY)
