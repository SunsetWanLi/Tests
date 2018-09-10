#include "LayerMgr.h"
#include "../Tools/StringTools.h"
#include "../Features/MapLayer.h"
#include "../BasicType/Line.h"
#include <iostream>
#include "../../../System/Shared/Utility.h"
#include "../../../System/Shared/MGLog.h"


using namespace std;

using namespace Embedded;

LayerMgr* LayerMgr::getInstance()
{
	if(!_instance)
		_instance = new LayerMgr();
	return _instance;
}

LayerMgr::LayerMgr()
	: m_countVisibleLayer(0)
{
	
}

LayerMgr::~LayerMgr()
{
	std::vector<std::pair<Features::MapLayer*, bool> >::iterator itr = m_layerWithVisible.begin();
	for ( ; itr != m_layerWithVisible.end(); ++itr)
		delete itr->first;
}

bool LayerMgr::loadLayer(const z11::MGWString& filePath)
{
 	z11::MGWString layerName = StringTools::substrFromBack(filePath, '/', '.');
    string datafile = z11::Utility::wstr2astr(filePath);
    z11::MGLogD("hell Name:%s",datafile.c_str());
    datafile = z11::Utility::wstr2astr(layerName);
    z11::MGLogD("hell2 Name:%s",datafile.c_str());
	return loadLayer(filePath, layerName);
}

bool LayerMgr::isLayerExist(const z11::MGWString& layerName) const
{
	return m_layersName.find(layerName) != m_layersName.end();
}

void LayerMgr::flipVisible(uint_32 index)
{
	m_layerWithVisible.at(index).second = !m_layerWithVisible.at(index).second;
	if (m_layerWithVisible.at(index).second)
		++m_countVisibleLayer;
	else
		--m_countVisibleLayer;
}

MapScope LayerMgr::getTotDisplayScope() const
{
	MapScope ret;
	for (uint_32 i = 0; i < m_layerWithVisible.size(); ++i)
	{
		if (m_layerWithVisible.at(i).second)
		{
			ISpatialIndex* spatialIndex 
				= m_layerWithVisible.at(i).first->getSpatialIndex();

			LayerRegionQueryStrategy qs;
			spatialIndex->queryStrategy(qs);
			ret.merge(MapScope(qs.m_indexedSpace));
		}
	}

	return ret;
}

Features::MapLayer* LayerMgr::getFirstVisibleMapLayer()
{
	Features::MapLayer* ret = NULL;

	for (uint_32 i = 0; i < m_layerWithVisible.size(); ++i)
	{
		if (m_layerWithVisible.at(i).second)
		{
			ret = m_layerWithVisible.at(i).first;
			break;
		}
	}

	return ret;
}

void LayerMgr::getVisibleLayers(std::vector<Features::MapLayer*>* ret)
{
	std::vector<std::pair<Features::MapLayer*, bool> >::iterator itr = m_layerWithVisible.begin();
	for (; itr != m_layerWithVisible.end(); ++itr)
	{
		if (itr->second)
			ret->push_back(itr->first);
	}
}

void LayerMgr::storeDisplayRegion(const Rect& scope)
{
	std::vector<std::pair<Features::MapLayer*, bool> >::iterator itr = m_layerWithVisible.begin();
	for (; itr != m_layerWithVisible.end(); ++itr)
	{
		if (itr->second)
		{
			itr->first->getSpatialIndex()->setDisplayRegion(scope);
		}	
	}
}

void LayerMgr::queryShapeByRect(const Rect& r, std::vector< IShape* >& shapes, bool considerNonVisible) const
{
	FetchDataVisitor vis(shapes);
	for (uint_32 i = 0; i < m_layerWithVisible.size(); ++i)
		if (considerNonVisible || m_layerWithVisible[i].second)
			m_layerWithVisible[i].first->getSpatialIndex()->intersectsWithQuery(r, vis, false);
}

void LayerMgr::queryShapeByRect(const Rect& r, std::vector< IShape* >& shapes, const z11::MGWString &layerName, bool considerNonVisible /* = false */) const
{
	FetchDataVisitor vis(shapes);
	for(uint_32 i = 0; i < m_layerWithVisible.size(); ++i)
	{
		if (considerNonVisible || m_layerWithVisible[i].second)
		{
			if(layerName.compare(m_layerWithVisible[i].first->getLayerName()) == 0)
			{
				m_layerWithVisible[i].first->getSpatialIndex()->intersectsWithQuery(r, vis, false);
				break;
			}
		}		
	}
}

void LayerMgr::queryShapeByRect(const Rect& r, std::vector<IShape*>& shapes, uint_32 index, bool considerNonVisible) const
{
	FetchDataVisitor vis(shapes);
	assert(index < m_layerWithVisible.size());
	if (considerNonVisible || m_layerWithVisible[index].second)
		m_layerWithVisible[index].first->getSpatialIndex()->intersectsWithQuery(r, vis, false);
}

void LayerMgr::queryLineByRect(const Rect&r, std::vector<Line*>& lines, bool considerNonVisible ) const
{
	vector<IShape*> shapes;
	queryShapeByRect(r, shapes, considerNonVisible);
	lines.clear();
	for(int i = 0; i < shapes.size(); ++i)
	{
		if(Line* l = dynamic_cast<Line*>(shapes[i]))
			lines.push_back(l);
	}

}
uint_32 LayerMgr::getLayerIndex(z11::MGWString &layerName) const
{	
	for(uint_32 i = 0; i < m_layerWithVisible.size(); ++i)
	{
		if(layerName.compare(m_layerWithVisible[i].first->getLayerName()) == 0)
		{
			return i;
		}
	}
	return (uint_32)-1;
}

const z11::MGWString& LayerMgr::getLayerName(uint_32 index) const
{
	assert(index < m_layerWithVisible.size());
	return m_layerWithVisible[index].first->getLayerName();
}

void LayerMgr::getObjectByAddress(uint_32 layerIndex, Embedded::id_type nodeID, uint_32 offset, Embedded::IShape **shape)
{
	assert(layerIndex < m_layerWithVisible.size());
	m_layerWithVisible[layerIndex].first->getObjectByAddress(nodeID, offset, shape);

}

void LayerMgr::getNames(std::vector<z11::MGWString> &names, bool considerNonVisible) const
{
	names.clear();
	names.reserve(m_layerWithVisible.size());
	for(uint_32 i = 0; i < m_layerWithVisible.size(); ++i)
	{
		if (considerNonVisible || m_layerWithVisible[i].second)
		{
			names.push_back(m_layerWithVisible[i].first->getLayerName());
		}
	}
}

//
// class LayerMgr::LayerRegionQueryStrategy
//

void LayerMgr::LayerRegionQueryStrategy::getNextEntry(const IEntry& entry, id_type& nextEntry, bool& hasNext)
{
	hasNext = false;

	IShape* ps;
	entry.getShape(&ps);
	ps->getMBR(m_indexedSpace);
	delete ps;
}

//
//	LayerMgr::FetchDataVisitor
//

void LayerMgr::FetchDataVisitor::visitData(const IData& d, uint_32 dataIndex)
{
	IShape* pS;
	d.getShape(&pS);
	m_shapes.push_back(pS);
}

//
// Internal
//

bool LayerMgr::loadLayer(const z11::MGWString& path, const z11::MGWString& name)
{
	if (!isLayerExist(name))
	{
		m_layerWithVisible.push_back(
			std::make_pair(new Features::MapLayer(path, name), true)
			);
		m_layersName.insert(name);
		++m_countVisibleLayer;
		return true;
	}
	return false;
}

LayerMgr* LayerMgr::_instance = 0;