LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
#LOCAL_PATH = $(JNI_DIR)/../../../../../ExternalLib/libcurlpp/src
LOCAL_PATH =	$(Z11_PATH)/ExternalLib/libcurlpp/src

LOCAL_SRC_FILES := \
		curlpp/cURLpp.cpp \
		curlpp/Easy.cpp \
		curlpp/Exception.cpp \
		curlpp/Form.cpp \
		curlpp/Info.cpp \
		curlpp/internal/CurlHandle.cpp \
		curlpp/internal/OptionList.cpp \
		curlpp/internal/OptionSetter.cpp \
		curlpp/internal/SList.cpp \
		curlpp/Multi.cpp \
		curlpp/OptionBase.cpp \
		curlpp/Options.cpp \
		utilspp/LifetimeLibrary.cpp \
		utilspp/PrivateMembers.cpp \



LOCAL_MODULE:= libcurlpp
#CURLPP_PATH =	$(subst $(WORK_JNI),/ExternalLib/libcurlpp/src,$(JNI_DIR))
LOCAL_C_INCLUDES := $(Z11_PATH)/ExternalLib/libcurlpp/include \
		$(Z11_PATH)/Platform/Android/prebuilt/libcurl/include

LOCAL_STATIC_LIBRARIES := wchar_static
include $(BUILD_STATIC_LIBRARY)
