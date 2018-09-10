# this is now the default FreeType build for Android
#
ifndef USE_FREETYPE
USE_FREETYPE := 2.4.2
endif

ifeq ($(USE_FREETYPE),2.4.2)
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# compile in ARM mode, since the glyph loader/renderer is a hotspot
# when loading complex pages in the browser
#
LOCAL_ARM_MODE := arm
LOCAL_PATH =	$(Z11_PATH)/ExternalLib/freetype2

LOCAL_SRC_FILES:= \
	src/autofit/autofit.c \
	src/bdf/bdf.c \
	src/cff/cff.c \
	src/base/ftbase.c \
	src/base/ftbbox.c \
	src/base/ftbitmap.c \
	src/base/ftglyph.c \
	src/gzip/ftgzip.c \
	src/base/ftinit.c \
	src/lzw/ftlzw.c \
	src/base/ftstroke.c \
	src/base/ftsystem.c \
	src/base/fttype1.c \
	src/base/ftfstype.c \
	src/base/ftfntfmt.c \
	src/base/ftlcdfil.c \
	src/pcf/pcf.c \
	src/pfr/pfr.c \
	src/psaux/psaux.c \
	src/pshinter/pshinter.c \
	src/psnames/psnames.c \
	src/raster/raster.c \
	src/sfnt/sfnt.c \
	src/smooth/smooth.c \
	src/truetype/truetype.c \
	src/type1/type1.c \
	src/cid/type1cid.c \
	src/type42/type42.c \
	src/winfonts/winfnt.c

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/builds \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/include/freetype

LOCAL_CFLAGS += -W -Wall
LOCAL_CFLAGS += -fPIC -DPIC
LOCAL_CFLAGS += "-DDARWIN_NO_CARBON"
LOCAL_CFLAGS += "-DFT2_BUILD_LIBRARY"

# enable the FreeType internal memory debugger in the simulator
# you need to define the FT2_DEBUG_MEMORY environment variable
# when running the program to activate it. It will dump memory
# statistics when FT_Done_FreeType is called
#
ifeq ($(TARGET_SIMULATOR),true)
LOCAL_CFLAGS += "-DFT_DEBUG_MEMORY"
endif

ifeq ($(SupportNeno),1)
LOCAL_ARM_NEON := true
endif

# the following is for testing only, and should not be used in final builds
# of the product
#LOCAL_CFLAGS += "-DTT_CONFIG_OPTION_BYTECODE_INTERPRETER"

LOCAL_CFLAGS += -O3

LOCAL_MODULE:= ft2

include $(BUILD_STATIC_LIBRARY)
endif
