LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

#LOCAL_PATH = $(JNI_DIR)/../../../../../z11
LOCAL_PATH = $(Z11_DIR)

LOCAL_MODULE:= Common

LOCAL_CFLAGS += -O3  -DANDROID_NDK \

LOCAL_C_INCLUDES := $(LOCAL_PATH) \

#Retiguide/Common/CRS/CoordConvertor.cpp \
#Retiguide/Common/CRS/CRSPacker.cpp \
#Retiguide/Common/CRS/CRSUnpacker.cpp \
#Retiguide/Common/CRS/CRTPacker.cpp \
#Retiguide/Common/CRS/back/CRTALG_ProjectionMercator.cpp \
#Retiguide/Common/Types/DisplayIndex.cpp \
	#Retiguide/Common/Types/GeoTypes/GeoAttribute.cpp \
	#Retiguide/Common/Types/GeoTypes/GeographyObject.cpp \
	#Retiguide/Common/Types/GeoTypes/Geometry.cpp \
	#Retiguide/Common/Types/GeoTypes/GeometryImage.cpp \
	#Retiguide/Common/Types/GeoTypes/GeometryLine.cpp \
	#Retiguide/Common/Types/GeoTypes/GeometryPoint.cpp \
	#Retiguide/Common/Types/GeoTypes/GeometryPolygon.cpp \
	#Retiguide/Common/Types/GeoTypes/GuiderGeoPointNoTrigger.cpp \
	#Retiguide/Common/Types/GeoTypes/GuiderGeoPointTrigger.cpp \
	#Retiguide/Common/Types/GeoTypes/Types.cpp \
	#Retiguide/Common/Util/ErrorInfo.cpp \
	

LOCAL_SRC_FILES:= \
	Retiguide/Common/err_out.cpp \
	Retiguide/Common/String_Tools.cpp \
	Retiguide/Common/CRS/ComputeEquation.cpp \
	Retiguide/Common/CRS/CoordSystem.cpp \
	Retiguide/Common/CRS/CRS.cpp \
	Retiguide/Common/CRS/CRS_Factory.cpp \
	Retiguide/Common/CRS/CRS_Generic.cpp \
	Retiguide/Common/CRS/CRS_Geocentric.cpp \
	Retiguide/Common/CRS/CRS_Geography.cpp \
	Retiguide/Common/CRS/CRS_Project.cpp \
	Retiguide/Common/CRS/CRT.cpp \
	Retiguide/Common/CRS/CRTALG_GeographyToGeography.cpp \
	Retiguide/Common/CRS/CRTALG_ProjectionGauss.cpp \
	Retiguide/Common/CRS/CRTALG_ProjectionMercator.cpp \
	Retiguide/Common/CRS/CRTALG_ProjectionMercatorTransverse.cpp \
	Retiguide/Common/CRS/CRT_Affine.cpp \
	Retiguide/Common/CRS/CRT_Compound.cpp \
	Retiguide/Common/CRS/CRT_Conversion.cpp \
	Retiguide/Common/CRS/CRT_Factory.cpp \
	Retiguide/Common/CRS/CRT_GeographyToGeography5480.cpp \
	Retiguide/Common/CRS/CRT_Math.cpp \
	Retiguide/Common/CRS/CRT_NoChange.cpp \
	Retiguide/Common/CRS/CRT_Projection.cpp \
	Retiguide/Common/CRS/CRT_ProjectionGauss.cpp \
	Retiguide/Common/CRS/CRT_ProjectionGauss_Zoned.cpp \
	Retiguide/Common/CRS/CRT_ProjectionLambert.cpp \
	Retiguide/Common/CRS/CRT_ProjectionMercator.cpp \
	Retiguide/Common/CRS/CRT_ProjectionMercatorTransverse.cpp \
	Retiguide/Common/CRS/CRT_ProjectionToProjection5480.cpp \
	Retiguide/Common/CRS/CRT_Transformation.cpp \
	Retiguide/Common/CRS/Datum.cpp \
	Retiguide/Common/CRS/err_out.cpp \
	Retiguide/Common/CRS/GlobalCrt.cpp \
	Retiguide/Common/CRS/SpatialRefMeta.cpp \
	Retiguide/Common/Share/Scope.cpp \
	Retiguide/Common/Share/ScreenPos2D.cpp \
	Retiguide/Common/Share/UserScreenRect.cpp \
	Retiguide/Common/Types/FeatureClassInfo.cpp \
	Retiguide/Common/Types/Fraction.cpp \
	Retiguide/Common/Types/ProgramLanguage.cpp \
	Retiguide/Common/Types/Settings.cpp \
	Retiguide/Common/Types/DataFetcherTypes/TypeDefs.cpp \
	Retiguide/Common/Types/WorkSpaceTypes/TerrCtrl.cpp \
	Retiguide/Common/Types/WorkSpaceTypes/WorkSpace.cpp \
	Retiguide/Common/Util/Algorithmic.cpp \
	Retiguide/Common/Util/PureMath.cpp \
	Retiguide/Common/Util/ScopeResizer.cpp \
	Retiguide/Common/Util/Transformer.cpp \
	Retiguide/Common/Util/TString.cpp \
	Retiguide/Common/Util/Coordinate/Coordinate.cpp \
	Retiguide/Common/Util/Coordinate/Matrix.cpp \
	Retiguide/Common/Util/Coordinate/Quaternion.cpp \
	Retiguide/Common/Util/Coordinate/Vector3.cpp \
	Retiguide/Common/Util/Coordinate/Vector4.cpp

LOCAL_STATIC_LIBRARIES := wchar_static

include $(BUILD_STATIC_LIBRARY)
