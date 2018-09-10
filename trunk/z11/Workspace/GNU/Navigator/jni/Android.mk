JNI_DIR := $(call my-dir)
Z11_DIR := $(subst /Workspace/GNU/Navigator/jni,,$(JNI_DIR))
LOCAL_PATH := $(Z11_DIR)
include $(CLEAR_VARS)

#include $(JNI_DIR)/freetype/Android.mk
#include $(JNI_DIR)/jpeg/Android.mk
#include $(JNI_DIR)/zlib/Android.mk
#include $(JNI_DIR)/libpng/Android.mk
include $(JNI_DIR)/CommonBase/Android.mk
include $(JNI_DIR)/MapKit/Android.mk
include $(JNI_DIR)/DataFetcher/Android.mk
include $(JNI_DIR)/Render/Android.mk
include $(JNI_DIR)/RenderEngine/Android.mk
include $(JNI_DIR)/Shell/Android.mk
include $(JNI_DIR)/Common/Android.mk
include $(JNI_DIR)/System/Android.mk
#include $(JNI_DIR)/GraphicCore/Android.mk
#include $(JNI_DIR)/GUI/Android.mk
#include $(JNI_DIR)/Application/Android.mk


####################################################################################################################
#LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#LOCAL_PATH = $(JNI_DIR)/../../../../../z11
LOCAL_PATH = $(Z11_DIR)

LOCAL_MODULE:= opengldevice

LOCAL_CFLAGS := -O3 -DANDROID_NDK \
                -DDISABLE_IMPORTGL \
		-DHAVE_PTHREADS

LOCAL_CFLAGS += "-DFT2_BUILD_LIBRARY"

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
		$(LOCAL_PATH)/Retiguide


LOCAL_SRC_FILES:= \
	GUI/Animations/MIAnimationsManager.cpp \
	GUI/Animations/MIAnimationAlgorithm.cpp \
	GUI/Animations/MIAnimationLayer.cpp \
	GUI/Base/MIObject.cpp \
	GUI/Base/MIResponder.cpp \
	GUI/Base/MITouchTracer.cpp \
	GUI/Base/MIView.cpp \
	GUI/Controllers/MIViewController.cpp \
	GUI/Events/MIEvent.cpp \
	GUI/Events/MIEventQueue.cpp \
	GUI/Events/MIListener.cpp \
	GUI/Events/MITouch.cpp \
	GUI/GuiContainer/GuiContainer.cpp \
	GUI/LightGraphic/LightGraphic.cpp \
	GUI/MIControls/MIBaseTextView.cpp \
	GUI/MIControls/MIButton.cpp \
	GUI/MIControls/MIControl.cpp \
	GUI/MIControls/MIPictureView.cpp \
	GUI/MIControls/MILabel.cpp \
	GUI/MIControls/MITableView.cpp \
	GUI/MIControls/MITextField.cpp \
	GUI/MIControls/MITextArea.cpp \
	GUI/MIControls/MIEditableTextView.cpp \
	GUI/MIControls/MICursor.cpp \
	GUI/MIControls/MITextButton.cpp \
	GUI/MIControls/SimpleTableCell.cpp \
	
	
LOCAL_STATIC_LIBRARIES := wchar_static Embedded DataFetcher Render RenderEngine Shell Common System 


include $(BUILD_SHARED_LIBRARY)
#####################################################################################################################


#	GraphicCore/DDDevice16.cpp \
#	GraphicCore/DDFactory.cpp \
#	GraphicCore/DeviceFactory.cpp \
#	GraphicCore/DrawDevice16.cpp \
#	GraphicCore/DrawDevicePointer.cpp \
#	GraphicCore/GFontMannager.cpp \
#	GraphicCore/GLDrawDevice16.cpp \
#	GraphicCore/GLFactory.cpp \
#	GraphicCore/GXDevice16.cpp \
#	GraphicCore/GXFactory.cpp \
#	GraphicCore/MGCanvas.cpp \
#	GraphicCore/MGColor.cpp \
#	GraphicCore/MGImage.cpp \
#	GraphicCore/MGFont.cpp \