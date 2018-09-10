LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

include $(JNI_DIR)/freetype/Android.mk
include $(JNI_DIR)/jpeg/Android.mk
include $(JNI_DIR)/zlib/Android.mk
include $(JNI_DIR)/libpng/Android.mk
include $(JNI_DIR)/CommonBase/Android.mk
include $(JNI_DIR)/GraphicCore/Android.mk
include $(JNI_DIR)/GUI/Android.mk
include $(JNI_DIR)/Application/Android.mk
include $(CLEAR_VARS)
LOCAL_PATH:=$(JNI_DIR)

LOCAL_MODULE:= libCommon

LOCAL_CFLAGS += -O3  -DANDROID_NDK \
                -DDISABLE_IMPORTGL \
		-DHAVE_PTHREADS

LOCAL_CFLAGS += "-DDARWIN_NO_CARBON"
LOCAL_CFLAGS += "-DFT2_BUILD_LIBRARY"

LOCAL_SRC_FILES:= \

LOCAL_STATIC_LIBRARIES := wchar_static

LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog

include $(BUILD_STATIC_LIBRARY)
