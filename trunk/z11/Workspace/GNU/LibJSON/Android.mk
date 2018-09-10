LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
#LOCAL_PATH = $(JNI_DIR)/../../../../../ExternalLib/json
LOCAL_PATH =	$(Z11_PATH)/ExternalLib/json

LOCAL_SRC_FILES := \
	json_writer.cpp \
	json_reader.cpp \
	json_value.cpp \



LOCAL_MODULE:= libjson
LOCAL_C_INCLUDES := $(INCLUDE_FILE_PATH)/ExternalLib/json/json


LOCAL_STATIC_LIBRARIES := wchar_static
include $(BUILD_STATIC_LIBRARY)
