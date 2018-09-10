#ifndef _EMBEDDED_BASIC_GM_TYPE_H_
#define _EMBEDDED_BASIC_GM_TYPE_H_
#include "../../../Retiguide/Common/Types/basic_types.h"

namespace Embedded
{
	//////////////////////////////////////////////////////////////////////////
	// Pos2D
	//////////////////////////////////////////////////////////////////////////

	//template<class T>
	//class Pos2D
	//{
	//public:
	//	T d0, d1;

	//public:
	//	Pos2D() {}
	//	Pos2D(T x, T y) : d0(x), d1(y) {}
	//	Pos2D(const Pos2D &right) : d0(right.d0), d1(right.d1) {}

	//	bool operator == (const Pos2D &right) const {
	//		return d0 == right.d0 && d1 == right.d1;
	//	}

	//	bool operator < (const Pos2D &right) const {
	//		return d0 < right.d0
	//			|| d0 == right.d0 && d1 > right.d1;
	//	}

	//	bool operator != (const Pos2D &right) const {
	//		return !(*this == right);
	//	}

	//	Pos2D& operator = (const Pos2D &right) {
	//		d0 = right.d0; d1 = right.d1 ;
	//		return *this;
	//	}
	//};	// Pos2D

	//typedef Pos2D<long> ScreenPos2D;
	//typedef Pos2D<double> MapPos2D;

	typedef GIS_Embedded::Pos2DEx ScreenPos2D;
	typedef GIS_Embedded::Pos2DEx Pos2DEx;
	typedef GIS_Embedded::Pos2D	  Pos2D;
	typedef GIS_Embedded::Pos2D   MapPos2D;

	//////////////////////////////////////////////////////////////////////////
	// Pos3D
	//////////////////////////////////////////////////////////////////////////

	template<class T>
	class Pos3D
	{
	public:
		T d0, d1, d2;

	public:
		Pos3D() {}
		Pos3D(T x, T y, T z) : d0(x), d1(y), d2(z) {}
		Pos3D(const Pos3D& right) : d0(right.d0), d1(right.d1), d2(right.d2) {}

		bool operator == (const Pos3D &right) const {
			return d0 == right.d0 && d1 == right.d1 && d2 == right.d2;
		}

		bool operator != (const Pos3D &right) const {
			return !(*this == right);
		}

		Pos3D& operator = (const Pos3D &right) {
			d0 = right.d0; d1 = right.d1; d2 = right.d2;
			return *this;
		}
	};	// Pos3D

	typedef Pos3D<long> ScreenPos3D;
	typedef Pos3D<double> MapPos3D;

}
#endif // _EMBEDDED_BASIC_GM_TYPE_H_
