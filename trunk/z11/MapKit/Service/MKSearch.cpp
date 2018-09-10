/*
 * =====================================================================================
 *
 *       Filename:  MKCircle.cpp
 *
 *    Description:  The MKCircle class is a concrete overlay object 
 *    representing a circular area on a map.
 *
 *        Version:  1.0
 *        Created:  04/12/2012 02:53:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MKSearch.h"
#include "MapKit/Embedded/Application/Pathfinding.h"
#include "MapKit/Embedded/Network/BindPoint.h"
#include "MapKit/Embedded/Application/LayerMgr.h"
#include "MapKit/Embedded/BasicType/Line.h"
#include "MapKit/Embedded/Tools/SpatialMath.h"
#include "Retiguide/Common/CRS/GlobalCrt.h"
#include "System/Shared/MGLog.h"
#include "System/Shared/Utility.h"
#include "System/IO/MGBundle.h"
#include "MapKit/Fetcher/OsmMap/MKOsmMapFetcher.h"
#include <vector>
using namespace std;
using namespace Embedded;
namespace z11
{


#if !defined (DO_NOT_USE_OLD_MAP)
	double MKSearch::computeShortestRoute( const MKPlanNode&start,const MKPlanNode&end )
	{
		std::deque<MKLocationCoordinate2D> route ;
		return computeShortestRoute(start,end,route);
	}


	double MKSearch::computeShortestRoute( const MKPlanNode&_start,const MKPlanNode&_end,std::deque<MKLocationCoordinate2D>&search_path_points )
	{
		//vector<MKLocationCoordinate2D> search_path_points;  
		Embedded::MapPos2D start, end;		
		GIS_Embedded::GlobalCrt crt;

		MKLocationCoordinate2D start_coord=_start.m_coord;
		MKLocationCoordinate2D end_coord=_end.m_coord;
		{
			int x,y;
			crt.geoToPrj(start_coord.longitude,start_coord.latitude,&x,&y);
			start.d0=x;
			start.d1=y;
			crt.geoToPrj(end_coord.longitude,end_coord.latitude,&x,&y);
			end.d0=x;
			end.d1=y;
		}

		std::vector<Embedded::Line*> lines;
		std::vector<bool> fromBegins; 

		Embedded::Pathfinding* pf = initPathFinding();
		//double dis = pf->computeShortestRoute(start, end, &lines, &fromBegins);
		//double dis = 
		double dis = pf->computeShortestRoute(start,_start.m_course, end,_end.m_course, &lines, &fromBegins, &search_path_points);

		//{
		//	RoadId id=bindPoint(_start);
		//	MGLogD("Start point %s bind point=(%lld)",_start.name.c_str(),id.m_id);
		//	id=bindPoint(_end);
		//	MGLogD("End point %s bind point=(%lld)",_end.name.c_str(),id.m_id);
		//}

		//if(dis < 0)
//		{
//			for (uint_32 j=0; j<lines.size(); j++)
//			{
//				Embedded::Line* line = lines[j];
//				std::vector<Embedded::MapPos2D > points;
//                points.clear();
//				if(line)
//				{
//					line->getPoints(&points);
//				}
//
//				MKLineCoordinate2D _line;
//				for (uint_32 k=0; k<points.size(); k++)
//				{
//					{
//						MKLocationCoordinate2D ll;
//						crt.prjToGeo(points[k].d0,points[k].d1,&ll.longitude,&ll.latitude);
//						_line.push_back((ll));
//					}
//				}
//				search_path_points.push_back(_line);
//				delete_and_set_null(lines[j]);
//			}
//		}
		return dis;
	}

	z11::RoadId MKSearch::bindPoint( const MKPlanNode &_start )
	{
		Embedded::MapPos2D start;
		GIS_Embedded::GlobalCrt crt;

		{
			MKLocationCoordinate2D start_coord=_start.m_coord;
			int x,y;
			crt.geoToPrj(start_coord.longitude,start_coord.latitude,&x,&y);
			start.d0=x;
			start.d1=y;
		}

		
		Embedded::Pathfinding* pf =  initPathFinding();
		Embedded::Network::BindPoint pt;
		if (_start.m_course<-1000)
		{
			pf->bindPoint(start,pt);
		}
		else
			pf->bindPoint(start,_start.m_course,pt);
		RoadId id;
		id.m_id=pt.m_roadID;
		{
			crt.prjToGeo(pt.m_beginPos.d0,pt.m_beginPos.d1,&id.m_begin.longitude,&id.m_begin.latitude);
			crt.prjToGeo(pt.m_endPos.d0,pt.m_endPos.d1,&id.m_end.longitude,&id.m_end.latitude);
            crt.prjToGeo(pt.m_pos.d0, pt.m_pos.d1, &id.m_pos.longitude, &id.m_pos.latitude);
		}
		return id;
	}

	z11::RoadId MKSearch::bindPoint( const MKPlanNode &pt,std::vector<MKLocationCoordinate2D>&route )
	{
		RoadId ret=bindPoint(pt);
		Embedded::Pathfinding* pf =  initPathFinding();
		Line *l=NULL;
		pf->getLineByAddress(ret.m_id,&l);
		std::vector<MapPos2D> points;
		l->getPoints(&points);
		{
			route.clear();
			GIS_Embedded::GlobalCrt crt;
			for (uint_32 i=0;i<points.size();i++)
			{
				MKLocationCoordinate2D pt;
				crt.prjToGeo(points[i].d0,points[i].d1,&pt.longitude,&pt.latitude);
				route.push_back(pt);
			}
		}
		delete_and_set_null(l);
		return ret;
	}

	Embedded::Pathfinding* MKSearch::initPathFinding()
	{

		Embedded::Pathfinding* pf =  Pathfinding::getInstance();
		if(!pf->isInited())
		{
			std::string file_name = MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/data/AdjacentList");
			z11::MGWString path = Utility::ustr2wstr(file_name);
			pf->init(path);
		}
		return pf;
	}

	double MKSearch::getDistance( const MKLocationCoordinate2D &start,const MKLocationCoordinate2D &end )
	{
		int start_x,start_y;
		GIS_Embedded::GlobalCrt crt;
		crt.geoToPrj(start.longitude,start.latitude,&start_x,&start_y);
		int end_x,end_y;
		crt.geoToPrj(end.longitude,end.latitude,&end_x,&end_y);
		return Embedded::SpatialMath::getDistance(MapPos2D(start_x,start_y),MapPos2D(end_x,end_y));
	}



#endif




	RoadId::RoadId() :m_id(0)
	{

	}

}
#if defined (DO_NOT_USE_OLD_MAP)
#include "binaryRoutePlanner.h"
#include "../router/RouteResultPreparation.h"
#include "MapKit/Protocols/MKRouteAnnotation.h"
#include "CommonBase/Utils/StringUtils.h"
using namespace z11;
extern vector<RouteSegmentResult>  runRouteTest(int startx,int starty,int endx,int endy);
vector<MKLocationCoordinate2D> segment;
vector<MKLocationCoordinate2D> getSegmentPoint()
{
	return segment;
}
vector<Ref<MKRouteAnnotation>> testRoute(int startx,int starty,int endx,int endy)
{
	vector<Ref<MKRouteAnnotation>> ret;
	if(!segment.empty())
		return ret;
	MKOsmMapFetcher::getInstance()->loadMapData();
	vector<RouteSegmentResult>  r=runRouteTest(startx, starty, endx, endy);
	for (uint_32 i=0;i<r.size();i++)
	{
		Ref<MKRouteAnnotation> annotation(new MKRouteAnnotation());
		SHARED_PTR<RouteDataObject>  object=r[i].getObject();
		vector<MKLocationCoordinate2D> coords;
		for (uint_32 j=r[i].getStartPointIndex();j<r[i].getEndPointIndex();j++)
		{
			coords.push_back(
				MKLocationCoordinate2D(
				get31LatitudeY(object->pointsY[j]),
				get31LongitudeX(object->pointsX[j])));
		}
		if (coords.empty())
		{
			if (!ret.empty())
			{
				annotation->setCoordinate(ret.back()->coordinate());

			}
		}
		else
		{
			annotation->setPoints(coords);
			annotation->setCoordinate(coords.front());
		}
		annotation->setTitle(r[i].getDescription());
		annotation->setBearingBegin(r[i].getBearingBegin());
		annotation->setBearingEnd(r[i].getBearingEnd());
		MKLocationCoordinate2D coord=annotation->coordinate();
		annotation->setSubtitle(object->getName());
		//annotation->setSubtitle(StringUtils::format("(%.2f,%.2f)",coord.longitude,coord.latitude));
		ret.push_back(annotation);
	}
	return ret;
}
#endif