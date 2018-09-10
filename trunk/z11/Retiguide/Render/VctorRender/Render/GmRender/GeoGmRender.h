#ifndef _CORE_RENDERENGINE_GEOGMRENDER_H
#define _CORE_RENDERENGINE_GEOGMRENDER_H

//#include "../../../../Common/Types/GeoTypes/GeographyObject.h"
#include "Retiguide/Common/Util/Transformer.h"
#include "../StyleRender.h"
#include "../LabelRender/GeoLabelPosTrans.h"

#include "MapKit/Embedded/Tools/SpatialDataBase.h"


using namespace Embedded;

namespace GIS_Embedded
{

	class GeoGmRender
	{

	public:

		GeoGmRender(){}

		virtual ~GeoGmRender(){}

		void setTansformer(const Transformer *transformer)
		{
			m_transformer = transformer;
		}
		
		void setScopeAdjust(const Scope*& scope)
		{
			m_scope_adjust = scope;
		}

		//for new data
		virtual void setGeo(Embedded::IShape* geo)
		{
			m_geo = geo;
// 			m_geo_label = &const_cast<(geo->getName())>;
		}

		void resizeGmBuff(int size)
		{
			if(size > m_gm_buff_size)
			{
				delete m_gm_buff;
				m_gm_buff = new z11::MGPoint[size];
				m_gm_buff_size = size;
			}
		}

		virtual int render(StyleRender& style_render) = 0;

	protected:
		const Transformer* m_transformer;
		const Scope* m_scope_adjust;

		//for new data
		IShape* m_geo;

	public:
		static MGPoint* m_gm_buff;
		static int m_gm_buff_size;
		static int m_buff_valid;

	protected:
		MGPoint m_label_point;
		int m_label_escape;


	};

}

#endif