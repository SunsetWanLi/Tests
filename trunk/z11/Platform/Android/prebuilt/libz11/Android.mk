LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := embeddedgis_static_prebuilt
LOCAL_MODULE_FILENAME := EmbeddedGIS
LOCAL_SRC_FILES := libs/armeabi-v7a/libEmbeddedGIS.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := gui_static_prebuilt
LOCAL_MODULE_FILENAME := GUI
LOCAL_SRC_FILES := libs/armeabi-v7a/libGUI.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := platform_static_prebuilt
LOCAL_MODULE_FILENAME := Platform
LOCAL_SRC_FILES := libs/armeabi-v7a/libPlatform.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../
include $(PREBUILT_STATIC_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE := graphiccore_static_prebuilt
LOCAL_MODULE_FILENAME := GraphicCore
LOCAL_SRC_FILES := libs/armeabi-v7a/libGraphicCore.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../
include $(PREBUILT_STATIC_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE := commonbase_static_prebuilt
LOCAL_MODULE_FILENAME := CommonBase
LOCAL_SRC_FILES := libs/armeabi-v7a/libCommonBase.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := system_static_prebuilt
LOCAL_MODULE_FILENAME := System
LOCAL_SRC_FILES := libs/armeabi-v7a/libSystem.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := ftgles_static_prebuilt
LOCAL_MODULE_FILENAME := ftgles
LOCAL_SRC_FILES := libs/armeabi-v7a/libftgles.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := freetype2_static_prebuilt
LOCAL_MODULE_FILENAME := ft2
LOCAL_SRC_FILES := libs/armeabi-v7a/libft2.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := json_static_prebuilt
LOCAL_MODULE_FILENAME := json
LOCAL_SRC_FILES := libs/armeabi-v7a/libjson.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := curlpp_static_prebuilt
LOCAL_MODULE_FILENAME := curlpp
LOCAL_SRC_FILES := libs/armeabi-v7a/libcurlpp.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../../
include $(PREBUILT_STATIC_LIBRARY)