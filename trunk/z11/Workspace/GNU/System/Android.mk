LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PATH =	$(Z11_PATH)/System
LOCAL_MODULE:= System

LOCAL_CFLAGS += -O3  -DANDROID_NDK \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
	$(LOCAL_PATH)/../ExternalLib/libcurlpp/include \
	$(LOCAL_PATH)/../Platform/Android/prebuilt/libcurl/include
	


LOCAL_SRC_FILES:= \
	ClipBoard/MGClipBoard.cpp \
	Device/MGDevice.cpp  \
	IO/MGBundle.cpp \
	IO/MGFileManager.cpp \
	MotionService/MGAcceleration.cpp \
	MotionService/MGRotation.cpp \
	MotionService/MGMotionManager.cpp \
	MotionService/MGAttitude.cpp \
	LocationService/MGHeading.cpp \
	LocationService/MGLocation.cpp \
	LocationService/MGLocationManager.cpp \
	LocationService/MKSimulateGPSReader.cpp \
	LocationService/MGLocationRegion.cpp \
	Media/Audio/MGAudioPlayer.cpp \
	Media/Audio/MGAudioPlayerDelegate.cpp \
	Media/Video/MGVideoPlayer.cpp \
    Media/Video/MGVideoPlayerDelegate.cpp \
	Media/Capture/MGCameraCaptureManager.cpp \
	Media/Recorder/MGMediaRecorder.cpp \
	MediaPicker/MGMediaPicker.cpp \
	MGSocket/MGSocket.cpp \
	Network/FTP/FtpParseInfo.cpp \
	Network/FTP/FtpUtil.cpp \
    Network/HttpPost/MGHttpPost.cpp \
	Network/URL/MGURL.cpp \
	Network/URL/MGURLConnection.cpp \
	Network/URL/MGURLDownload.cpp \
	Network/URL/MGURLDownloadThread.cpp \
	Network/URL/MGURLRequest.cpp \
	Network/URL/MGURLResponse.cpp \
	Network/URLFetcher/MGURLFetcher.cpp \
	Network/URLFetcher/MGURLFetcherThread.cpp \
	Shared/MGLog.cpp \
	Shared/MGPath.cpp \
	Shared/NetDef.cpp \
	Shared/Syncable.cpp \
	Shared/Utility.cpp \
	Shared/MGTime.cpp \
	SQLite/sqlite3.c \
	Thread/MGSemaphore.cpp \
	Thread/MGMutex.cpp \
	Thread/MGThread.cpp \
	Timer/MGTimer.cpp \
	WebService/MGRequestSoapObject.cpp \
	WebService/MGSoapObject.cpp \
	WebService/MGWebService.cpp \
	WebService/MGWebServiceThread.cpp \
	WebView/MGWebViewManager.cpp \
	
#	SQLite/CppSQLite3.cpp \
#	SQLite/CppSQLite3Demo.cpp \
LOCAL_STATIC_LIBRARIES := CommonBase wchar_static

include $(BUILD_STATIC_LIBRARY)
