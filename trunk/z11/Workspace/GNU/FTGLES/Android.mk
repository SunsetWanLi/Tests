LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_PATH =	$(Z11_PATH)/ExternalLib/ftgles

LOCAL_CFLAGS += -O3  -DANDROID_NDK -DFTGL_LIBRARY_STATIC -DFT2_BUILD_LIBRARY

LOCAL_SRC_FILES := \
	FTBuffer.cpp \
	FTCharmap.cpp \
	FTContour.cpp \
	FTFace.cpp \
	FTGlyphContainer.cpp \
	FTLibrary.cpp \
	FTPoint.cpp \
	FTSize.cpp \
	FTVectoriser.cpp \
	FTFont/FTBitmapFont.cpp \
	FTFont/FTBufferFont.cpp \
	FTFont/FTFont.cpp \
	FTFont/FTFontGlue.cpp \
	FTFont/FTEfficientFont.cpp \
	FTFont/FTOutlineFont.cpp \
	FTFont/FTPixmapFont.cpp \
	FTFont/FTPolygonFont.cpp \
	FTFont/FTTextureFont.cpp \
	FTGL/ftglesGlue.cpp \
	FTGlyph/FTBitmapGlyph.cpp \
	FTGlyph/FTBufferGlyph.cpp \
	FTGlyph/FTEfficientGlyph.cpp \
	FTGlyph/FTGlyph.cpp \
	FTGlyph/FTGlyphGlue.cpp \
	FTGlyph/FTOutlineGlyph.cpp \
	FTGlyph/FTPixmapGlyph.cpp \
	FTGlyph/FTPolygonGlyph.cpp \
	FTGlyph/FTTextureGlyph.cpp \
	FTLayout/FTLayout.cpp \
	FTLayout/FTLayoutGlue.cpp \
	FTLayout/FTSimpleLayout.cpp \
	FTLayout/FTCustomLayout.cpp \
	iGLU-1.0.0/libtess/dict.c \
	iGLU-1.0.0/libtess/geom.c \
	iGLU-1.0.0/libtess/memalloc.c \
	iGLU-1.0.0/libtess/mesh.c \
	iGLU-1.0.0/libtess/normal.c \
	iGLU-1.0.0/libtess/priorityq-heap.c \
	iGLU-1.0.0/libtess/priorityq.c \
	iGLU-1.0.0/libtess/render.c \
	iGLU-1.0.0/libtess/sweep.c \
	iGLU-1.0.0/libtess/tess.c \
	iGLU-1.0.0/libtess/tessmono.c \
	iGLU-1.0.0/libutil/error.c \
	iGLU-1.0.0/libutil/glue.c \
	iGLU-1.0.0/libutil/project.c \
	iGLU-1.0.0/libutil/registry.c \


LOCAL_MODULE:= ftgles

LOCAL_C_INCLUDES := $(Z11_PATH)/ExternalLib/ftgles  \
		$(Z11_PATH)/ExternalLib/freetype2/include  \
		$(Z11_PATH)/ExternalLib/freetype2/include/freetype  \
		$(Z11_PATH)/ExternalLib/ftgles/iGLU-1.0.0/include  \

include $(BUILD_STATIC_LIBRARY)
