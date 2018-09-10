LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#LOCAL_PATH = $(JNI_DIR)/../../../../../z11
LOCAL_PATH = $(Z11_DIR)

LOCAL_MODULE:= Embedded

LOCAL_CFLAGS += -O3  -DANDROID_NDK \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \


LOCAL_SRC_FILES:= \
	MapKit/Embedded/Application/AttributeMgr.cpp \
	MapKit/Embedded/Application/LayerMgr.cpp \
	MapKit/Embedded/Application/Pathfinding.cpp \
	MapKit/Embedded/Application/PathResult.cpp \
	MapKit/Embedded/Application/QueryMgr.cpp \
	MapKit/Embedded/Application/SimuNaviTool.cpp \
	MapKit/Embedded/BasicType/Attribute.cpp \
	MapKit/Embedded/BasicType/Line.cpp \
	MapKit/Embedded/BasicType/Point.cpp \
	MapKit/Embedded/BasicType/Polygon.cpp \
	MapKit/Embedded/BasicType/Rect.cpp \
	MapKit/Embedded/Features/Condense.cpp \
	MapKit/Embedded/Features/Data.cpp \
	MapKit/Embedded/Features/MapLayer.cpp \
	MapKit/Embedded/Features/Visitor.cpp \
	MapKit/Embedded/Network/AdjacencyList.cpp \
	MapKit/Embedded/Network/BindPoint.cpp \
	MapKit/Embedded/Network/HighNetwork.cpp \
	MapKit/Embedded/Network/HighNWSearching.cpp \
	MapKit/Embedded/Network/LookUpTable.cpp \
	MapKit/Embedded/Network/LowNetwork.cpp \
	MapKit/Embedded/Network/LowNWQuadTree.cpp \
	MapKit/Embedded/Network/LowNWSearching.cpp \
	MapKit/Embedded/Network/QuadtreeTable.cpp \
	MapKit/Embedded/Network/Utility.cpp \
	MapKit/Embedded/SpatialIndex/Node.cpp \
	MapKit/Embedded/SpatialIndex/PointBindLine.cpp \
	MapKit/Embedded/SpatialIndex/StaticRTree.cpp \
	MapKit/Embedded/StorageManager/Buffer.cpp \
	MapKit/Embedded/StorageManager/FIFOBuffer.cpp \
	MapKit/Embedded/StorageManager/StaticDiskStorageManager.cpp \
	MapKit/Embedded/Tools/SpatialMath.cpp \
	MapKit/Embedded/Tools/StringTools.cpp \
	MapKit/Embedded/Tools/Tools.cpp
	
LOCAL_STATIC_LIBRARIES := wchar_static

include $(BUILD_STATIC_LIBRARY)
