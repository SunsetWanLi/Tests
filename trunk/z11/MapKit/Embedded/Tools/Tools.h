#ifndef _EMBEDDED_TOOLS_H_
#define _EMBEDDED_TOOLS_H_

#define NOMINMAX
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <string>
#include <sstream>
#include <fstream>
#include <queue>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <list>

#include <cmath>
#include <limits>
#include <climits>
#include <time.h>

#include "../../../CommonBase/CommonDef.h"
#include "../../../CommonBase/MGWString.h"



#include "PointerPool.h"
#include "PoolPointer.h"
#include "SmartPointer.h"

//#ifdef interface
//#undef interface
//#endif
//#define interface class

#ifdef DOUBLE_MAX
#undef DOUBLE_MAX
#endif 

#ifdef DOUBLE_ZERO
#undef DOUBLE_ZERO
#endif

namespace Embedded
{

	
	using z11::int_8;
	using z11::int_16;
	using z11::int_32;
	using z11::int_64;
	using z11::uint_8;
	using z11::uint_16;
	using z11::uint_32;
	using z11::uint_64;


	//typedef uint8_t uint_8;



	const double DOUBLE_MAX			= 1.7976931348623157E+308;
	const uint_32 UINT32_MAX_1		= 0xffffffff;

	const double SPATIAL_MATH_PI	= 3.14159265358979325;

	const double DOUBLE_TOLERANCE	= std::numeric_limits<double>::epsilon() * 100;
	const double PRJ_TOLERANCE		= 0.01;
	const double GEO_TOLERANCE		= 0.0000001;
	const double ANGLE_TOLEANCE		= SPATIAL_MATH_PI / 180 / 100;		// 0.01 degree

	// Current using
	const double DOUBLE_ZERO		= GEO_TOLERANCE;

	enum VariantType
	{
		VT_LONG = 0x0,
		VT_BYTE,
		VT_SHORT,
		VT_FLOAT,
		VT_DOUBLE,
		VT_CHAR,
		VT_USHORT,
		VT_ULONG,
		VT_INT,
		VT_UINT,
		VT_BOOL,
		VT_PCHAR,
		VT_PWCHAR,
		VT_PVOID,
		VT_EMPTY,
		VT_LONGLONG,
		VT_ULONGLONG
	};

	template<class T>
	struct DeletePointerFunctor
	{
		void operator()(const T* pT) const
		{
			delete pT, pT = NULL;
		}
	};

	//
	// Exceptions
	//
	class Exception
	{
	public:
		virtual std::string what() = 0;
		virtual ~Exception() {}
	}; // Exception

	class IndexOutOfBoundsException : public Exception
	{
	public:
		IndexOutOfBoundsException(uint_32 i);
		virtual ~IndexOutOfBoundsException() {}
		virtual std::string what();

	private:
		std::string m_error;
	}; // IndexOutOfBoundsException

	class IllegalArgumentException : public Exception
	{
	public:
		IllegalArgumentException(std::string s);
		virtual ~IllegalArgumentException() {}
		virtual std::string what();

	private:
		std::string m_error;
	}; // IllegalArgumentException

	class IllegalStateException : public Exception
	{
	public:
		IllegalStateException(std::string s);
		virtual ~IllegalStateException() {}
		virtual std::string what();

	private:
		std::string m_error;
	}; // IllegalStateException

	class EndOfStreamException : public Exception
	{
	public:
		EndOfStreamException(std::string s);
		virtual ~EndOfStreamException() {}
		virtual std::string what();

	private:
		std::string m_error;
	}; // EndOfStreamException

	class ResourceLockedException : public Exception
	{
	public:
		ResourceLockedException(std::string s);
		virtual ~ResourceLockedException() {}
		virtual std::string what();

	private:
		std::string m_error;
	}; // ResourceLockedException

	class InvalidPageException : public Exception
	{
	public:
		InvalidPageException(uint_32 id);
		virtual ~InvalidPageException() {}
		virtual std::string what();

	private:
		std::string m_error;
	}; // InvalidPageException

	class NotSupportedException : public Exception
	{
	public:
		NotSupportedException(std::string s);
		virtual ~NotSupportedException() {}
		virtual std::string what();

	private:
		std::string m_error;
	}; // NotSupportedException

	//
	// Interfaces
	//

	class IObject
	{
	public:
		virtual ~IObject () {}
		virtual IObject* clone() = 0;
	}; // IObject

	class ISerializable
	{
	public:
		virtual ~ISerializable() {}
		// returns the size of the required uint_8 array.
		virtual uint_32 getByteArraySize() const = 0;
		// load this object using the uint_8 array.
		virtual void loadFromByteArray(const uint_8* data) = 0;
		// store this object in the uint_8 array.
		virtual void storeToByteArray(uint_8** data, uint_32& length) const = 0;
	};	// ISerializable

	//
	// Classes & Functions
	//

	class Variant
	{
	public:
		Variant();

		VariantType m_varType;

		union
		{
			int_16 iVal;              // VT_SHORT
			int_32 lVal;              // VT_LONG
			int_64 llVal;             // VT_LONGLONG
			uint_8 bVal;                 // VT_BYTE
			float fltVal;              // VT_FLOAT
			double dblVal;             // VT_DOUBLE
			char cVal;                 // VT_CHAR
			uint_16 uiVal;            // VT_USHORT
			uint_32 ulVal;            // VT_ULONG
			uint_64 ullVal;           // VT_ULONGLONG
			bool blVal;                // VT_BOOL
			char* pcVal;               // VT_PCHAR
			wchar_t* pwcVal;           // VT_PWCHAR
			void* pvVal;               // VT_PVOID
		} m_val;
	}; // Variant

	class PropertySet;
	std::ostream& operator<<(std::ostream& os, const PropertySet& p);

	class PropertySet : public ISerializable
	{
	public:
		PropertySet() {}
		PropertySet(const uint_8* data);
		virtual ~PropertySet() {}

		//
		// ISerializable interface
		//
		virtual uint_32 getByteArraySize() const;
		virtual void loadFromByteArray(const uint_8* data);
		virtual void storeToByteArray(uint_8** data, uint_32& length) const;

		Variant getProperty(const std::string& property);
		void setProperty(const std::string& property, Variant& v);
		void removeProperty(const std::string& property);

	private:
		std::map<std::string, Variant> m_propertySet;

	}; // PropertySet

}

#endif // _EMBEDDED_TOOLS_H_