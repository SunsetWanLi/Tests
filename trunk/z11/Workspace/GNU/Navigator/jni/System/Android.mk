LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#LOCAL_PATH = $(JNI_DIR)/../../../../../z11
LOCAL_PATH = $(Z11_DIR)/System

LOCAL_MODULE:= System


LOCAL_CFLAGS += -O3  -DANDROID_NDK \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \


LOCAL_SRC_FILES:= \
	Device/MGDevice.cpp  \
	IO/MGBundle.cpp \
	Shared/MGLog.cpp \
	Shared/MGPath.cpp \
	Shared/NetDef.cpp \
	Shared/Syncable.cpp \
	Shared/Utility.cpp \
	Shared/MGTime.cpp \
	Thread/MGMutex.cpp \
	Thread/MGSemaphore.cpp \
	Thread/MGThread.cpp \

LOCAL_STATIC_LIBRARIES := CommonBase wchar_static

include $(BUILD_STATIC_LIBRARY)
