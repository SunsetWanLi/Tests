#pragma once
#include "../basic_types.h"
#include "CommonBase/MGWString.h"
namespace GIS_Embedded{
	class GuiderGeoPoint
	{
		enum GeoTypeT{
			GT_UNKNOWN,
			GT_TOLIET,
			GT_SHOP
		};
	private:		
		Pos2DEx m_pos;
		GeoTypeT m_GType;
	protected:
		static unsigned int total_languages ;
		static unsigned int index_languages ;
	public:
		static void setTotalLanguages(unsigned int total);
		static void setIndexLanguages(unsigned int index);
	public:
		virtual bool hasTriggerArea() const = 0;
		virtual const z11::MGWString getName() const = 0;
		virtual void setKey(const z11::MGWString &key) = 0;
	public:
         GuiderGeoPoint()
		 {
			 m_pos = Pos2DEx(-1,-1);
			 m_GType = GT_UNKNOWN;
		 }
		virtual ~GuiderGeoPoint(){}
		GeoTypeT getMyGeoType() const;;
		void setGeoType(GeoTypeT type);
		
		const Pos2DEx getMyPosition() const;
		void setMyPosition(int x,int y);


	};
}