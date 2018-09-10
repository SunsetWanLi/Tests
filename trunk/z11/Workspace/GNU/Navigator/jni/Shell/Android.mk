LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#LOCAL_PATH = $(JNI_DIR)/../../../../../z11
LOCAL_PATH = $(Z11_DIR)

LOCAL_MODULE:= Shell

LOCAL_CFLAGS += -O3  -DANDROID_NDK \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \


	

LOCAL_SRC_FILES:= \
	Retiguide/Shell/Handler/ActionHandler.cpp \
	Retiguide/Shell/Handler/HandlerQueue.cpp \
	Retiguide/Shell/Handler/MapAdjustHandler.cpp \
	Retiguide/Shell/Handler/MapMouseHandler.cpp \
	Retiguide/Shell/Handler/ShortestPathHandler.cpp \
	Retiguide/Shell/Handler/TrafficInfoHandler.cpp \
	Retiguide/Shell/Handler/UserCtrlHandler.cpp \
	Retiguide/Shell/Model/CrossModel.cpp \
	Retiguide/Shell/Model/LightModel.cpp \
	Retiguide/Shell/Model/MapModel.cpp \
	Retiguide/Shell/Model/Model.cpp \
	Retiguide/Shell/Model/ModelQueue.cpp \
	Retiguide/Shell/Model/PathModel.cpp \
	Retiguide/Shell/Model/TrafficModel.cpp \
	Retiguide/Shell/Model/UserModel.cpp \
	Retiguide/Shell/Navigation/Navigation.cpp \
	Retiguide/Shell/Navigation/NaviMap.cpp \
	Retiguide/Shell/Util/MapMover.cpp \
	Retiguide/Shell/Util/MapResizer.cpp


LOCAL_STATIC_LIBRARIES := wchar_static

include $(BUILD_STATIC_LIBRARY)
