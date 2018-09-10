#include  "KMLParser.h"
#include "GraphicCore/GraphicStructure.h"
#include "MapKit/Views/OverlayViews/MKOverlayPathView.h"
#include "MapKit/DataTypes/Shapes/MKPointAnnotation.h"
#include "MapKit/DataTypes/Shapes/MKPolygon.h"
#include "MapKit/DataTypes/Shapes/MKShape.h"
#include "MapKit/DataTypes/Shapes/MKPolyline.h"
#include "MapKit/Views/AnnotationViews/MKAnnotationView.h"
#include "MapKit/Views/AnnotationViews/MKPinAnnotationView.h"
#include "MapKit/Views/OverlayViews/MKPolygonView.h"
#include "MapKit/Views/OverlayViews/MKPolylineView.h"
#include "System/Shared/MGLog.h"
#include <stdio.h>
using namespace std;
namespace z11
{
	// KMLElement and subclasses declared here implement a class hierarchy for
	// storing a KML document structure.  The actual KML file is parsed with a SAX
	// parser and only the relevant document structure is retained in the object
	// graph produced by the parser.  Data parsed is also transformed into
	// appropriate UIKit and MapKit classes as necessary.

	// Abstract KMLElement type.  Handles storing an element identifier (id="...")
	// as well as a buffer for accumulating character data parsed from the xml.
	// In general, subclasses should have beginElement and endElement classes for
	// keeping track of parsing state.  The parser will call beginElement when
	// an interesting element is encountered, then all character data found in the
	// element will be stored into accum, and then when endElement is called accum
	// will be parsed according to the conventions for that particular element type
	// in order to save the data from the element.  Finally, clearString will be
	// called to reset the character data accumulator.

	class KMLElement : MGObject {
	public:
		string identifier;
		string accum;
		KMLElement *initWithIdentifier(const string& ident)
		{
			identifier=(ident);
			return this;
		}

		// Returns YES if we're currently parsing an element that has character
		// data contents that we are interested in saving.
		virtual bool canAddString()
		{
			return false;
		}
		// Add character data parsed from the xml
		virtual void addString(const string &str)
		{
			if (canAddString())
			{
				accum+=(str);
			}
			
		}
		// Once the character data for an element has been parsed, use clearString to
		// reset the character buffer to get ready to parse another element.
		void clearString()
		{
			accum.clear();
		}
		KMLElement()
		{

		}
	private:
		KMLElement(const string &ident):identifier(ident)
		{
		}
	};

	RGBAColorRef colorWithKMLString(const string &accum)
	{
		assert(0);
		return RGBAColorRef::red();
	}
	float floatWithKMLString(const string &accum)
	{
		float ret=0.0f;
		sscanf(accum.c_str(),"%f",&ret);
		return ret;
	}
	bool boolWithKMLString(const string&accum)
	{
		bool ret=false;
		assert(0);
		return ret;
		//sscanf
	}
	// Represents a KML <Style> element.  <Style> elements may either be specified
	// at the top level of the KML document with identifiers or they may be
	// specified anonymously within a Geometry element.
	class KMLStyle :public KMLElement {    
	public:
		RGBAColorRef  strokeColor;
		float strokeWidth;
		RGBAColorRef fillColor;

		bool fill;
		bool stroke;

		struct { 
			int inLineStyle:1;
			int inPolyStyle:1;

			int inColor:1;
			int inWidth:1;
			int inFill:1;
			int inOutline:1;
		} flags;


		void beginLineStyle()
		{
			flags.inLineStyle=1;
		}

		void endLineStyle()
		{
			flags.inLineStyle=0;
		}


		void beginPolyStyle()
		{
			flags.inPolyStyle=1;
		}
		void endPolyStyle()
		{
			flags.inPolyStyle=0;
		}

		void beginColor()
		{
			flags.inColor=1;
		}
		void endColor()
		{
			flags.inColor=0;
			if (flags.inLineStyle)
			{
				strokeColor = colorWithKMLString(accum);
			}
			else if(flags.inPolyStyle)
			{
				fillColor = colorWithKMLString(accum);
			}

			clearString();
		}

		void beginWidth()
		{
			flags.inWidth=1;
		}
		void endWidth()
		{
			flags.inWidth=0;
			strokeWidth = floatWithKMLString(accum);
			clearString();
		}

		void beginFill()
		{
			flags.inFill=1;
		}
		void endFill()
		{
			flags.inFill=0;
			fill=boolWithKMLString(accum);
			clearString();
		}

		void beginOutline()
		{
			flags.inOutline=1;
		}
		void endOutline()
		{
			flags.inOutline=0;
			stroke=boolWithKMLString(accum);
			clearString();
		}

		void applyToOverlayPathView(MKOverlayPathView *view)
		{
			view->setStrokeColor(strokeColor);
			view->setFillColor(fillColor);
			view->setLineWidth(strokeWidth);
		}
	};



	class KMLGeometry : public KMLElement {
	public:
		struct {
			int inCoords:1;
		} flags;

		virtual bool canAddString()
		{
			return flags.inCoords;
		}

		virtual void beginCoordinates()
		{
			flags.inCoords=1;
		}
		virtual void endCoordinates()
		 {
			 flags.inCoords=0;
		 }

		// Create (if necessary) and return the corresponding Map Kit MKShape object
		// corresponding to this KML Geometry node.
		virtual MKShape * mapkitShape()
		{
			return NULL;
		}

		 // Create (if necessary) and return the corresponding MKOverlayPathView for
		 // the MKShape object.
		 virtual MKOverlayPathView *createOverlayView(MKShape *shape)
		 {
			 return NULL;
		 }

	};

	void strToCoords(const std::string &accum,vector<MKLocationCoordinate2D > &points)
	{
		assert(0);
	}
	// A KMLPoint element corresponds to an MKAnnotation and MKPinAnnotationView
	class KMLPoint :public KMLGeometry {
	public:
		MKLocationCoordinate2D point;
		virtual void endCoordinates()
		{
			vector<MKLocationCoordinate2D >points;
			KMLGeometry::endCoordinates();
			strToCoords(accum,points);
			if (points.size()==1)
			{
				point=points[0];
			}
			clearString();
			
		}
		virtual MKShape* mapkitShape()
		{
			MKPointAnnotation *annotation=new MKPointAnnotation();
			annotation->setCoordinate(point) ;
			return annotation;
		}
	};


	// A KMLPolygon element corresponds to an MKPolygon and MKPolygonView
	class KMLPolygon :public  KMLGeometry {
		string outerRing;
		vector<string> innerRings;

		struct {
			int inOuterBoundary:1;
			int inInnerBoundary:1;
			int inLinearRing:1;
		} polyFlags;


		void beginOuterBoundary()
		{
			polyFlags.inOuterBoundary=1;
		}
		void endOuterBoundary()
		{
			polyFlags.inOuterBoundary=0;
			outerRing=accum;
			clearString();
		}

		void beginInnerBoundary()
		{
			polyFlags.inInnerBoundary=1;
		}
		void endInnerBoundary()
		{
			polyFlags.inInnerBoundary=0;
			innerRings.push_back(accum);
			clearString();
		}

		void beginLinearRing()
		{
			polyFlags.inLinearRing=1;
		}
		void endLinearRing()
		{
			polyFlags.inLinearRing=0;
		}

		virtual MKShape *mapkitShape()
		{
			// KMLPolygon corresponds to MKPolygon

			// The inner and outer rings of the polygon are stored as kml coordinate
			// list strings until we're asked for mapkitShape.  Only once we're here
			// do we lazily transform them into CLLocationCoordinate2D arrays.

			// First build up a list of MKPolygon cutouts for the interior rings.
			vector<MKLocationCoordinate2D> innerPolys;
			vector<MKLocationCoordinate2D> coords;
			for (uint_32 i=0;i<innerRings.size();i++)
			{
				strToCoords(accum,coords);
				innerPolys.insert(innerPolys.end(),coords.begin(),coords.end());
				coords.clear();
			}
			

			// Now parse the outer ring.
			strToCoords(outerRing, coords);

			// Build a polygon using both the outer coordinates and the list (if applicable)
			// of interior polygons parsed.
			//MKPolygon *poly = [MKPolygon polygonWithCoordinates:coords count:coordsLen interiorPolygons:innerPolys];
			assert(0);
			MKPolygon *poly =MKPolygon::polygonWithCoordinates(coords);
			return poly;
		}

		virtual MKOverlayPathView *createOverlayView(MKShape *shape)
		{
			MKPolygon *poly=dynamic_cast<MKPolygon *>(shape);
			if (poly!=NULL)
			{
				MKPolygonView *polyView=MKPolygonView::initWithPolygon(poly);
				return polyView;
			}
			return NULL;
		}
	};

	class KMLLineString : public KMLGeometry {
	public:
		vector<MKLocationCoordinate2D> points;
		virtual void endCoordinates()
		{
			flags.inCoords=0;
			points.clear();
			strToCoords(accum,points);
			clearString();
		}

		virtual MKShape*mapkitShape()
		{
			return MKPolyline::polylineWithCoordinates(points);
		}
	};

	class KMLPlacemark : KMLElement {
		KMLStyle* style;
		KMLGeometry* geometry;

		string name;
		string placemarkDescription;

		string styleUrl;

		MKShape *mkShape;

		MKAnnotationView *m_annotationView;
		MKOverlayPathView *m_overlayView;

		struct {
			int inName:1;
			int inDescription:1;
			int inStyle:1;
			int inGeometry:1;
			int inStyleUrl:1;
		} flags;

		virtual bool canAddString()
		{
			return flags.inName || flags.inStyleUrl || flags.inDescription;
		}
		virtual void addString(const std::string &str)
		{
			if (flags.inStyle)
			{
				style->addString(str);
			}
			else if(flags.inGeometry)
			{
				geometry->addString(str);
			}
			else
			{
				KMLElement::addString(str);
			}
			
		}
		void beginName()
		{
			flags.inName=1;
		}
	
		void endName()
		{
			flags.inName=0;
			name=accum;
			clearString();
		}

		void beginDescription()
		{
			flags.inDescription=1;
		}
		void endDescription()
		{
			flags.inDescription=0;
			placemarkDescription=accum;
			clearString();
		}

		void beginStyleUrl()
		{
			flags.inStyleUrl=1;
		}
		void endStyleUrl()
		{
			flags.inStyleUrl=0;
			styleUrl=accum;
			clearString();
		}



		void beginStyleWithIdentifier(const string &ident)
		{
			flags.inStyle=1;
			style = new KMLStyle();
			style->initWithIdentifier(ident);
		}
		void endStyle()
		{
			flags.inStyle=0;
		}

#define ELTYPE(typeName) (elementName == "#typeName")
		void beginGeometryOfTypeWithIdentifier(const string &elementName ,const string &ident)
		{
			flags.inGeometry=1;
			if (ELTYPE(Point))
			{
				geometry= new KMLPoint();
				geometry->initWithIdentifier(ident);
			}
			else if (ELTYPE(Polygon))
			{
				geometry=new KMLPolygon();
				geometry->initWithIdentifier(ident);
			}
			else if(ELTYPE(LineString))
			{
				geometry=new KMLLineString();
				geometry->initWithIdentifier(ident);
			}
		}
		void endGeometry()
		{
			flags.inGeometry=0;
			
			
		}
		//KMLElement
		KMLPolygon *polygon()
		{
			KMLPolygon *ret=dynamic_cast<KMLPolygon *>(geometry);
			return ret;
		}
		void _createShape()
		{
			if (!mkShape)
			{
				mkShape=geometry->mapkitShape();
				mkShape->setTitle(name);
			}
			
		}

		MKOverlay *overlay()
		{
			_createShape();
			MKOverlay *ret=dynamic_cast<MKOverlay *>(mkShape);
			return ret;
		}
		MKAnnotation *point()
		{
			_createShape();
			MKPointAnnotation *ret=dynamic_cast<MKPointAnnotation *>(mkShape);
			return ret;
			
		}
		MKOverlayView *overlayView()
		{
			if (!m_overlayView)
			{
				MKOverlay *_overlay=overlay();
				if (_overlay)
				{
					assert(0);
					m_overlayView = geometry->createOverlayView(mkShape);
					style->applyToOverlayPathView(m_overlayView);
				}
				
			}
			return m_overlayView;
			
		}
		MKAnnotationView *annotationView()
		{
			if (!m_annotationView)
			{
				MKAnnotation* _annotaion = point();
				if (_annotaion)
				{
					MKPinAnnotationView *pin=MKPinAnnotationView::initWithAnnotation(_annotaion);
					pin->canShowCallout=(true);
					pin->animatesDrop=(true);
					m_annotationView=pin;
				}
				
			}
			return m_annotationView;
			
		}
	};

	KMLParser * z11::KMLParser::initWithContentsOfFile( const std::string& path )
	{
		KMLParser *ret=new KMLParser();
		return ret;
	}

	void z11::KMLParser::parseKML()
	{

	}

	std::vector<MKOverlay *>  z11::KMLParser::overlays()
	{
		return m_overlays;
	}

	std::vector<MKAnnotation *>  z11::KMLParser::points()
	{
		return m_points;
	}

	MKAnnotationView * z11::KMLParser::viewForAnnotation( const MKAnnotation* point )
	{
		return NULL;
	}

	MKOverlayView * z11::KMLParser::viewForOverlay( MKOverlay *overlay )
	{
		return NULL;
	}

	z11::KMLParser::KMLParser()
	{
		_placemark=NULL;
		_style=NULL;
	}

}