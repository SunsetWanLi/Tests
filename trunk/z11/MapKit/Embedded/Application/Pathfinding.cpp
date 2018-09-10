#include "../Network/Utility.h"
#include "Pathfinding.h"
#include "../SpatialIndex/StaticRTree.h"
#include "../SpatialIndex/PointBindLine.h"
#include "../Network/LowNetwork.h"
#include "../Network/HighNetwork.h"
#include "../Network/QuadtreeTable.h"
#include "../../../Retiguide/Common/CRS/GlobalCrt.h"

#include "../BasicType/Line.h"
#include "AttributeMgr.h"
#include "../../../System/Shared/Utility.h"
#include "../../../System/Shared/MGLog.h"

using namespace Embedded;

Pathfinding* Pathfinding::getInstance()
{
	if(!_instance)
		_instance = new Pathfinding();
	return _instance;
}


bool Pathfinding::init(const z11::MGWString& topoFilePath)
{
	if(m_inited)
		return m_inited;

	z11::MGWString dirPath = topoFilePath.substr(0, topoFilePath.find_last_of((L'/') ));

	m_nwFile = StorageManager::loadStaticDiskStorageManager(topoFilePath);
	m_nwBuffer = StorageManager::createNewFIFOBuffer(*m_nwFile, 100, false);

	z11::MGWString highNWPath = dirPath + WS(L"/MultiLayersNW");
	m_highNWFile = StorageManager::loadStaticDiskStorageManager(highNWPath);
	m_highNWBuffer = StorageManager::createNewFIFOBuffer(*m_highNWFile, 100, false);

	z11::MGWString tablePath = dirPath + WS(L"/LookUpTable");
	m_tableFile = StorageManager::loadStaticDiskStorageManager(tablePath);
	m_tableBuffer = StorageManager::createNewFIFOBuffer(*m_tableFile, 100, false);

	m_table = new Network::QuadTreeTable(*m_tableBuffer, PUBLISH_PAGE_SIZE);
    
    string datafile = z11::Utility::wstr2astr(dirPath);
    z11::MGLogD("dir Name:%s",datafile.c_str());
    
	initLayers(m_nwBuffer, dirPath);

	const ICondenseStrategy* cs = dynamic_cast<STATIC_LAYER*>(m_indexes.at(0))->getCondenseStrategy();
	std::vector<ISpatialIndex*> indexes;
	copy(m_indexes.begin(), m_indexes.end(), back_inserter(indexes));
	m_bind = new SpatialIndex::PointBindLine(indexes, cs);

	//m_network = new Network::LowNetwork(m_nwBuffer, m_bind, m_table, true);

	//Network::LowNetwork* lowNW = new Network::LowNetwork(m_nwBuffer, m_bind, NULL, false);
	//m_network = new Network::HighNetwork(m_highNWBuffer, lowNW, m_bind, NULL, false);
	Network::LowNetwork* lowNW = new Network::LowNetwork(m_nwBuffer, m_bind, m_table, true);
	m_network = new Network::HighNetwork(m_highNWBuffer, lowNW, m_bind, m_table, true);

	AttributeMgr::getInstance()->loadAttributes(dirPath);

	m_inited = true;
	return m_inited;
}

Pathfinding::~Pathfinding()
{
	delete m_network;
	delete m_bind;
	for_each(m_layers.begin(), m_layers.end(), DeletePointerFunctor<Features::MapLayer>());

	delete m_table;
	delete m_tableBuffer;
	delete m_tableFile;

	delete m_highNWBuffer;
	delete m_highNWFile;

	delete m_nwBuffer;
	delete m_nwFile;
}

double Pathfinding::computeShortestRoute(const ScreenPos2D &start, const ScreenPos2D &end, PathResult& path) const
{
	MapPos2D start1(start.d0, start.d1), end1(end.d0, end.d0);
	std::vector<Line*> lines;
	std::vector<bool> fromBegins;
	double dis = computeShortestRoute(start1, end1, &lines, &fromBegins);
	path.setLines( lines, fromBegins);
	for(uint_32 i = 0; i < lines.size(); ++i)
	{
		delete lines.at(i);
	}

	return dis;

}


double Pathfinding::computeShortestRoute(const MapPos2D& start, const MapPos2D& end,
									   std::vector<Line*>* route, std::vector<bool> *fromBegins) const
{
	SearchingInfo info(start, end);
	double dis = m_network->computeShortestPath(info);
	createRoute(info, route);

	MapPos2D beginPos, endPos;
	MapPos2D lastPos = start;
	fromBegins->resize(route->size());
	for(uint_32 i = 0; i < route->size(); ++i)
	{
		Line* curLine = route->at(i);
		curLine->getPoint(0, &beginPos);
		curLine->getPoint(curLine->posNum()-1, &endPos);
		double disBegin = SpatialMath::getDistance(lastPos, beginPos);
		double disEnd = SpatialMath::getDistance(lastPos, endPos);
		if( disBegin <=  disEnd)
		{
			lastPos = endPos;
			(*fromBegins)[i] = true;
		}
		else
		{
			lastPos = beginPos;
			(*fromBegins)[i] = false;
		}

		if(i == route->size()-1)
		{
			disBegin = SpatialMath::getDistance(end, beginPos);
			disEnd = SpatialMath::getDistance(end, endPos);
			if((*fromBegins)[i])
				assert(disBegin >= disEnd );
			else 
				assert(disBegin <= disEnd);
		}

	}	
	
	return dis;
}

void Pathfinding::getLineByAddress(uint_64 id, Line** l)
{
	Features::MapLayer::getLineByAddress(&m_indexes, id, l);
}

void Pathfinding::initLayers(IStorageManager* sm, const z11::MGWString& path)
{
	std::vector<z11::MGWString> layerNames;
	Network::fetchLayersName(sm, layerNames);
	uint_32 layerNum = layerNames.size();
	assert(layerNum);

	m_layers.clear(); m_indexes.clear();
	m_layers.reserve(layerNum);	m_indexes.reserve(layerNum);
	for (uint_32 i = 0; i < layerNum; ++i)
	{
        string datafile = z11::Utility::wstr2astr(layerNames[i]);
        z11::MGLogD("File Name:%s",datafile.c_str());
		Features::MapLayer* ml 
			= new Features::MapLayer(path + WS(L"/") + layerNames[i] + WS(L".dat"), layerNames[i]);
		m_layers.push_back(ml);
		m_indexes.push_back(dynamic_cast<STATIC_LAYER*>(ml->getSpatialIndex()));
	}
}

void Pathfinding::createRoute(const SearchingInfo& info, std::vector<Line*>* route) const
{
	if (info.m_roadIDs.empty())
		return;

	if (info.m_roadIDs.size() == 1)
	{
		Line* line;
		parseRouteContainInLine(info.m_roadIDs.front(), info.m_start.m_bindPos, info.m_end.m_bindPos, &line);
		route->push_back(line);
	}
	else
	{
        Line* line;

        parseRouteEndpoint(info.m_roadIDs.front(), info.m_start, &line);
        route->push_back(line);
        for (uint_32 i = 1; i < info.m_roadIDs.size() - 1; ++i)
        {
            Features::MapLayer::getLineByAddress(&m_indexes, info.m_roadIDs[i], &line);
            route->push_back(line);
        }
        parseRouteEndpoint(info.m_roadIDs.back(), info.m_end, &line);
        route->push_back(line);
	}
}

void Pathfinding::parseRouteContainInLine(uint_64 id, const MapPos2D& s, const MapPos2D& e, Line** l) const
{
	Line* original;
	Features::MapLayer::getLineByAddress(&m_indexes, id, &original);
	std::vector<MapPos2D> ctlpos;
	original->getPoints(&ctlpos);

	double sPos = SpatialMath::getPointPositionOnLine(s, ctlpos),
		ePos = SpatialMath::getPointPositionOnLine(e, ctlpos);
	assert(ePos != -1 && ePos != -1);

	std::vector<MapPos2D> newLine;
	if (sPos < ePos)
		SpatialMath::getPartCtlposOfLine(sPos, ePos, ctlpos, &newLine);
	else
	{
		SpatialMath::getPartCtlposOfLine(ePos, sPos, ctlpos, &newLine);
		reverse(newLine.begin(), newLine.end());
	}

	*l = new Line(newLine,original->getAttrAddress());
	delete original;
	
}

void Pathfinding::parseRouteEndpoint(uint_64 id, const SearchingInfo::LocationInfo& info, Line** l) const
{
	Line* original;
	Features::MapLayer::getLineByAddress(&m_indexes, id, &original);
	std::vector<MapPos2D> ctlpos;
	original->getPoints(&ctlpos);

	double index = SpatialMath::getPointPositionOnLine(info.m_bindPos, ctlpos);
	std::vector<MapPos2D> newLine;
	if (info.m_bDir)
		SpatialMath::getPartCtlposOfLine(0, index, ctlpos, &newLine);
	else
		SpatialMath::getPartCtlposOfLine(index, ctlpos.size() - 1, ctlpos, &newLine);

	*l = new Line(newLine, original->getAttrAddress());
	delete original;
	
}

Pathfinding* Pathfinding::_instance = 0;

bool Embedded::Pathfinding::bindPoint( const MapPos2D& pt,Network::BindPoint &bind_pt ) const
{
	return m_network->bindPoint(pt,bind_pt);
}

bool Embedded::Pathfinding::bindPoint( const MapPos2D& pt,double course,Network::BindPoint &bind_pt ) const
{
	return m_network->bindPoint(pt,course,bind_pt);

}
double Embedded::Pathfinding::computeShortestRoute( const MapPos2D& start,double s_course,const MapPos2D& end,double e_course, std::vector<Line*>* route, std::vector<bool> *fromBegins, std::deque<z11::MKLocationCoordinate2D> *path_pos) const
{
	SearchingInfo info(start,s_course, end,e_course);
	double dis = m_network->computeShortestPath(info);
	createRoute(info, route);
    
//    std::cout<<"route size is : "<<route->size()<<std::endl;
//    for(uint_32 i = 0; i < route->size(); ++i)
//    {
//        std::cout<<route->at(i)->getLength()<<std::endl;
//    }
    
    GIS_Embedded::GlobalCrt crt;

	MapPos2D beginPos, endPos, nextBeginPos, nextEndPos;
	fromBegins->resize(route->size());
	for(uint_32 i = 0; i < route->size(); ++i)
	{
        if (route->size() == 1) {
            Line* curLine = route->at(i);
            std::vector<MapPos2D> points;
            curLine->getPoints(&points);
            bool found_start = false;
            bool found_end = false;
            for (auto it = points.begin(); it != points.end(); ++it) {
                if (info.m_start.m_bindPos == *it) {
                    found_start = true;
                }
                if (info.m_end.m_bindPos == *it) {
                    found_end = true;
                }
                if (found_end && !found_start) {
                    (*fromBegins)[i] = false;
                    z11::MKLocationCoordinate2D ll;
                    crt.prjToGeo(it->d0,it->d1,&ll.longitude,&ll.latitude);
                    path_pos->push_front(ll);
                }
                if (found_start && !found_end) {
                    (*fromBegins)[i] = true;
                    z11::MKLocationCoordinate2D ll;
                    crt.prjToGeo(it->d0,it->d1,&ll.longitude,&ll.latitude);
                    path_pos->push_back(ll);
                }
                if (found_start && found_end) {
                    if ((*fromBegins)[i] == true) {
                        z11::MKLocationCoordinate2D ll;
                        crt.prjToGeo(it->d0,it->d1,&ll.longitude,&ll.latitude);
                        path_pos->push_back(ll);
                    } else {
                        z11::MKLocationCoordinate2D ll;
                        crt.prjToGeo(it->d0,it->d1,&ll.longitude,&ll.latitude);
                        path_pos->push_front(ll);
                    }
                    break;
                }
            }
        } else {
            Line* curLine = route->at(i);
            Line* nextLine = NULL;
            if (i < route->size() - 1) {
                nextLine = route->at(i+1);
                curLine->getPoint(0, &beginPos);
                curLine->getPoint(curLine->posNum()-1, &endPos);
                if (nextLine != NULL) {
                    nextLine->getPoint(0, &nextBeginPos);
                    nextLine->getPoint(nextLine->posNum()-1, &nextEndPos);
                } else {
                    continue;
                }
                double eedis = SpatialMath::getDistance(nextEndPos, endPos);
                double bbdis = SpatialMath::getDistance(nextBeginPos, beginPos);
                double ebdis = SpatialMath::getDistance(nextEndPos, beginPos);
                double bedis = SpatialMath::getDistance(nextBeginPos, endPos);
                double mindis = eedis < bbdis ? eedis : bbdis;
                mindis = bbdis < ebdis ? bbdis : ebdis;
                mindis = ebdis < bedis ? ebdis : bedis;
                if (mindis == bbdis) {
                    (*fromBegins)[i] = false;
                    (*fromBegins)[i+1] = true;
                } else if (mindis == bedis) {
                    (*fromBegins)[i] = true;
                    (*fromBegins)[i+1] = true;
                } else if (mindis == ebdis) {
                    (*fromBegins)[i] = false;
                    (*fromBegins)[i+1] = false;
                } else if (mindis == eedis) {
                    (*fromBegins)[i] = true;
                    (*fromBegins)[i+1] = false;
                }
            }
            std::vector<MapPos2D> curLinePoints;
            curLine->getPoints(&curLinePoints);
            if (i == 0) {
                bool found_start = false;
                if ((*fromBegins)[i] == true) {
                    for (auto it = curLinePoints.begin(); it != curLinePoints.end(); ++it) {
                        if (*it == info.m_start.m_bindPos) {
                            found_start = true;
                        }
                        if (found_start) {
                            z11::MKLocationCoordinate2D ll;
                            crt.prjToGeo(it->d0,it->d1,&ll.longitude,&ll.latitude);
                            path_pos->push_back(ll);
                        }
                    }
                } else {
                    for (auto it = curLinePoints.rbegin(); it != curLinePoints.rend(); ++it) {
                        if (*it == info.m_start.m_bindPos) {
                            found_start = true;
                        }
                        if (found_start) {
                            z11::MKLocationCoordinate2D ll;
                            crt.prjToGeo(it->d0,it->d1,&ll.longitude,&ll.latitude);
                            path_pos->push_back(ll);
                        }
                    }
                }
            } else if (i == route->size() - 1) {
                if ((*fromBegins)[i] == true) {
                    for (auto it = curLinePoints.begin(); it != curLinePoints.end(); ++it) {
                        z11::MKLocationCoordinate2D ll;
                        crt.prjToGeo(it->d0,it->d1,&ll.longitude,&ll.latitude);
                        path_pos->push_back(ll);
                        if (*it == info.m_end.m_bindPos) {
                            break;
                        }
                    }
                } else {
                    for (auto it = curLinePoints.rbegin(); it != curLinePoints.rend(); ++it) {
                        z11::MKLocationCoordinate2D ll;
                        crt.prjToGeo(it->d0,it->d1,&ll.longitude,&ll.latitude);
                        path_pos->push_back(ll);
                        if (*it == info.m_end.m_bindPos) {
                            break;
                        }
                    }
                }
            } else {
                if ((*fromBegins)[i] == true) {
                    for (auto it = curLinePoints.begin(); it != curLinePoints.end(); ++it) {
                        z11::MKLocationCoordinate2D ll;
                        crt.prjToGeo(it->d0,it->d1,&ll.longitude,&ll.latitude);
                        path_pos->push_back(ll);
                    }
                } else {
                    for (auto it = curLinePoints.rbegin(); it != curLinePoints.rend(); ++it) {
                        z11::MKLocationCoordinate2D ll;
                        crt.prjToGeo(it->d0,it->d1,&ll.longitude,&ll.latitude);
                        path_pos->push_back(ll);
                    }
                }
            }
        }
        delete_and_set_null(route->at(i));
	}
//    std::cout<<"original caculate distance is : "<<dis<<std::endl;
	return dis;
}