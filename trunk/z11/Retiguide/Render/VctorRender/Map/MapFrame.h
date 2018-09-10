#ifndef _CORE_RENDERENGINE_MAPFRAME_H
#define _CORE_RENDERENGINE_MAPFRAME_H

#include "../Feature/FeatureLayer.h"
#include "../Annotation/AnnotationLayer.h"
#include "../Cross/CrossLayer.h"
#include "../Feature/FeatureLayerStyle.h"
#include "../Annotation/AnnotationLayerStyle.h"
#include "Retiguide/RenderEngine/GmRender/PolygonStyleRender.h"
#include "Retiguide/RenderEngine/GmRender/PointStyleRender.h"
#include "Retiguide/RenderEngine/GmRender/LineStyleRender.h"
#include "Retiguide/RenderEngine/LabelRender/LabelStyleRender.h"
#include "Retiguide/RenderEngine/GmRender/GmStyleRender.h"
#include "Retiguide/Common/Types/WorkSpaceTypes/WorkSpace.h"
#include "../Render/StyleTranslate.h"
#include "Retiguide/Common/Types/WorkSpaceTypes/WSLayer.h"
#include "Retiguide/Common/Types/WorkSpaceTypes/WSLayerStyle.h"
#include "Retiguide/Common/Share/Scope.h"
#include "Retiguide/DataFetcher/DataFetcher.h"
#include "GraphicCore/MGColor.h"
#include "CommonBase/MGWString.h"
#include <vector>
#include <string>
using namespace std;
using namespace z11;

namespace GIS_Embedded{
class MapFrame {

 private:
 	void initialLayers(WSLayer& ws_layer,int index);
	void initialFeatureLayerStyle(WSLayerStyle& style,FeatureLayer*& layer);
 	void initialAnnotationLayerStyle(WSLayerStyle& style,AnnotationLayer*& layer);
 	void initialPointLayerStyle(WSLayerStyle& style,FeatureLayer*& layer);
 	void initialLineLayerStyle(WSLayerStyle& style,FeatureLayer*& layer);
 	void initialPolygonLayerStyle(WSLayerStyle& style,FeatureLayer*& layer);
 
 	void intitialCrossLayers(vector<WSCrossLayer>& cross_layer);
public:
	FeatureLayer *initialFeatureLayer(WSLayer& ws_layer);
	AnnotationLayer *initialAnnotationLayer(WSLayer& ws_layer);
public:
	MapFrame()
	{
		m_bg_color = z11::RGBAColorMake(190,190,190);
// 		m_map_frame_id = WS("");
		m_cross_bg_color =  z11::RGBAColorMake(100,100,100);
		m_arrow_width = 50;
		m_arrow_color =  z11::RGBAColorMake(0,0,255);
  		DataFetcher *fetcher = DataFetcher::getInstance();
   		fetcher->getFclsGlobalIndex(&m_fcls_global_index);
 	}
 
 	~MapFrame();

 	void clear();
 
 	void initialMapFrame(WorkSpace& work_space);
 
 	void bindMapFrame(MapFrame &map_frame);
 
 
 	int getMapLayersNeedRender(int scale,Scope& view_scope,
 					vector<int>* layer_global_ids,vector<int>* layer_indexs);
 	int getMapLayersNeedRender(int scale,Scope& view_scope,vector<int>& fcls_ids,
 		vector<int>* layer_global_ids,vector<int>* layer_indexs);//designated layers lw
 	int getMapLayers(int scale,Scope& view_scope,vector<int>& fcls_ids,
 		vector<int>* layer_global_ids,vector<int>* layer_indexs);////designated layers in all scales   lw 0324
 
 	bool isFclsIdInMap(vector<int>& fcls_ids,int fcls_id);
 
 
 	const CrossLayerStyle& getCrossLayerStyleByFrc(int frc)const;

	FclsGlobalIndex* getFclsGlobalIndex()
	{
		return m_fcls_global_index;
	}
 
 public:
	 z11::MGWString m_map_frame_id;
 	RGBAColorRef m_bg_color;
	RGBAColorRef m_cross_bg_color;
	RGBAColorRef m_arrow_color;
	vector<FeatureLayer*> m_feature_layers;
 	vector<AnnotationLayer*> m_annotation_layers;
 	int m_arrow_width;
 	vector<CrossLayer*> m_cross_layers;//数组维数�?，分别代表frc�?�?的交叉口道路样式
 
  private:
  	FclsGlobalIndex *m_fcls_global_index;

};
}
#endif