#ifndef _EMBEDDED_LINE_H_
#define _EMBEDDED_LINE_H_
#include "../Tools/SpatialDataBase.h"
#include "BasicGMType.h"
#include "Attribute.h"

namespace Embedded
{
	class Rect;

	class Line : public IObject, public virtual IShape
	{
	public:
		Line(uint_32 attrAddr);
		Line(const double* pCoords, uint_32 size,uint_32 attrAddr);
		Line(const std::vector<MapPos2D>& line, uint_32 attrAddr);
		Line(const Line& l);
		virtual ~Line();

		Line& operator=(const Line& l);
		bool operator==(const Line& l) const;

		//
		// IObject interface
		//
		virtual Line* clone();

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
		virtual const z11::MGWString& getName() const; 

		void initialize(uint_32 posNum, const double* pCoords);
		void getBothEndsPoint(MapPos2D* s, MapPos2D* e) const;
		void getPoint(uint_32 index, MapPos2D* point) const;
		void getPoints(std::vector<MapPos2D>* points) const;
		void getPoints(std::vector<ScreenPos2D> *points) const;
		uint_32 posNum() const { return m_posNum; }
		void moveControlPoi(uint_32 pointIndex, const MapPos2D& newPos);
		uint_32 getAttrAddress() const { return m_attrAddr;}
		double getLength() const;
		
		//static void setAttributeSource(IStorageManager *source);

	private:
		void initialize(const double* pCoords); 
		void initialize(const std::vector<MapPos2D>& line);
		void initialize(); 
		void checkRoom(uint_32 size);
		void getAttribute();

	public:
		static const int DIMENSION = 2;
		uint_32 m_posNum;
		double* m_pCoords;
		friend class Rect;
	public:	
		static const uint_32 NOTNEEDATTR	= 0xffffffff; //attribute is not needed
		static const uint_32 NULLATTRADDRESS = 0xfffffffe; //attribute is null	
		static const uint_32 MAXVALIDATTRADDRESS = 0xfffffff0;
	private:
		uint_32 m_attrAddr;
		Attribute* m_attr;
		static IStorageManager *_attrSource;
	}; // Line

}

#endif // _EMBEDDED_LINE_H_

