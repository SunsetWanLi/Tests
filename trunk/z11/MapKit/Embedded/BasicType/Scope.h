#ifndef _EMBEDDED_SCOPE_H_
#define _EMBEDDED_SCOPE_H_
#include "Rect.h"
#include "../Tools/Tools.h"

namespace Embedded
{
	//template<class T>
	//class Scope
	//{
	//public:
	//	Scope() : m_xMin(DOUBLE_MAX), m_xMax(-DOUBLE_MAX), 
	//		m_yMin(DOUBLE_MAX), m_yMax(-DOUBLE_MAX) {}

	//	Scope(T left, T top, T right, T bottom)
	//		: m_xMin(left), m_xMax(right), m_yMax(top), m_yMin(bottom) {}
	//	Scope(const Rect& r)
	//		: m_xMin(r.m_pLow[0]), m_xMax(r.m_pHigh[0]), m_yMin(r.m_pLow[1]), m_yMax(r.m_pHigh[1]) {}
	//	Scope(const Pos2D<T>& lhs, const Pos2D<T>& rhs)
	//	{
	//		if (lhs.d0 < rhs.d0)
	//		{
	//			m_xMin = lhs.d0;
	//			m_xMax = rhs.d0;
	//		}
	//		else
	//		{
	//			m_xMin = rhs.d0;
	//			m_xMax = lhs.d0;
	//		}

	//		if (lhs.d1 < rhs.d1)
	//		{
	//			m_yMin = lhs.d1;
	//			m_yMax = rhs.d1;
	//		}
	//		else
	//		{
	//			m_yMin = rhs.d1;
	//			m_yMax = lhs.d1;
	//		}
	//	}

	//	const Scope& operator = (const Rect& r) {
	//		m_xMin = r.m_pLow[0]; m_xMax = r.m_pHigh[0];
	//		m_yMin = r.m_pLow[1]; m_yMax = r.m_pHigh[1]; 
	//		return *this;
	//	}

	//	const Scope& operator = (const Scope& s) {
	//		m_xMin = s.m_xMin; m_xMax = s.m_xMax;
	//		m_yMin = s.m_yMin; m_yMax = s.m_yMax; 
	//		return *this;
	//	}

	//	void merge(const Scope<T>& scope) {
	//		m_xMin = std::min(scope.m_xMin, m_xMin);
	//		m_yMin = std::min(scope.m_yMin, m_yMin);
	//		m_xMax = std::max(scope.m_xMax, m_xMax);
	//		m_yMax = std::max(scope.m_yMax, m_yMax);
	//	}

	//	bool isIntersect(const Scope<T>& s) const {
	//		if (m_xMin > s.m_xMax || m_xMax < s.m_xMin ||
	//			m_yMin > s.m_yMax || m_yMax < s.m_yMin)
	//			return false;
	//		return true;
	//	}

	//	double xLength() const { return m_xMax - m_xMin; }
	//	double yLength() const { return m_yMax - m_yMin; }

	//	bool isNull() const { 
	//		return 
	//			m_xMin == DOUBLE_MAX && 
	//			m_xMax == -DOUBLE_MAX && 
	//			m_yMin == DOUBLE_MAX &&
	//			m_yMax == -DOUBLE_MAX;
	//	}
	//	void setNull() {
	//		m_xMin = m_yMin = std::numeric_limits<T>::max();
	//		m_xMax = m_yMax = std::numeric_limits<T>::min();
	//	}

	//public:
	//	T m_xMin;
	//	T m_xMax;
	//	T m_yMin;
	//	T m_yMax;
	//}; // Scope

	//typedef Scope<long> ScreenScope;
	//typedef Scope<double> MapScope;

	
	class ScreenScope
	{
	public:
		ScreenScope() : m_xMin(0x7fffffff), m_xMax(-0x7fffffff), 
			m_yMin(0x7fffffff), m_yMax(-0x7fffffff) {}

		ScreenScope(long left, long top, long right, long bottom)
			: m_xMin(left), m_xMax(right), m_yMax(top), m_yMin(bottom) {}
		ScreenScope(const Rect& r)
			: m_xMin((long)r.m_pLow[0]), m_xMax((long)r.m_pHigh[0]), m_yMin((long)r.m_pLow[1]), m_yMax((long)r.m_pHigh[1]) {}
		ScreenScope(const Pos2DEx& lhs, const Pos2DEx& rhs)
		{
			if (lhs.d0 < rhs.d0)
			{
				m_xMin = lhs.d0;
				m_xMax = rhs.d0;
			}
			else
			{
				m_xMin = rhs.d0;
				m_xMax = lhs.d0;
			}

			if (lhs.d1 < rhs.d1)
			{
				m_yMin = lhs.d1;
				m_yMax = rhs.d1;
			}
			else
			{
				m_yMin = rhs.d1;
				m_yMax = lhs.d1;
			}
		}

		const ScreenScope& operator = (const Rect& r) {
			m_xMin = (long)r.m_pLow[0]; m_xMax = (long)r.m_pHigh[0];
			m_yMin = (long)r.m_pLow[1]; m_yMax = (long)r.m_pHigh[1]; 
			return *this;
		}

		const ScreenScope& operator = (const ScreenScope& s) {
			m_xMin = s.m_xMin; m_xMax = s.m_xMax;
			m_yMin = s.m_yMin; m_yMax = s.m_yMax; 
			return *this;
		}

		void merge(const ScreenScope& scope) {
			m_xMin = std::min<long>(scope.m_xMin, m_xMin);
			m_yMin = std::min<long>(scope.m_yMin, m_yMin);
			m_xMax = std::max<long>(scope.m_xMax, m_xMax);
			m_yMax = std::max<long>(scope.m_yMax, m_yMax);
		}

		bool isIntersect(const ScreenScope& s) const {
			if (m_xMin > s.m_xMax || m_xMax < s.m_xMin ||
				m_yMin > s.m_yMax || m_yMax < s.m_yMin)
				return false;
			return true;
		}

		double xLength() const { return m_xMax - m_xMin; }
		double yLength() const { return m_yMax - m_yMin; }

		bool isNull() const { 
			return 
				m_xMin == DOUBLE_MAX && 
				m_xMax == -DOUBLE_MAX && 
				m_yMin == DOUBLE_MAX &&
				m_yMax == -DOUBLE_MAX;
		}
		void setNull() {
			m_xMin = m_yMin = 0x7fffffff;
			m_xMax = m_yMax = -0x7fffffff;
		}

	public:
		long m_xMin;
		long m_xMax;
		long m_yMin;
		long m_yMax;
	}; // ScreenScope

	
	class MapScope
	{
	public:
		MapScope() : m_xMin(DOUBLE_MAX), m_xMax(-DOUBLE_MAX), 
			m_yMin(DOUBLE_MAX), m_yMax(-DOUBLE_MAX) {}

		MapScope(double left, double top, double right, double bottom)
			: m_xMin(left), m_xMax(right), m_yMax(top), m_yMin(bottom) {}
		MapScope(const Rect& r)
			: m_xMin(r.m_pLow[0]), m_xMax(r.m_pHigh[0]), m_yMin(r.m_pLow[1]), m_yMax(r.m_pHigh[1]) {}
		MapScope(const Pos2D& lhs, const Pos2D& rhs)
		{
			if (lhs.d0 < rhs.d0)
			{
				m_xMin = lhs.d0;
				m_xMax = rhs.d0;
			}
			else
			{
				m_xMin = rhs.d0;
				m_xMax = lhs.d0;
			}

			if (lhs.d1 < rhs.d1)
			{
				m_yMin = lhs.d1;
				m_yMax = rhs.d1;
			}
			else
			{
				m_yMin = rhs.d1;
				m_yMax = lhs.d1;
			}
		}

		const MapScope& operator = (const Rect& r) {
			m_xMin = r.m_pLow[0]; m_xMax = r.m_pHigh[0];
			m_yMin = r.m_pLow[1]; m_yMax = r.m_pHigh[1]; 
			return *this;
		}

		const MapScope& operator = (const MapScope& s) {
			m_xMin = s.m_xMin; m_xMax = s.m_xMax;
			m_yMin = s.m_yMin; m_yMax = s.m_yMax; 
			return *this;
		}

		void merge(const MapScope& scope) {
			m_xMin = std::min<double>(scope.m_xMin, m_xMin);
			m_yMin = std::min<double>(scope.m_yMin, m_yMin);
			m_xMax = std::max<double>(scope.m_xMax, m_xMax);
			m_yMax = std::max<double>(scope.m_yMax, m_yMax);
		}

		bool isIntersect(const MapScope& s) const {
			if (m_xMin > s.m_xMax || m_xMax < s.m_xMin ||
				m_yMin > s.m_yMax || m_yMax < s.m_yMin)
				return false;
			return true;
		}

		double xLength() const { return m_xMax - m_xMin; }
		double yLength() const { return m_yMax - m_yMin; }

		bool isNull() const { 
			return 
				m_xMin == DOUBLE_MAX && 
				m_xMax == -DOUBLE_MAX && 
				m_yMin == DOUBLE_MAX &&
				m_yMax == -DOUBLE_MAX;
		}
		void setNull() {
			m_xMin = m_yMin = DOUBLE_MAX;
			m_xMax = m_yMax = -DOUBLE_MAX;
		}

	public:
		double m_xMin;
		double m_xMax;
		double m_yMin;
		double m_yMax;
	}; // MapScope
}

#endif // _EMBEDDED_SCOPE_H_