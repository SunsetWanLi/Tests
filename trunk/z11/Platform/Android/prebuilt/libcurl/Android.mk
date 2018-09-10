LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := curl_static_prebuilt
LOCAL_MODULE_FILENAME := curl
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libcurl.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_STATIC_LIBRARIES += ssl_static_prebuilt
LOCAL_STATIC_LIBRARIES += crypto_static_prebuilt
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := crypto_static_prebuilt
LOCAL_MODULE_FILENAME := crypto
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libcrypto.a
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_MODULE := ssl_static_prebuilt
LOCAL_MODULE_FILENAME := ssl
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libssl.a
include $(PREBUILT_STATIC_LIBRARY)
