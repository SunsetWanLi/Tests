LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#LOCAL_PATH = $(JNI_DIR)/../../../../../z11
LOCAL_PATH = $(Z11_DIR)

LOCAL_MODULE:= Render

LOCAL_CFLAGS += -O3  -DANDROID_NDK \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \


	
	#Retiguide/Render/Basic_Render.cpp \

LOCAL_SRC_FILES:= \
	Retiguide/Render/RasterRender/ImagePyramid.cpp \
	Retiguide/Render/RasterRender/ImagePyramidUpload.cpp \
	Retiguide/Render/RasterRender/Pyramid.cpp \
	Retiguide/Render/RasterRender/PyramidRequest.cpp \
	Retiguide/Render/RasterRender/PyramidScaler.cpp \
	Retiguide/Render/RasterRender/PyramidStringConvert.cpp \
	Retiguide/Render/RasterRender/PyramidTileIndex.cpp \
	Retiguide/Render/RasterRender/RasterRender.cpp \
	Retiguide/Render/VctorRender/Annotation/AnnotationLayer.cpp \
	Retiguide/Render/VctorRender/Annotation/AnnotationLayerRender.cpp \
	Retiguide/Render/VctorRender/Annotation/AnnotationLayerStyle.cpp \
	Retiguide/Render/VctorRender/Cross/CrossView.cpp \
	Retiguide/Render/VctorRender/Feature/FeatureLayer.cpp \
	Retiguide/Render/VctorRender/Feature/FeatureLayerRender.cpp \
	Retiguide/Render/VctorRender/Feature/FeatureLayerStyle.cpp \
	Retiguide/Render/VctorRender/Map/MapFrame.cpp \
	Retiguide/Render/VctorRender/Map/MapTerrCtrl.cpp \
	Retiguide/Render/VctorRender/Map/MapView.cpp \
	Retiguide/Render/VctorRender/Map/ScreenPixMap.cpp \
	Retiguide/Render/VctorRender/Render/StyleRender.cpp \
	Retiguide/Render/VctorRender/Render/StyleTranslate.cpp \
	Retiguide/Render/VctorRender/Render/GmRender/GeoGmImageRender.cpp \
	Retiguide/Render/VctorRender/Render/GmRender/GeoGmLineRender.cpp \
	Retiguide/Render/VctorRender/Render/GmRender/GeoGmPointRender.cpp \
	Retiguide/Render/VctorRender/Render/GmRender/GeoGmPolygonRender.cpp \
	Retiguide/Render/VctorRender/Render/GmRender/GeoGmRender.cpp \
	Retiguide/Render/VctorRender/Render/LabelRender/GeoLabelLineRender.cpp \
	Retiguide/Render/VctorRender/Render/LabelRender/GeoLabelPointRender.cpp \
	Retiguide/Render/VctorRender/Render/LabelRender/GeoLabelPolygonRender.cpp \
	Retiguide/Render/VctorRender/Render/LabelRender/GeoLabelPosTrans.cpp \
	Retiguide/Render/VctorRender/Render/LabelRender/GeoLabelRender.cpp


LOCAL_STATIC_LIBRARIES := wchar_static

include $(BUILD_STATIC_LIBRARY)
