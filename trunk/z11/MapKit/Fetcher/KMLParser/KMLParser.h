#pragma once
#include <vector>
#include "CommonBase/MGDictionary.h"
#include "MapKit/DataTypes/Base/MKGeometry.h"
#include "MapKit/Protocols/MKAnnotation.h"
#include "MapKit/Protocols/MKOverlay.h"
namespace z11
{
	class KMLPlacemark;
	class KMLStyle;
	class MKAnnotationView;
	class MKOverlayView;
	class KMLParser {
	public:
		MGDictionary* _styles;
		std::vector<KMLPlacemark*> _placemarks;

		KMLPlacemark *_placemark;
		KMLStyle *_style;

		static KMLParser *initWithContentsOfFile(const std::string& path);
		void parseKML();

		std::vector<MKOverlay *> overlays();
		
		std::vector<MKAnnotation *> points();

		MKAnnotationView *viewForAnnotation(const MKAnnotation* point);
		MKOverlayView *viewForOverlay(MKOverlay *overlay);
	private:
		std::vector<MKOverlay *> m_overlays;
		std::vector<MKAnnotation *> m_points;
		KMLParser();
		//    NSXMLParser *_xmlParser;
	};
}
