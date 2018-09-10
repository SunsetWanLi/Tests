LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)


LOCAL_ARM_MODE := arm
LOCAL_PATH =	$(Z11_PATH)/ExternalLib/skia/src/

#############################################################
################ Common.mk ##################################
#############################################################

common_SRC_FILES := \
	core/Sk64.cpp \
	core/SkAAClip.cpp \
	core/SkAdvancedTypefaceMetrics.cpp \
	core/SkAlphaRuns.cpp \
	core/SkAnnotation.cpp \
	core/SkBBoxHierarchy.cpp \
	core/SkBBoxHierarchyRecord.cpp \
	core/SkBBoxRecord.cpp \
	core/SkBitmap.cpp \
	core/SkBitmapHeap.cpp \
	core/SkBitmapProcShader.cpp \
	core/SkBitmapProcState.cpp \
	core/SkBitmapProcState_matrixProcs.cpp \
	core/SkBitmapSampler.cpp \
	core/SkBitmap_scroll.cpp \
	core/SkBlitMask_D32.cpp \
	core/SkBlitRow_D16.cpp \
	core/SkBlitRow_D32.cpp \
	core/SkBlitRow_D4444.cpp \
	core/SkBlitter.cpp \
	core/SkBlitter_4444.cpp \
	core/SkBlitter_A1.cpp \
	core/SkBlitter_A8.cpp \
	core/SkBlitter_ARGB32.cpp \
	core/SkBlitter_RGB16.cpp \
	core/SkBlitter_Sprite.cpp \
	core/SkBuffer.cpp \
	core/SkCanvas.cpp \
	core/SkChunkAlloc.cpp \
	core/SkClipStack.cpp \
	core/SkColor.cpp \
	core/SkColorFilter.cpp \
	core/SkColorTable.cpp \
	core/SkComposeShader.cpp \
	core/SkConcaveToTriangles.cpp \
	core/SkConfig8888.cpp \
	core/SkCordic.cpp \
	core/SkCubicClipper.cpp \
	core/SkData.cpp \
	core/SkDebug.cpp \
	core/SkDeque.cpp \
	core/SkDevice.cpp \
	core/SkDeviceProfile.cpp \
	core/SkDither.cpp \
	core/SkDraw.cpp \
	core/SkEdge.cpp \
	core/SkEdgeBuilder.cpp \
	core/SkEdgeClipper.cpp \
	core/SkFilterProc.cpp \
	core/SkFlate.cpp \
	core/SkFlattenable.cpp \
	core/SkFlattenableBuffers.cpp \
	core/SkFloat.cpp \
	core/SkFloatBits.cpp \
	core/SkFontHost.cpp \
	core/SkGeometry.cpp \
	core/SkGlyphCache.cpp \
	core/SkGraphics.cpp \
	core/SkImageFilter.cpp \
	core/SkInstCnt.cpp \
	core/SkLineClipper.cpp \
	core/SkMallocPixelRef.cpp \
	core/SkMask.cpp \
	core/SkMaskFilter.cpp \
	core/SkMaskGamma.cpp \
	core/SkMath.cpp \
	core/SkMatrix.cpp \
	core/SkMemory_stdlib.cpp \
	core/SkMetaData.cpp \
	core/SkOrderedReadBuffer.cpp \
	core/SkOrderedWriteBuffer.cpp \
	core/SkPackBits.cpp \
	core/SkPaint.cpp \
	core/SkPath.cpp \
	core/SkPathEffect.cpp \
	core/SkPathHeap.cpp \
	core/SkPathMeasure.cpp \
	core/SkPicture.cpp \
	core/SkPictureFlat.cpp \
	core/SkPicturePlayback.cpp \
	core/SkPictureRecord.cpp \
	core/SkPictureStateTree.cpp \
	core/SkPixelRef.cpp \
	core/SkPoint.cpp \
	core/SkProcSpriteBlitter.cpp \
	core/SkPtrRecorder.cpp \
	core/SkQuadClipper.cpp \
	core/SkRasterClip.cpp \
	core/SkRasterizer.cpp \
	core/SkRect.cpp \
	core/SkRefCnt.cpp \
	core/SkRefDict.cpp \
	core/SkRegion.cpp \
	core/SkRegion_path.cpp \
	core/SkRegion_rects.cpp \
	core/SkRTree.cpp \
	core/SkScalar.cpp \
	core/SkScalerContext.cpp \
	core/SkScan.cpp \
	core/SkScan_Antihair.cpp \
	core/SkScan_AntiPath.cpp \
	core/SkScan_Hairline.cpp \
	core/SkScan_Path.cpp \
	core/SkShader.cpp \
	core/SkSpriteBlitter_ARGB32.cpp \
	core/SkSpriteBlitter_RGB16.cpp \
	core/SkStream.cpp \
	core/SkString.cpp \
	core/SkStroke.cpp \
	core/SkStrokerPriv.cpp \
	core/SkTLS.cpp \
	core/SkTSearch.cpp \
	core/SkTypeface.cpp \
	core/SkTypefaceCache.cpp \
	core/SkUnPreMultiply.cpp \
	core/SkUtils.cpp \
	core/SkUtilsArm.cpp \
	core/SkWriter32.cpp \
	core/SkXfermode.cpp \
	effects/Sk1DPathEffect.cpp \
	effects/Sk2DPathEffect.cpp \
	effects/SkArithmeticMode.cpp \
	effects/SkAvoidXfermode.cpp \
	effects/SkBitmapSource.cpp \
	effects/SkBlendImageFilter.cpp \
	effects/SkBlurDrawLooper.cpp \
	effects/SkBlurImageFilter.cpp \
	effects/SkBlurMask.cpp \
	effects/SkBlurMaskFilter.cpp \
	effects/SkColorFilterImageFilter.cpp \
	effects/SkColorFilters.cpp \
	effects/SkColorMatrix.cpp \
	effects/SkColorMatrixFilter.cpp \
	effects/SkCornerPathEffect.cpp \
	effects/SkDashPathEffect.cpp \
	effects/SkDiscretePathEffect.cpp \
	effects/SkEmbossMask.cpp \
	effects/SkEmbossMaskFilter.cpp \
	effects/SkKernel33MaskFilter.cpp \
	effects/SkLayerDrawLooper.cpp \
	effects/SkLayerRasterizer.cpp \
	effects/SkLightingImageFilter.cpp \
	effects/SkMagnifierImageFilter.cpp \
	effects/SkMatrixConvolutionImageFilter.cpp \
	effects/SkMorphologyImageFilter.cpp \
	effects/SkPaintFlagsDrawFilter.cpp \
	effects/SkPixelXorXfermode.cpp \
	effects/SkPorterDuff.cpp \
	effects/SkSingleInputImageFilter.cpp \
	effects/SkStippleMaskFilter.cpp \
	effects/SkTableColorFilter.cpp \
	effects/SkTableMaskFilter.cpp \
	effects/SkTestImageFilters.cpp \
	effects/SkTransparentShader.cpp \
	effects/gradients/SkBitmapCache.cpp \
	effects/gradients/SkClampRange.cpp \
	effects/gradients/SkGradientShader.cpp \
	effects/gradients/SkLinearGradient.cpp \
	effects/gradients/SkRadialGradient.cpp \
	effects/gradients/SkSweepGradient.cpp \
	effects/gradients/SkTwoPointConicalGradient.cpp \
	effects/gradients/SkTwoPointRadialGradient.cpp \
	image/SkImage.cpp \
	image/SkSurface.cpp \
	images/bmpdecoderhelper.cpp \
	images/SkFlipPixelRef.cpp \
	images/SkImageDecoder.cpp \
	images/SkImageDecoder_Factory.cpp \
	images/SkImageDecoder_libjpeg.cpp \
	images/SkImageDecoder_libpng.cpp \
	images/SkImageDecoder_libbmp.cpp \
	images/SkImageEncoder.cpp \
	images/SkImageEncoder_Factory.cpp \
	images/SkJpegUtility.cpp \
	images/SkScaledBitmapSampler.cpp \
	images/SkImageRef.cpp \
	images/SkImageRef_GlobalPool.cpp \
	images/SkImageRefPool.cpp \
	images/SkImages.cpp \
	images/SkPageFlipper.cpp \

##############################################################

ifeq ($(SupportNeno),1)
common_CFLAGS += -mfpu=neon
LOCAL_ARM_NEON := true
ARCH_ARM_HAVE_VFP :=true
endif

# need a flag to tell the C side when we're on devices with large memory
# budgets (i.e. larger than the low-end devices that initially shipped)
ifeq ($(ARCH_ARM_HAVE_VFP),true)
    common_CFLAGS += -DANDROID_LARGE_MEMORY_DEVICE
endif

ifneq ($(ARCH_ARM_HAVE_VFP),true)
	common_CFLAGS += -DSK_SOFTWARE_FLOAT
endif

ifeq ($(LOCAL_ARM_NEON),true)
	common_CFLAGS += -D__ARM_HAVE_NEON
endif

#####################################################################
###########################  ports ####################################
#####################################################################
common_SRC_FILES += \
	core/SkMMapStream.cpp \
	ports/SkDebug_android.cpp \
	ports/SkFontDescriptor.cpp \
	ports/SkFontHost_sandbox_none.cpp \
	ports/SkGlobalInitialization_default.cpp \
	ports/SkFontHost_tables.cpp \
	ports/SkOSFile_stdio.cpp \
	ports/SkTime_Unix.cpp \
	ports/SkXMLParser_empty.cpp \
	ports/SkThread_pthread.cpp \
	ports/SkFontHost_android.cpp \
	ports/SkFontHost_FreeType.cpp \
	ports/SkFontHost_FreeType_common.cpp \
	ports/FontHostConfiguration_android.cpp \
#	ports/SkFontHost_none.cpp \
#	ports/SkMemory_malloc.cpp \

#####################################################################

#####################################################################
########################### arm mode #################################
#####################################################################
ifeq ($(TARGET_ARCH),arm)

ifeq ($(LOCAL_ARM_NEON),true)
common_SRC_FILES += \
	opts/SkBitmapProcState_arm_neon.cpp \
	opts/SkBitmapProcState_matrixProcs_neon.cpp \
	opts/SkBlitRow_opts_arm_neon.cpp \
	opts/memset16_neon.S \
	opts/memset32_neon.S
endif

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
common_SRC_FILES += \
	opts/opts_check_arm.cpp \
	opts/memset.arm.S \
	opts/SkBitmapProcState_opts_arm.cpp \
	opts/SkBlitRow_opts_arm.cpp
endif

ifeq ($(TARGET_ARCH_ABI),armeabi)
common_SRC_FILES += \
	opts/SkBlitRow_opts_none.cpp \
	opts/SkBitmapProcState_opts_none.cpp \
	opts/SkUtils_opts_none.cpp
endif

else

common_SRC_FILES += \
	opts/SkBlitRow_opts_none.cpp \
	opts/SkBitmapProcState_opts_none.cpp \
	opts/SkUtils_opts_none.cpp
		
endif
#####################################################################
	
ifeq ($(NO_FALLBACK_FONT),true)
	common_CFLAGS += -DNO_FALLBACK_FONT
endif

common_CFLAGS += -DSK_FONTHOST_FREETYPE_RUNTIME_VERSION=0x020400 \
-DSK_CAN_USE_DLOPEN=0 \
-DSK_BUILD_FOR_ANDROID \
-DSK_BUILD_FOR_ANDROID_NDK \
-DSK_ALLOW_STATIC_GLOBAL_INITIALIZERS=0 \
-DSK_RELEASE \
-DGR_RELEASE=1 \
-DNDEBUG \
-DFT2_BUILD_LIBRARY \
-DSK_ENABLE_LIBPNG \
-DSK_SUPPORT_GPU=0
#-DSK_SCALAR_IS_FLOAT \
#-DSK_CAN_USE_FLOAT

LOCAL_CPPFLAGS := \
	-fno-rtti \
	-fno-exceptions

include $(CLEAR_VARS)
common_C_INCLUDES := $(Z11_PATH)/ExternalLib/skia/src/ \
		$(Z11_PATH)/ExternalLib/freetype2/include \
		$(Z11_PATH)/ExternalLib/freetype2/include/freetype \
		$(Z11_PATH)/Platform/Android/prebuilt/libpng/include \
		$(Z11_PATH)/Platform/Android/prebuilt/libjpeg/include \
		$(Z11_PATH)/ExternalLib/expat \
$(Z11_PATH)/ExternalLib/skia/src/core \
 $(Z11_PATH)/ExternalLib/skia/src/effects \
 $(Z11_PATH)/ExternalLib/skia/src/images \
 $(Z11_PATH)/ExternalLib/skia/src/image \
 $(Z11_PATH)/ExternalLib/skia/src/utils \
 $(Z11_PATH)/ExternalLib/skia/src/sfnt \
$(Z11_PATH)/ExternalLib/skia/include/ \
$(Z11_PATH)/ExternalLib/skia/include/core \
$(Z11_PATH)/ExternalLib/skia/include/config \
$(Z11_PATH)/ExternalLib/skia/include/ports \
$(Z11_PATH)/ExternalLib/skia/include/effects \
$(Z11_PATH)/ExternalLib/skia/include/images \
$(Z11_PATH)/ExternalLib/skia/include/xml \

LOCAL_SRC_FILES += $(common_SRC_FILES)
LOCAL_CFLAGS += $(common_CFLAGS)
LOCAL_C_INCLUDES += $(common_C_INCLUDES) \


LOCAL_MODULE:= libskia

LOCAL_COPY_HEADERS_TO := $(common_COPY_HEADERS_TO)
LOCAL_COPY_HEADERS := $(common_COPY_HEADERS)

include $(BUILD_STATIC_LIBRARY)


