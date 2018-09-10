#pragma once
#ifndef _COMMON_DEF_H_
#define _COMMON_DEF_H_

#include <stddef.h> // size_t
#if defined(_WIN32) || defined(_WIN64)
#pragma warning (disable:4819) 
#else
	#include <unistd.h> // intptr_t
#endif

#if defined(_WIN32) || defined(_WIN64)
#ifdef WINCE
#include <winsock2.h>
#include <windows.h>
#else
#endif
#endif

#ifndef NULL
#define  NULL 0
#endif

#ifndef MAXVALUE
#define MAXVALUE(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MINVALUE
#define MINVALUE(a,b)            (((a) < (b)) ? (a) : (b))
#endif

namespace z11
{
#ifndef MIINTERFACE
#define MIINTERFACE class
#endif
typedef int int_32;
typedef unsigned int uint_32;
typedef short int_16;
typedef unsigned short uint_16;
typedef int bool_32;
typedef unsigned char byte_8;
typedef unsigned char uint_8;
typedef char int_8;

#if defined(_WIN32) || defined(_WIN64)
	typedef  __int64 int_64;
	typedef  unsigned __int64 uint_64;
#else // linux
	typedef  long long int_64;
	typedef  unsigned long long uint_64;
	#if defined __x86_64__ //64bit platform 
	# define __WORDSIZE	64
		# define __WORDSIZE_COMPAT32	1
	#else         // 32bit platform
		# define __WORDSIZE	32
	#endif // #if defined __x86_64__
#endif  // #if defined(_WIN32) || defined(_WIN64)

// bzero
#ifndef zeromem
#define zeromem(src,sz ) memset((void *)(src),0,(size_t )( sz))
#endif

#ifndef STATIC_ASSERT
	#define STATIC_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]
#endif


#if !defined(offset_of)
	#if !defined(__cplusplus)
		#define offset_of(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
	#else  // C++
	/* The reference cast is necessary to thwart an operator& that might
	   be applicable to MEMBER type. See DR 273 for details. */
		#define offset_of(TYPE, MEMBER) (reinterpret_cast <size_t> \
			 (&reinterpret_cast <char &>(static_cast <TYPE *> (0)->MEMBER)))
	#endif // C++
#endif

#if !defined(_countof) //
 	#if !defined(__cplusplus)
 		#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))//
 	#else // C++
 		extern "C++"
 		{
 			template <typename _CountofType, size_t _SizeOfArray>
 			char (*__countof_helper2( _CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];
 		#define _countof(_Array) sizeof(*__countof_helper2(_Array))
 		}
 	#endif
#endif



}// namespace z11


#if defined(_WIN32) || defined(_WIN64)
typedef unsigned short wchar_t;
#endif


typedef unsigned long   COLORREF;


template<class Type>
inline void delete_and_set_null(Type *&p)
{
	if (p)
	{
		delete p;
		p=NULL;
	}
}

template<class Type>
inline void release_and_set_null(Type *&p)
{
	if(p)
	{
		p->release();
		p=NULL;
	}
}

//delete array
template<class Type>
inline void delete_array_and_set_null(Type *&p)
{
	if (p)
	{
		delete[] p;
		p=NULL;
	}
}

//
inline int expand_to_2_power(int val)
{
	if (val>0)
	{
		--val;
		int count;
		for (count=1;val>>=1;count++);
		return 1<<count;
	}
	else
		return 0;
}

#define MGPROPERTY(varType, varName, funName)\
protected: varType varName;\
public: inline varType get##funName(void) const { return varName; }\
public: inline void set##funName( varType var){ varName = var; }

#define MGPROPERTY_BOOL(varType, varName, funName)\
protected: varType varName;\
public: inline varType is##funName(void) const { return varName; }\
public: inline void set##funName( varType var){ varName = var; }

#define MGPROPERTY_RETAIN(varType, varName, funName)\
protected: varType varName;\
public: inline varType get##funName(void) const { return varName; }\
public: inline void set##funName( varType var){ \
    if (var != varName) { \
        delete_and_set_null(varName); \
    } \
    varName = var; \
}

#define MGPROPERTY_RETAIN_SUBVIEW(varType, varName, funName)\
protected: varType varName;\
public: inline varType get##funName(void) const { return varName; }\
public: inline void set##funName( varType var){ \
    if (var != varName) { \
        if (varName != NULL) { \
            varName->autoClean(); \
            removeSubview(varName); \
        } \
        varName = var; \
        addSubview(var); \
    } \
}


#define MGPROPERTY_READONLY(varType, varName, funName)\
protected: varType varName;\
public: inline varType get##funName(void) const { return varName; }

#define MGPROPERTY_READONLY_BOOL(varType, varName, funName)\
protected: varType varName;\
public: inline varType is##funName(void) const { return varName; }

#define MG_BREAK_IF(cond)			if(cond) break;

#endif
