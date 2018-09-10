#ifndef _EMBEDDED_POLYGON_H_
#define _EMBEDDED_POLYGON_H_
#include "../Tools/SpatialDataBase.h"
#include "Attribute.h"

namespace Embedded
{
	class Rect;

	class Polygon : public IObject, public virtual IShape
	{
	public:
		Polygon();
		Polygon(uint_32 posNum, const double* pCoords);
		Polygon(const Polygon& l);
		virtual ~Polygon();

		Polygon& operator=(const Polygon& l);
		bool operator==(const Polygon& l) const;

		//
		// IObject interface
		//
		virtual Polygon* clone();

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
		virtual const z11::MGWString& getName() const { return m_attr.getName();}

		////double getMinimumDistance(const Point& p) const;
		////double getCoordinate(uint_32 index) const;
		void getPoints(std::vector<MapPos2D>* points) const;
		void getPoints(std::vector<ScreenPos2D> *points) const;

		void initialize(const uint_32 ringNum, const uint_32* posNum, const double* pCoords);

		uint_32 posNum() const;
		///const z11::MGWString& getName() const{ return m_attr.getName(); }

	private:
		//void initialize(const uint_32* posNum, const double* pCoords); 
		//uint_32 getTotPosNum(uint_32 ringNum, uint_32* posNum) const;
		//void checkRoom(uint_32 ringNum, uint_32* posNum);
		
	public:
		static const int DIMENSION = 2;
		uint_32 m_posNum;
		double* m_pCoords;
		Attribute m_attr;

	}; // Polygon

}

#endif // _EMBEDDED_POLYGON_H_

