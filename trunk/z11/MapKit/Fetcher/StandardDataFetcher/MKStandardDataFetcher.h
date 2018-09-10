/*
 * =====================================================================================
 *
 *       Filename:  MKStandardDataFetcher.h
 *
 *    Description:  This  Fetch data from  local standard map data
 *
 *        Version:  1.0
 *        Created:  04/13/2012 09:32:00 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#include "MapKit/DataTypes/Base/MKGeometry.h"
#include "Retiguide/Render/VctorRender/Map/MapFrame.h"
#include "MapKit/Embedded/Application/LayerMgr.h"
#include "Retiguide/Render/VctorRender/Render/StyleRender.h"
#include "Retiguide/Render/VctorRender/Map/ScreenPixMap.h"
#include "Retiguide/Common/Util/Transformer.h"
#include "Retiguide/Render/VctorRender/Map/MapTerrCtrl.h"
#include "Retiguide/Render/VctorRender/Feature/FeatureLayerRender.h"
#include "MapKit/DataTypes/Base/MKTypes.h"
#include "MapKit/Protocols/MKPOIAnnotation.h"
namespace z11
{
	class MGCanvas;
	//should only be called in one thread
	class MKStandardDataFetcher:public Syncable
	{
	public:
		static MKStandardDataFetcher* getInstance();
	public:
		void init(int width,int height);
		void getLayerDataInScope(const MKMapRect& view_scope);
		void adjustScope(const MKMapRect& _view_scope);
		void clearData();
		void render(MGCanvas *canvas);
		std::string getCacheFolderPath(const MKGlobalTileIndex &index,MKMapType type);
		MKMapRect getViewScope(const MKGlobalTileIndex &index);
		MKMapRect m_view_scope;
		GIS_Embedded::Scope m_map_scope;
		vector<int> m_scale_ranks;
		int m_scale_rank;
		
		MGImage *getImage(const MKGlobalTileIndex &index,MKMapType type);
		vector<MKPOIAnnotation *> getPOIAnnotations();
		void renderTrafficRoad(MGCanvas *canvas);
	private:
		MKStandardDataFetcher();
		static MKStandardDataFetcher *fetcher;
		MGSize m_screen_size;
		GIS_Embedded::ScreenPixMap m_screen_pix_map;
		GIS_Embedded::FeatureLayerRender m_feature_layer_render;
		GIS_Embedded::Transformer m_transformer;
		GIS_Embedded::StyleRender m_style_render;
		GIS_Embedded::MapTerrCtrl *m_map_terr_ctrl;
		GIS_Embedded::MapFrame m_map_frame;

		vector<pair<std::string, vector<std::string> > > m_datasrc_nodes;
		Embedded::LayerMgr *m_layerMgr;
		vector<std::string> m_fcls_names;
		std::vector<std::vector<Embedded::IShape*> > m_shapes;
		vector<int> m_layer_mapframe_indexs;
		vector<int> m_layer_global_ids;
		MGCanvas *m_canvas;

		GIS_Embedded::FeatureLayer* m_traffic_feature_layer;
		GIS_Embedded::AnnotationLayer *m_traffic_annotation_layer;
	};//
};
