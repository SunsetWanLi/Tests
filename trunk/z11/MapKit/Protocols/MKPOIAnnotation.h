#pragma once
#include "MKAnnotation.h"
namespace z11
{
	class MKPOIAnnotation :
		public MKAnnotation
	{
	public:			
		virtual MKLocationCoordinate2D coordinate();
		virtual void setCoordinate(const MKLocationCoordinate2D&coord);
		MKPOIAnnotation(void);
		~MKPOIAnnotation(void);
		std::string m_image_path;
		MKZoomScale m_maximum_scale;
		MKZoomScale m_minimum_scale;
		std::string m_name;

		virtual std::string title();
	private:
		MKLocationCoordinate2D m_coord;
	};
}

