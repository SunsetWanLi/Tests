LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PATH =	$(Z11_PATH)/Automation
LOCAL_MODULE:= Automation

LOCAL_CFLAGS += -O3  -DANDROID_NDK \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../
	


LOCAL_SRC_FILES:= \
	MIAutoAction.cpp \
	MIAutomation.cpp \
	SocketServer/RaymeCommand.cpp \
	SocketServer/RaymeNode.cpp \
	SocketServer/RaymeReader.cpp \
	SocketServer/RaymeServer.cpp \
	SocketServer/RaymeSocket.cpp \
	SocketServer/RaymeWriter.cpp
	
LOCAL_STATIC_LIBRARIES := CommonBase wchar_static

include $(BUILD_STATIC_LIBRARY)
