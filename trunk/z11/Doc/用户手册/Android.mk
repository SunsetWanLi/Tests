JNI_DIR := $(call my-dir)
APP_NAME := LianlianCharge
SRC_PREFIX := $(APP_NAME)/Application
CURRENT_JNI_WORK_PATH :=Apps/$(APP_NAME)/proj.android/jni
LOCAL_PATH := $(JNI_DIR)
#Z11_PATH := $(subst $(CURRENT_JNI_WORK_PATH),,$(JNI_DIR))z11/trunk/z11
Z11_PATH := $(JNI_DIR)/../../../../z11/trunk/z11
ANDROID_PROJECT_PATH := $(Z11_PATH)/Workspace/GNU
LIB_PATH := /Users/wangzhipeng/develop/workdir/Apps/BuildResult/local/armeabi-v7a

include $(CLEAR_VARS)
LOCAL_MODULE := commonbase
LOCAL_SRC_FILES := $(LIB_PATH)/libCommonBase.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := platform
LOCAL_SRC_FILES := $(LIB_PATH)/libPlatform.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := graphiccore
LOCAL_SRC_FILES := $(LIB_PATH)/libGraphicCore.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := gui
LOCAL_SRC_FILES := $(LIB_PATH)/libGUI.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := system
LOCAL_SRC_FILES := $(LIB_PATH)/libSystem.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := embeddedgis
LOCAL_SRC_FILES := $(LIB_PATH)/libEmbeddedGIS.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := ft2
LOCAL_SRC_FILES := $(LIB_PATH)/libft2.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := ftgles
LOCAL_SRC_FILES := $(LIB_PATH)/libftgles.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := skia
LOCAL_SRC_FILES := $(LIB_PATH)/libskia.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := cryptopp
LOCAL_SRC_FILES := $(LIB_PATH)/libcryptopp.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := expat
LOCAL_SRC_FILES := $(LIB_PATH)/libexpat.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := zxing
LOCAL_SRC_FILES := $(LIB_PATH)/libzxing.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := json
LOCAL_SRC_FILES := $(LIB_PATH)/libjson.a
include $(PREBUILT_STATIC_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE := curlpp
LOCAL_SRC_FILES := $(LIB_PATH)/libcurlpp.a
include $(PREBUILT_STATIC_LIBRARY)
include $(Z11_PATH)/Platform/Android/prebuilt/libcurl/Android.mk
include $(Z11_PATH)/Platform/Android/prebuilt/libjpeg/Android.mk
include $(Z11_PATH)/Platform/Android/prebuilt/libpng/Android.mk

#Build the main library
include $(CLEAR_VARS)

LOCAL_PATH :=  $(JNI_DIR)/../../..

LOCAL_MODULE := z11

LOCAL_CFLAGS := -O3 -DANDROID_NDK \
    -DHAVE_PTHREADS \
    -DWITH_NONAMESPACES \

LOCAL_CFLAGS += "-DFT2_BUILD_LIBRARY"

LOCAL_SRC_FILES := \
		$(SRC_PREFIX)/../proj.android/jni/main.cpp \
		$(SRC_PREFIX)/../../../z11/trunk/z11/Platform/Android/jni/AndroidJni.cpp \
		$(SRC_PREFIX)/Common/threedes.cpp \
		$(SRC_PREFIX)/Common/Common.cpp \
		$(SRC_PREFIX)/Common/Header.cpp \
		$(SRC_PREFIX)/Common/LunarCalendar.cpp \
		$(SRC_PREFIX)/Common/TimeState.cpp \
		$(SRC_PREFIX)/CommonControls/AnnotationView.cpp \
		$(SRC_PREFIX)/CommonControls/CommonPickerView.cpp \
		$(SRC_PREFIX)/CommonControls/CommonTableView.cpp \
		$(SRC_PREFIX)/CommonControls/HalfPanView.cpp \
		$(SRC_PREFIX)/CommonControls/IndexButton.cpp \
		$(SRC_PREFIX)/CommonControls/LoadingView.cpp \
		$(SRC_PREFIX)/CommonControls/MIActionSheet.cpp \
		$(SRC_PREFIX)/CommonControls/MyView.cpp \
		$(SRC_PREFIX)/CommonControls/PanRightView.cpp \
		$(SRC_PREFIX)/CommonControls/PanView.cpp \
		$(SRC_PREFIX)/CommonControls/PicturePanView.cpp \
		$(SRC_PREFIX)/CommonControls/PictureView.cpp \
		$(SRC_PREFIX)/CommonControls/PullRefreshTableView.cpp \
		$(SRC_PREFIX)/CommonControls/PullRefreshView.cpp \
		$(SRC_PREFIX)/CommonControls/RefreshableScrollView.cpp \
		$(SRC_PREFIX)/CommonControls/ScreenShotView.cpp \
		$(SRC_PREFIX)/CommonControls/ScrollTextView.cpp \
		$(SRC_PREFIX)/CommonControls/SearchBoxView.cpp \
		$(SRC_PREFIX)/CommonControls/SegmentControl.cpp \
		$(SRC_PREFIX)/CommonControls/SegmentLikeControl.cpp \
		$(SRC_PREFIX)/CommonControls/SingleSheet.cpp \
		$(SRC_PREFIX)/CommonControls/StatusButton.cpp \
		$(SRC_PREFIX)/CommonControls/TextButton.cpp \
		$(SRC_PREFIX)/CommonControls/TimePickerView.cpp \
		$(SRC_PREFIX)/CommonControls/TopView.cpp \
		$(SRC_PREFIX)/DBHelper/DBHelper.cpp \
		$(SRC_PREFIX)/DBRecord/DBRecord.cpp \
		$(SRC_PREFIX)/Download/DownloadImageButton.cpp \
		$(SRC_PREFIX)/Download/MGFTPDownload.cpp \
		$(SRC_PREFIX)/Download/MyFetcherDelegate.cpp \
		$(SRC_PREFIX)/Download/MyFetcherTask.cpp \
		$(SRC_PREFIX)/Download/MyFetcherThread.cpp \
		$(SRC_PREFIX)/Download/MyURLDownloadDelegate.cpp \
		$(SRC_PREFIX)/GUIRootView/AppView.cpp \
		$(SRC_PREFIX)/GUIRootView/AppViewController.cpp \
		$(SRC_PREFIX)/GUIRootView/GUIRootView.cpp \
		$(SRC_PREFIX)/GUIRootView/GUIRootViewController.cpp \
		$(SRC_PREFIX)/IndexView/IndexView.cpp \
		$(SRC_PREFIX)/IndexView/IndexViewController.cpp \
		$(SRC_PREFIX)/AppBean/MsgBean.cpp \
		$(SRC_PREFIX)/AppBean/WordBean.cpp \
		$(SRC_PREFIX)/AppBean/OrderBean.cpp \
		$(SRC_PREFIX)/AppBean/BalBean.cpp \
		$(SRC_PREFIX)/AppBean/PostBean.cpp \
		$(SRC_PREFIX)/AccountView/AccountView.cpp \
		$(SRC_PREFIX)/AccountView/AccountViewController.cpp \
		$(SRC_PREFIX)/AndMoreView/AndMoreView.cpp \
		$(SRC_PREFIX)/AndMoreView/AndMoreViewController.cpp \
		$(SRC_PREFIX)/AndMoreView/ContactView.cpp \
		$(SRC_PREFIX)/AndMoreView/ContactViewController.cpp \
		$(SRC_PREFIX)/ApplicationView/ApplicationView.cpp \
		$(SRC_PREFIX)/ApplicationView/ApplicationViewController.cpp \
		$(SRC_PREFIX)/BottomView/BottomView.cpp \
		$(SRC_PREFIX)/BottomView/BottomViewController.cpp \
		$(SRC_PREFIX)/BulletinView/BulletinView.cpp \
		$(SRC_PREFIX)/BulletinView/BulletinViewController.cpp \
		$(SRC_PREFIX)/BulletinView/BulletinDetailView.cpp \
		$(SRC_PREFIX)/BulletinView/BulletinDetailViewController.cpp \
		$(SRC_PREFIX)/InitialView/InitialView.cpp \
		$(SRC_PREFIX)/InitialView/InitialViewController.cpp \
		$(SRC_PREFIX)/LoginView/LoginView.cpp \
		$(SRC_PREFIX)/LoginView/LoginViewController.cpp \
		$(SRC_PREFIX)/LoginView/GestureView.cpp \
		$(SRC_PREFIX)/LoginView/GestureViewController.cpp \
		$(SRC_PREFIX)/MessageView/MessageView.cpp \
		$(SRC_PREFIX)/MessageView/MessageViewController.cpp \
		$(SRC_PREFIX)/MessageView/MsgDetailView.cpp \
		$(SRC_PREFIX)/MessageView/MsgDetailViewController.cpp \
		$(SRC_PREFIX)/OrderView/OrderView.cpp \
		$(SRC_PREFIX)/OrderView/OrderViewController.cpp \
		$(SRC_PREFIX)/ReconView/ReconView.cpp \
		$(SRC_PREFIX)/ReconView/ReconViewController.cpp	\
		$(SRC_PREFIX)/SetupView/SetupView.cpp \
		$(SRC_PREFIX)/SetupView/SetupViewController.cpp \
		$(SRC_PREFIX)/WebUrls/WebUrls.cpp \
		$(SRC_PREFIX)/WebView/WebView.cpp \
		$(SRC_PREFIX)/WebView/WebViewController.cpp \
		
LOCAL_C_INCLUDES += \
	$(Z11_PATH) \
	$(Z11_PATH)/include \
	$(Z11_PATH)/ExternalLib/freetype2/include \
	$(Z11_PATH)/ExternalLib/libcurlpp/include

LOCAL_WHOLE_STATIC_LIBRARIES := skia cryptopp expat zxing json curlpp curl_static_prebuilt cocos_jpeg_static cocos_libpng_static wchar_static commonbase platform graphiccore gui system embeddedgis ft2 ftgles
#LOCAL_SHARED_LIBRARIES := ffmpeg
LOCAL_LDLIBS :=  -lGLESv1_CM -ldl -llog -lz

include $(BUILD_SHARED_LIBRARY)
