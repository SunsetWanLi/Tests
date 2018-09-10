//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (MGEGLView_platform.h)
/// @brief (本文件实现的功能的简述)
///
///(本文件实现的功能的详述)
///
/// @version 0.1   (版本声明)
/// @author        (Gu RongFang)
/// @date          (11-08-30)
///
///
///    修订说明：最初版本
//////////////////////////////////////////////////////////////////////////
#ifndef _MGEGLVIEW_PLATFORM_H_
#define _MGEGLVIEW_PLATFORM_H_


#include "MGPlatformConfig.h"

#if (MG_TARGET_PLATFORM == MG_PLATFORM_WIN32)
    #include "Win32/MGEGLView_win32.h"
#elif (MG_TARGET_PLATFORM == MG_PLATFORM_WINCE)
	#include "WinCE/MGEGLView_wince.h"
#elif (MG_TARGET_PLATFORM == MG_PLATFORM_ANDROID)
    #include "Android/MGEGLView_android.h"
#elif (MG_TARGET_PLATFORM == MG_PLATFORM_LINUX)
    #include "Linux/MGEGLView_linux.h"
#elif (MG_TARGET_PLATFORM == MG_PLATFORM_IOS)
    //#include "iOS/MGEGLView_ios.h"
#else
    //#error
#endif



#endif //_MGEGLVIEW_PLATFORM_H_
