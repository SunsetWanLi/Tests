// basic_types.h
// zzj, Jan. 13
// define the basic data types such as integers


#ifndef __RASTERIMAGE_BASIC_TYPES_H__
#define __RASTERIMAGE_BASIC_TYPES_H__

//#ifndef WIN32
//#define WIN32
//#endif
 
namespace Raster_GIS
{
/*
#ifdef WIN32

#define int_32 int
#define uint_32 unsigned int
#define int_64 __int64
#define uint_64 unsigned __int64
#define int_16 short
#define uint_16 unsigned short
#define bool_32 int

#endif	//WIN32
*/
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


}//namespace Raster_GIS

#endif //IS_GIS_BASIC_TYPES_H_

