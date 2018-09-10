LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PATH =	$(Z11_PATH)/ExternalLib/expat
LOCAL_MODULE:= libexpat

LOCAL_CFLAGS += -O3  -DANDROID_NDK \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
	


LOCAL_SRC_FILES:= \
	xmlparse.c \
	xmlrole.c \
	xmltok.c \
	xmltok_impl.c \
	xmltok_ns.c 
	
include $(BUILD_STATIC_LIBRARY)
