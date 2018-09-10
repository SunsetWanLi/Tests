LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PATH =	$(Z11_PATH)/ExternalLib/gpsd
LOCAL_MODULE:= libgpsd

LOCAL_CFLAGS += -O3  -DANDROID_NDK \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
	


LOCAL_SRC_FILES:= \
	gps_maskdump.c \
	gpsutils.c \
	json.c \
	libgps_core.c \
	libgps_dbus.c \
	libgps_json.c \
	libgps_shm.c \
	libgps_sock.c \
	libgpsmm.cpp \
	netlib.c \
	shared_json.c


LOCAL_C_INCLUDES += $(LOCAL_PATH)

include $(BUILD_STATIC_LIBRARY)
