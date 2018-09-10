LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#LOCAL_PATH = $(JNI_DIR)/../../../../../z11
LOCAL_PATH = $(Z11_DIR)

LOCAL_MODULE:= RenderEngine

LOCAL_CFLAGS += -O3  -DANDROID_NDK \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \


	

LOCAL_SRC_FILES:= \
	Retiguide/RenderEngine/GmRender/GmStyleRender.cpp \
	Retiguide/RenderEngine/GmRender/LineStyleRender.cpp \
	Retiguide/RenderEngine/GmRender/PointStyleRender.cpp \
	Retiguide/RenderEngine/GmRender/PolygonStyleRender.cpp \
	Retiguide/RenderEngine/ImageRender/ImageStyleRender.cpp \
	Retiguide/RenderEngine/LabelRender/LabelStyleRender.cpp


LOCAL_STATIC_LIBRARIES := wchar_static

include $(BUILD_STATIC_LIBRARY)
