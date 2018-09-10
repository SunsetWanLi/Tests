LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PATH =	$(Z11_PATH)/CommonBase/

LOCAL_MODULE:= CommonBase

LOCAL_CFLAGS += -O3  -DANDROID_NDK \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \


LOCAL_SRC_FILES:= \
	MGObject.cpp \
	MGDictionary.cpp \
	MGData.cpp \
	MGDataReader.cpp \
	MGStructure.cpp \
	MGString.cpp \
	MGMutableData.cpp \
	Memory/MGRefObject.cpp \
	Memory/MGAutoreleasePool.cpp \
	Algorithm/Base64/Base64.cpp \
	Algorithm/Base64/modp_b64.cpp \

include $(BUILD_STATIC_LIBRARY)
