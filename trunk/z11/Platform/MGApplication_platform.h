//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (MGApplication_platform.h)
/// @brief (Application�Ŀ�ƽ̨ʵ��)
///
///(���ļ�ʵ�ֵĹ��ܵ�����)
///
/// @version 0.1   (�汾����)
/// @author        (Gu RongFang)
/// @date          (11-08-29)
///
///
///    �޶�˵��������汾
//////////////////////////////////////////////////////////////////////////
#ifndef _MGAPPLICATION_PLATFORM_H_
#define _MGAPPLICATION_PLATFORM_H_
#include "MGPlatformConfig.h"

#if (MG_TARGET_PLATFORM == MG_PLATFORM_WIN32)
#include "Win32/MGApplication_win32.h"
#elif (MG_TARGET_PLATFORM == MG_PLATFORM_WINCE)
#include "WinCE/MGApplication_wince.h"
#elif (MG_TARGET_PLATFORM == MG_PLATFORM_ANDROID)
#include "Android/MGApplication_android.h"
#elif (MG_TARGET_PLATFORM == MG_PLATFORM_LINUX)
#include "Linux/MGApplication_linux.h"
#elif (MG_TARGET_PLATFORM == MG_PLATFORM_IOS)
//# include "ios/MGApplication_ios.h"
#else
#error
#endif

#endif //_MGAPPLICATION_PLATFORM_H_
