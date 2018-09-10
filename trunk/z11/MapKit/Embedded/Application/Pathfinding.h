#ifndef _EMBEDDED_APP_PATH_FINDING_H_
#define _EMBEDDED_APP_PATH_FINDING_H_
#include "../SpatialIndex/Utility.h"
#include "../Features/MapLayer.h"
#include "../../DataTypes/Base/MKGeometry.h"
#include "PathResult.h"

namespace Embedded
{
	class Pathfinding
	{
	public:
		static Pathfinding* getInstance();
	public:		
		bool init(const z11::MGWString& topoFilePath);
		bool isInited(){ return m_inited;}
		~Pathfinding();
		double computeShortestRoute(const ScreenPos2D &start, const ScreenPos2D &end, PathResult& path) const;
		double computeShortestRoute(const MapPos2D& start, const MapPos2D& end,
			std::vector<Line*>* route,  std::vector<bool> *fromBegins) const;
		double computeShortestRoute(const MapPos2D& start,double s_course,const MapPos2D& end,double e_course,
                                    std::vector<Line*>* route,  std::vector<bool> *fromBegins, std::deque<z11::MKLocationCoordinate2D> *path_pos) const;
		void getLineByAddress(uint_64 id, Line** l);
		bool bindPoint(const MapPos2D& pt,Network::BindPoint &bind_pt) const;
		bool bindPoint(const MapPos2D& pt,double course,Network::BindPoint &bind_pt) const;

	private:
		Pathfinding(){ m_inited = false;};
		void initLayers(IStorageManager* sm, const z11::MGWString& path);

		void createRoute(const SearchingInfo& info, std::vector<Line*>* route) const;
		void parseRouteContainInLine(uint_64 id, const MapPos2D& s, const MapPos2D& e, Line** l) const;
		void parseRouteEndpoint(uint_64 id, const SearchingInfo::LocationInfo& info, Line** l) const;

	private:
		std::vector<Features::MapLayer*> m_layers;
		std::vector<STATIC_LAYER*> m_indexes;

		IPointBindLine* m_bind;

		IStorageManager* m_nwFile;
		StorageManager::IBuffer* m_nwBuffer;

		IStorageManager* m_highNWFile;
		StorageManager::IBuffer* m_highNWBuffer;

		INetwork* m_network;

		IStorageManager* m_tableFile;
		StorageManager::IBuffer* m_tableBuffer;
		ILookUp* m_table;

		bool m_inited;
	private:
		static Pathfinding* _instance;

	}; // Pathfinding
}

#endif // _EMBEDDED_APP_PATH_FINDING_H_
