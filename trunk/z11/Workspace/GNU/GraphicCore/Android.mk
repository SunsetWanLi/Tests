LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PATH =	$(Z11_PATH)/GraphicCore

LOCAL_MODULE:= GraphicCore

LOCAL_CFLAGS += -O3  -DANDROID_NDK \
	"-DFT2_BUILD_LIBRARY" \
	-DSK_FONTHOST_FREETYPE_RUNTIME_VERSION=0x020400 \
-DSK_CAN_USE_DLOPEN=0 \
-DSK_BUILD_FOR_ANDROID \
-DSK_BUILD_FOR_ANDROID_NDK \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
		$(Z11_PATH)/ExternalLib/freetype2/include \
		$(Z11_PATH)/ExternalLib/skia/src/ \
$(Z11_PATH)/ExternalLib/skia/src/core \
 $(Z11_PATH)/ExternalLib/skia/src/effects \
$(Z11_PATH)/ExternalLib/skia/include/ \
$(Z11_PATH)/ExternalLib/skia/include/core \
$(Z11_PATH)/ExternalLib/skia/include/config \
$(Z11_PATH)/ExternalLib/skia/include/ports \
$(Z11_PATH)/ExternalLib/skia/include/effects \
$(Z11_PATH)/ExternalLib/skia/include/images \
$(Z11_PATH)/ExternalLib/skia/include/xml \
$(Z11_PATH)/ExternalLib/freetype2/include/freetype \


LOCAL_SRC_FILES:= \
	Canvas/CGCanvas.cpp \
	Canvas/CGBitmap.cpp \
	Canvas/CGRegionClipper.cpp \
	FreeTypeFont.cpp \
	GraphicStructure.cpp \
	MGBMPCodec.cpp \
	MGColor.cpp \
	MGPersistenceImage.cpp \
	MGFont.cpp \
	MGGraphicEngine.cpp \
	MGGraphicEngineContext.cpp \
	MGPPMCodec.cpp \
	MGImage.cpp \
	MGCanvas.cpp \
	MGPath.cpp \
	MGPaint.cpp \
	MGJPEGCodec.cpp \
	MGPlatformFont.cpp \
	MGPNGCodec.cpp \
	MGFTGLESFontContextManager.cpp \
	MGModel.cpp \
	MGFTGLESFont.cpp \
	MGGrid.cpp \

LOCAL_STATIC_LIBRARIES := ftgles CommonBase System wchar_static

include $(BUILD_STATIC_LIBRARY)
