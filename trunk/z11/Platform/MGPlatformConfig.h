//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (MGPlatformConfig.h)
/// @brief (≈‰÷√Àµ√˜ «ƒƒ∏ˆ∆ΩÃ®µƒ)
///
///(≈‰÷√Àµ√˜ «ƒƒ∏ˆ∆ΩÃ®µƒ)
///
/// @version 0.1   (∞Ê±æ…˘√˜)
/// @author        (Gu RongFang)
/// @date          (11-08-29)
///
///
///    –ﬁ∂©Àµ√˜£∫◊Ó≥ı∞Ê±æ
//////////////////////////////////////////////////////////////////////////
#ifndef _MGPLATFORMCONFIG_H_
#define _MGPLATFORMCONFIG_H_


//////////////////////////////////////////////////////////////////////////
// pre configure
//////////////////////////////////////////////////////////////////////////

// ∂®“Â Ωƒƒ∏ˆ∆ΩÃ®µƒ
#define MG_PLATFORM_UNKNOWN            0
#define MG_PLATFORM_IOS                1
#define MG_PLATFORM_ANDROID            2
#define MG_PLATFORM_WIN32              3
#define MG_PLATFORM_WINCE			   4
#define MG_PLATFORM_LINUX              5

// Determine tartet platform by compile environment macro.
#define MG_TARGET_PLATFORM             MG_PLATFORM_UNKNOWN

// iphone
#if ! MG_TARGET_PLATFORM && (defined(__IPHONE_2_0) || defined(COMPILE_ON_IOS))
//#if ! MG_TARGET_PLATFORM && (defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR))
    #undef  MG_TARGET_PLATFORM
    #define MG_TARGET_PLATFORM         MG_PLATFORM_IOS
    #define MG_SUPPORT_MULTITHREAD 0
    #define MG_SUPPORT_UNICODE 0
    #define MG_SUPPORT_PVRTC
//#warning "IPHONE"
#endif

// android
#if ! MG_TARGET_PLATFORM && defined(ANDROID_NDK)
//#if ! MG_TARGET_PLATFORM && defined(ANDROID)
    #undef  MG_TARGET_PLATFORM
#define MG_TARGET_PLATFORM         MG_PLATFORM_ANDROID
#endif

//linux
#if ! MG_TARGET_PLATFORM && defined(linux)
    #undef  MG_TARGET_PLATFORM
#define MG_TARGET_PLATFORM         MG_PLATFORM_LINUX
#endif

// win32
#if ! MG_TARGET_PLATFORM && defined(_WIN32) || defined(_WIN64)

#ifdef WINCE //WINCE
	#undef  MG_TARGET_PLATFORM
	#define MG_TARGET_PLATFORM         MG_PLATFORM_WINCE
#else //WIN32 ◊¿√Ê
	#undef  MG_TARGET_PLATFORM
	#define MG_TARGET_PLATFORM         MG_PLATFORM_WIN32
#endif

#define MG_SUPPORT_MULTITHREAD     1
#if defined(UNICODE)
#define MG_SUPPORT_UNICODE     1
#else
#define MG_SUPPORT_UNICODE     0

#endif
#endif

//////////////////////////////////////////////////////////////////////////
// user configure
//////////////////////////////////////////////////////////////////////////

// Check user assigned target platform.
#if defined(MG_UNDER_IOS)
    #undef  MG_TARGET_PLATFORM
    #define MG_TARGET_PLATFORM         MG_PLATFORM_IOS
#endif

#if defined(MG_UNDER_ANDROID)
    #undef  MG_TARGET_PLATFORM
    #define MG_TARGET_PLATFORM         MG_PLATFORM_ANDROID
#endif

#if defined(MG_UNDER_WIN32)
    #undef  MG_TARGET_PLATFORM
    #define MG_TARGET_PLATFORM         MG_PLATFORM_WIN32
#endif

// Check user assigned supportive of multi-thread
#if defined(MG_ENABLE_MULTITHREAD)
    #undef  MG_SUPPORT_MULTITHREAD
    #define MG_SUPPORT_MULTITHREAD     1
#elif defined(MG_DISABLE_MULTITHREAD)
    #undef  MG_SUPPORT_MULTITHREAD
    #define MG_SUPPORT_MULTITHREAD     0
#endif

// Check user assigned supportive of unicode
#if defined(MG_ENABLE_UNICODE)
#undef  MG_SUPPORT_UNICODE
#define MG_SUPPORT_UNICODE             1
#elif defined(MG_DISABLE_UNICODE)
#undef  MG_SUPPORT_UNICODE
#define MG_SUPPORT_UNICODE             0
#endif

//////////////////////////////////////////////////////////////////////////
// post configure
//////////////////////////////////////////////////////////////////////////

// check user set platform
//#warning "MG_TARGET_PLATFORM == MG_PLATFORM_UNKNOWN?"
#if  MG_TARGET_PLATFORM == MG_PLATFORM_UNKNOWN
//    #error  "Can not recognize the target platform, compling under a unsupported platform?"
#warning  "Can not recognize the target platform, compling under a unsupported platform?"

#endif

// z11-x havn't support multi-thread yet
#undef  MG_SUPPORT_MULTITHREAD
#define MG_SUPPORT_MULTITHREAD         0

// z11-x havn't support unicode yet
#undef  MG_SUPPORT_UNICODE
#define MG_SUPPORT_UNICODE             0

// Check the supportive of platform
#if (MG_TARGET_PLATFORM == MG_PLATFORM_WIN32)
#pragma warning (disable:4127)
#endif  // MG_PLATFORM_WIN32




#endif //_MGPLATFORMCONFIG_H_
