#ifndef _EMBEDDED_RECT_H_
#define _EMBEDDED_RECT_H_
#include "../Tools/SpatialDataBase.h"
#include "BasicGMType.h"

namespace Embedded
{
	class Line;

	class Rect : public IObject, public virtual IShape
	{
	public:
		Rect();
		Rect(const double* pLow, const double* pHigh);
		Rect(const Point& low, const Point& high);
		Rect(const Point& center, const double& edge);
		Rect(const Rect& rect);
		Rect(const MapPos2D& leftLow, const MapPos2D& rightTop);
		virtual ~Rect();

		Rect& operator=(const Rect& rect);
		bool operator==(const Rect& rect) const;

		bool isEmpty() const { return m_pLow == NULL || m_pHigh == NULL; }

		//
		// IObject interface
		//
		virtual Rect* clone();

		//
		// ISerializable interface
		//
		virtual uint_32 getByteArraySize() const;
		virtual void loadFromByteArray(const uint_8* data);
		virtual void storeToByteArray(uint_8** data, uint_32& length) const;

		//
		// IShape interface
		//
		virtual bool intersectsShape(const IShape& in) const;
		virtual bool containsShape(const IShape& in) const;
		virtual bool touchesShape(const IShape& in) const;
		virtual void getCenter(Point& out) const;
		virtual void getMBR(Rect& out) const;
		virtual double getArea() const;
		virtual double getMinimumDistance(const IShape& in) const;
		virtual const z11::MGWString& getName() const{ static z11::MGWString nullName(WS(L"")); return nullName;};

		bool intersectsRect(const Rect& in) const;
		bool intersectsLine(const Line& in) const;
		bool containsRect(const Rect& in) const;
		bool containsLine(const Line& in) const;
		bool touchesRect(const Rect& in) const; 
		double getMinimumDistance(const Rect& in) const;

		bool containsPoint(const Point& in) const;
		bool containsPoint(const MapPos2D& in) const;
		bool touchesPoint(const Point& in) const;
		bool touchesPoint(const MapPos2D& in) const;
		double getMinimumDistance(const Point& in) const;

		Rect getIntersectingRect(const Rect& in) const;
		double getIntersectingArea(const Rect& in) const;
		double getMargin() const;

		void combineRect(const Rect& in);
		void combinePoint(const Point& in);
		void combinePoint(const MapPos2D& in);
		void getCombinedRect(Rect& out, const Rect& in) const;

		void makeInfinite();

		void initialize(const double* pLow, const double* pHigh);
		void initialize(const MapPos2D& leftLow, const MapPos2D& rightTop);

		void changeSize(const double& ratio);
		void changeSize(const double& xChange, const double& yChange);

		double xLength() const { return m_pHigh[0] - m_pLow[0]; }
		double yLength() const { return m_pHigh[1] - m_pLow[1]; }

		MapPos2D leftLow() const { return MapPos2D(m_pLow[0], m_pLow[1]); }
		MapPos2D rightTop() const  { return MapPos2D(m_pHigh[0], m_pHigh[1]); }

		MapPos2D center() const {
			return MapPos2D(0.5 * (m_pLow[0] + m_pHigh[0]), 0.5 * (m_pLow[1] + m_pHigh[1]));
		};

	private:
		void initialize();

	public:
		static const int DIMENSION = 2;
		double* m_pLow;
		double* m_pHigh;
	}; // Rect
}

#endif // _EMBEDDED_RECT_H_