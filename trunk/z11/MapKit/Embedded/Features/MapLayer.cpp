#include "MapLayer.h"
#include "../Tools/StringTools.h"
#include "../SpatialIndex/StaticRTree.h"
#include "../Network/AdjacencyList.h"
#include "../BasicType/Line.h"
#include "../../../System/Shared/Utility.h"
#include "../../../System/Shared/MGLog.h"

using namespace Embedded;
using namespace Embedded::Features;

MapLayer::MapLayer(const z11::MGWString& filePath, const z11::MGWString layerName)
	: m_filePath(filePath), m_layerName(layerName)
{
	loadSpatialIndex();
}

MapLayer::~MapLayer()
{
	delete m_spatialIndex;
	delete m_buffer;
	delete m_file;
}

ISpatialIndex* MapLayer::getSpatialIndex()
{
	return m_spatialIndex;
}

void MapLayer::getLineByAddress(const std::vector<STATIC_LAYER*>* layers, uint_64 id, Line** l)
{
	uint_32 layerID;
	id_type nodeID;
	uint_32 offset;
	Network::AdjacencyList::decodeTopoEdgeID(id, &layerID, &nodeID, &offset);

	IShape* s;
	assert(layerID < layers->size());
	layers->at(layerID)->readDataByAddress(nodeID, offset, &s);

	*l = dynamic_cast<Line*>(s);
}

void MapLayer::loadSpatialIndex()
{
	z11::MGWString type = StringTools::substrFromBack(m_filePath, '_', '.');
	if (type == WS(L"STATIC"))
    {
		loadStaticRTree(false);
    }
	else if(type == WS(L"STATICM"))
    {
		loadStaticRTree(true);
    }
	else
    {
		throw IllegalArgumentException(
		"MapLayer::loadSpatialIndex: Error SpatialIndex Type. "
		);
    }
}

void MapLayer::loadStaticRTree(bool bCrossPage)
{
	z11::MGWString idxPath = m_filePath.substr(0, m_filePath.find_last_of('.'));
    string datafile = z11::Utility::wstr2astr(idxPath);
    z11::MGLogD("idx Name:%s",datafile.c_str());
    datafile = z11::Utility::wstr2astr(m_filePath);
    z11::MGLogD("mfike Name:%s",datafile.c_str());
    
    
	if(!bCrossPage)
		m_file = StorageManager::loadStaticDiskStorageManager(idxPath);
	else
		m_file = StorageManager::loadDiskStorageManager(idxPath);
	m_buffer = StorageManager::createNewFIFOBuffer(*m_file, 100, false);
	m_spatialIndex = SpatialIndex::loadStaticRTree(*m_buffer, 0);
}

void MapLayer::getObjectByAddress(Embedded::id_type nodeID, uint_32 offset, Embedded::IShape **shape)
{
	STATIC_LAYER* rTree = dynamic_cast<STATIC_LAYER*>(m_spatialIndex);
	if(rTree)
		rTree->readDataByAddress(nodeID, offset, shape);
} 