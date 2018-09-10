#ifndef _EMBEDDED_POINT_H_
#define _EMBEDDED_POINT_H_
#include "../Tools/SpatialDataBase.h"
#include "Attribute.h"

namespace Embedded
{
	class Rect;
	class Line;

	class Point : public IObject, public virtual IShape
	{
	public:
		Point();
		Point(const MapPos2D& p);
		Point(const double* pCoords);
		Point(const Point& p);
		virtual ~Point();

		Point& operator=(const Point& p);
		bool operator==(const Point& p) const;

		//
		// IObject interface
		//
		virtual Point* clone();

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
		virtual const z11::MGWString& getName() const{ return m_attr.getName(); }

		double getMinimumDistance(const Point& in) const;
		double getMinimumDistance(const Line& in) const;
		double getCoordinate(uint_32 index) const;
		void getPoint(MapPos2D *pos) const;
		void getPoint(ScreenPos2D *pos) const;

		void initialize(const double* pCoords);
		void initialize();
		//const z11::MGWString& getName() const{ return m_attr.getName(); }

	private:
		bool isEmpty() const { return m_pCoords == NULL; }

	public:
		static const int DIMENSION = 2;
		double* m_pCoords;
		Attribute m_attr;

		friend class Rect;
	}; // Point

	const uint_32 POINT_DATA_SIZE = sizeof(double) * Point::DIMENSION;
}

#endif // _EMBEDDED_POINT_H_

