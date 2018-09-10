LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_PATH =	$(Z11_PATH)/ExternalLib/jpeg

LOCAL_SRC_FILES := \
	cdjpeg.c \
	djpeg.c \
	jaricom.c \
	jcapimin.c \
	jcapistd.c \
	jcarith.c \
	jccoefct.c \
	jccolor.c \
	jcdctmgr.c \
	jchuff.c \
	jcinit.c \
	jcmainct.c \
	jcmarker.c \
	jcmaster.c \
	jcomapi.c \
	jcparam.c \
	jcprepct.c \
	jcsample.c \
	jctrans.c \
	jdapimin.c \
	jdapistd.c \
	jdarith.c \
	jdatadst.c \
	jdatasrc.c \
	jdcoefct.c \
	jdcolor.c \
	jddctmgr.c \
	jdhuff.c \
	jdinput.c \
	jdmainct.c \
	jdmarker.c \
	jdmaster.c \
	jdmerge.c \
	jdpostct.c \
	jdsample.c \
	jdtrans.c \
	jerror.c \
	jfdctflt.c \
	jfdctfst.c \
	jfdctint.c \
	jidctflt.c \
	jidctfst.c \
	jidctint.c \
	jmemmgr.c \
	jmemnobs.c \
	jquant1.c \
	jquant2.c \
	jutils.c \
	rdcolmap.c \
	wrbmp.c \
	wrgif.c \
	wrppm.c \
	wrrle.c \
	wrtarga.c 


LOCAL_MODULE:= libjpeg
LOCAL_C_INCLUDES := $(INCLUDE_FILE_PATH)/ExternalLib/jpeg  

include $(BUILD_STATIC_LIBRARY)
