LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PATH = $(JNI_DIR)/../../../../../z11

LOCAL_MODULE:= DataFetcher

LOCAL_CFLAGS += -O3  -DANDROID_NDK \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \


LOCAL_SRC_FILES:= \
	Retiguide/DataFetcher/DataFetcher.cpp \
	Retiguide/DataFetcher/DataFetcherThread.cpp \
	Retiguide/DataFetcher/DataFetcherUtil.cpp \
	Retiguide/DataFetcher/DataReader.cpp \
	Retiguide/DataFetcher/DataWriter.cpp \
	Retiguide/DataFetcher/FclsInfoAdapter.cpp \
	Retiguide/DataFetcher/FileAdapter.cpp \
	Retiguide/DataFetcher/FileBuffer.cpp \
	Retiguide/DataFetcher/FileDataFetcher.cpp \
	Retiguide/DataFetcher/GGPointBuffer.cpp \
	Retiguide/DataFetcher/GuiderGeoPointAdapter.cpp \
	Retiguide/DataFetcher/MapConfigAdapter.cpp \
	Retiguide/DataFetcher/NetReader.cpp \
	Retiguide/DataFetcher/PathDef.cpp \
	Retiguide/DataFetcher/ProgramLanguageAdapter.cpp \
	Retiguide/DataFetcher/RadarAdapter.cpp \
	Retiguide/DataFetcher/RamBuffer.cpp \
	Retiguide/DataFetcher/SettingsAdapter.cpp \
	Retiguide/DataFetcher/TerrCtrlAdapter.cpp \
	Retiguide/DataFetcher/TypeDefs.cpp \
	Retiguide/DataFetcher/WorkspaceAdapter.cpp \
	Retiguide/DataFetcher/XmlReader.cpp

LOCAL_STATIC_LIBRARIES := wchar_static

include $(BUILD_STATIC_LIBRARY)
