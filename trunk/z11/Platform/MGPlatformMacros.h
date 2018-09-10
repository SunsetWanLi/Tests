//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2011 Zhejiang University.
///    All rights reserved.
///
/// @file    (MGPlatformMacros.h)
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
#ifndef _MGPLATFORMMACROS_H_
#define _MGPLATFORMMACROS_H_
#define MG_DEBUG 1
/**
 * define some platform specific macros
 */
#include "MGPlatformConfig.h"

#define MacGLView					void
#define NSWindow					        void

/** @def MG_ENABLE_CACHE_TEXTTURE_DATA
Enable it if you want to cache the texture data.
Basically,it's only enabled in android

It's new in z11-x since v0.99.5
*/
#if (MG_TARGET_PLATFORM == MG_PLATFORM_ANDROID)
    #define MG_ENABLE_CACHE_TEXTTURE_DATA       1
#else
    #define MG_ENABLE_CACHE_TEXTTURE_DATA       0
#endif


// generic macros

// namespace z11 {}
#define NS_MG_BEGIN                     namespace z11 {
#define NS_MG_END                       }
#define USING_NS_MG                    using namespace zz

/** MG_PROPERTY_READONLY is used to declare a protected variable.
 We can use getter to read the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 @warning : The getter is a public virtual function, you should rewrite it first.
 The variables and methods declared after MG_PROPERTY_READONLY are all public.
 If you need protected or private, please declare.
 */
#define MG_PROPERTY_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void);

/** MG_PROPERTY is used to declare a protected variable.
 We can use getter to read the variable, and use the setter to change the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 "set + funName" is the name of the setter.
 @warning : The getter and setter are public virtual functions, you should rewrite them first.
 The variables and methods declared after MG_PROPERTY are all public.
 If you need protected or private, please declare.
 */
#define MG_PROPERTY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void);\
public: virtual void set##funName(varType var);

/** MG_SYNTHESIZE_READONLY is used to declare a protected variable.
 We can use getter to read the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 @warning : The getter is a public inline function.
 The variables and methods declared after MG_SYNTHESIZE_READONLY are all public.
 If you need protected or private, please declare.
 */
#define MG_SYNTHESIZE_READONLY(varType, varName, funName)\
protected: varType varName;\
public: inline varType get##funName(void) const { return varName; }

/** MG_SYNTHESIZE is used to declare a protected variable.
 We can use getter to read the variable, and use the setter to change the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 "set + funName" is the name of the setter.
 @warning : The getter and setter are public  inline functions.
 The variables and methods declared after MG_SYNTHESIZE are all public.
 If you need protected or private, please declare.
 */
#define MG_SYNTHESIZE(varType, varName, funName)\
protected: varType varName;\
public: inline varType get##funName(void) const { return varName; }\
public: inline void set##funName(varType var){ varName = var; }

#define MG_SAFE_DELETE(p)			if(p) { delete p; p = 0; }
#define MG_SAFE_DELETE_ARRAY(p)    if(p) { delete[] p; p = 0; }
#define MG_SAFE_FREE(p)			if(p) { free(p); p = 0; }
#define MG_SAFE_RELEASE(p)			if(p) { p->release(); }
#define MG_SAFE_RELEASE_NULL(p)	if(p) { p->release(); p = 0; }
#define MG_SAFE_RETAIN(p)			if(p) { p->retain(); }


// z11 debug
#if !defined(MG_DEBUG) || MG_DEBUG == 0
#define MGLOG(...)
#define MGLOGINFO(...)
#define MGLOGERROR(...)

#elif MG_DEBUG == 1
#define MGLOG(format, ...)      z11::MGLogD(format, ##__VA_ARGS__)
#define MGLOGERROR(format,...)  z11::MGLogD(format, ##__VA_ARGS__)
#define MGLOGINFO(format,...)   z11::MGLogD(format, ##__VA_ARGS__)

#elif MG_DEBUG > 1
#define MGLOG(format, ...)      z11::MGLogD(format, ##__VA_ARGS__)
#define MGLOGERROR(format,...)  z11::MGLogD(format, ##__VA_ARGS__)
#define MGLOGINFO(format,...)   z11::MGLogD(format, ##__VA_ARGS__)
#endif // MG_DEBUG

// shared library declartor
#define MG_DLL

// assertion
#include <assert.h>
#define MG_ASSERT(cond)                assert(cond)
#define MG_UNUSED_PARAM(unusedparam)   (void)unusedparam



// platform depended macros

#if (MG_TARGET_PLATFORM == MG_PLATFORM_WIN32)

    #undef MG_DLL
    #if defined(_USRDLL)
        #define MG_DLL     __declspec(dllexport)
    #else 		/* use a DLL library */
        #define MG_DLL     __declspec(dllimport)
    #endif

#endif  // MG_PLATFORM_WIN32



#endif //_MGPLATFORMMACROS_H_
