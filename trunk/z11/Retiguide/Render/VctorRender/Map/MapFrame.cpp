#include "MapFrame.h"
#include "Retiguide/Common/Types/basic_types.h"

//#include "../../../../include/MGFoundation.h"

namespace GIS_Embedded
{
	MapFrame::~MapFrame()
	{
		for(vector<FeatureLayer*>::iterator i = m_feature_layers.begin(); i!= m_feature_layers.end(); i ++)
			delete *i;
		for(vector<AnnotationLayer*>::iterator i = m_annotation_layers.begin(); i!= m_annotation_layers.end(); i++)
			delete *i;

		for(vector<CrossLayer*>::iterator i = m_cross_layers.begin(); i!= m_cross_layers.end(); i++)
			delete *i;
	}

	void MapFrame::initialMapFrame(WorkSpace& work_space)
	{
		m_map_frame_id = work_space.m_workspace_id;
 		m_bg_color = z11::RGBAColorRef::fromRGB(work_space.m_bg_color);

		int sum = work_space.m_ws_layers.size();
		m_feature_layers.resize(sum);
		m_annotation_layers.resize(sum);

		//////////////////////////////////////////////////////////////////////////
		//work_space.m_ws_layers[3].LAYER_STYLE.LINE_STYLE[0].second.FACE_COLOR=16777215;
		//work_space.m_ws_layers[3].LAYER_STYLE.LINE_STYLE[1].second.FACE_COLOR=16777215;
		//MGLogD("Crack ShaoXing chengzhengdaolu style by GRF ");
		//////////////////////////////////////////////////////////////////////////

		for(int i=0; i < sum; i++)
			initialLayers(work_space.m_ws_layers[i],i);


		intitialCrossLayers(work_space.m_cross_type.m_cross_layers);

	}

	void MapFrame::initialLayers(WSLayer& ws_layer,int index)
	{
		int globe_index = m_fcls_global_index->calculateIndex(ws_layer.LAYER_ID);//0;

		m_feature_layers[index] = initialFeatureLayer(ws_layer);
		m_feature_layers[index]->m_layer_global_index = globe_index;

		m_annotation_layers[index] = initialAnnotationLayer(ws_layer);
		m_annotation_layers[index]->m_layer_global_index = globe_index;
	}

	void MapFrame::initialFeatureLayerStyle(WSLayerStyle& style,FeatureLayer*& layer)
	{
		switch(layer->m_layer_type)
		{
		case FeatureLayer::t_POINT:
			initialPointLayerStyle(style,layer);
			break;
		case FeatureLayer::t_LINE:
			initialLineLayerStyle(style,layer);
			break;
		case FeatureLayer::t_POLYGON:
			initialPolygonLayerStyle(style,layer);
			break;
		case FeatureLayer::t_IMAGE:
			break;
		}
	}

	void MapFrame::initialPointLayerStyle(WSLayerStyle& style,FeatureLayer*& layer)
	{
		int sum = style.POINT_STYLE.size();
		layer->m_feature_layer_style.m_ceiline_scale_styles.resize(sum);
		for(int i=0;i<sum;i++)
		{
			pair<int,GmStyleRender>& layer_style = layer->m_feature_layer_style.m_ceiline_scale_styles[i];
			pair<int,WSPointStyle>& ws_style = style.POINT_STYLE[i];
			layer_style.first = ws_style.first;
			StyleTranslate::transPointStyle(ws_style.second,&layer_style.second.m_point_render);
		}
	}

	void MapFrame::initialLineLayerStyle(WSLayerStyle& style,FeatureLayer*& layer)
	{
		int sum = style.LINE_STYLE.size();
		layer->m_feature_layer_style.m_ceiline_scale_styles.resize(sum);
		for(int i=0;i<sum;i++)
		{
			pair<int,GmStyleRender>& layer_style = layer->m_feature_layer_style.m_ceiline_scale_styles[i];
			pair<int,WSLineStyle>& ws_style = style.LINE_STYLE[i];
			layer_style.first = ws_style.first;
			StyleTranslate::transLineStyle(ws_style.second,&layer_style.second.m_line_render);
		}
	}

	void MapFrame::initialPolygonLayerStyle(WSLayerStyle& style,FeatureLayer*& layer)
	{
		int sum = style.POLYGON_STYLE.size();
		layer->m_feature_layer_style.m_ceiline_scale_styles.resize(sum);
		for(int i=0;i<sum;i++)
		{
			pair<int,GmStyleRender>& layer_style = layer->m_feature_layer_style.m_ceiline_scale_styles[i];
			pair<int,WSPolygonStyle>& ws_style = style.POLYGON_STYLE[i];
			layer_style.first = ws_style.first;
			StyleTranslate::transPolygonStyle(ws_style.second,&layer_style.second.m_polygon_render);
		}
	}

	void MapFrame::initialAnnotationLayerStyle(WSLayerStyle& style,AnnotationLayer*& layer)
	{
		int sum = style.LABEL_STYLE.size();
		layer->m_annogation_layer_style.m_ceiline_scale_styles.resize(sum);
		for(int i=0;i<sum;i++)
		{
			pair<int,LabelStyleRender>& layer_style = layer->m_annogation_layer_style.m_ceiline_scale_styles[i];
			pair<int,WSLabelStyle>& ws_style = style.LABEL_STYLE[i];
			layer_style.first = ws_style.first;
			StyleTranslate::transLabelStyle(ws_style.second,&layer_style.second);
		}
	}

	void MapFrame::intitialCrossLayers(vector<WSCrossLayer>& cross_layer)
	{
		//m_cross_layers.resize(10);

		int i;
		int sum=cross_layer.size();
		int frc = 0;
		for (i=0;i<sum;i++)
		{
			if (cross_layer[i].FRC > frc)
				frc = cross_layer[i].FRC;
		}

		m_cross_layers.resize(frc+1);
		for(i=0;i<=frc;i++)
			m_cross_layers[i] = new CrossLayer();

		for(i=0,sum=cross_layer.size();i<sum;i++)
		{
			int frc = cross_layer[i].FRC;
			m_cross_layers[frc]->m_layer_frc = frc;
			m_cross_layers[frc]->m_layer_style.m_grade = cross_layer[i].GRADE;
			m_cross_layers[frc]->m_layer_style.m_width = cross_layer[i].WIDTH;
		}
	}

	void MapFrame::bindMapFrame(MapFrame &map_frame)
	{
		if(m_map_frame_id.compare(map_frame.m_map_frame_id) == 0)
			return;

		m_feature_layers.insert(m_feature_layers.end(),map_frame.m_feature_layers.begin(),map_frame.m_feature_layers.end());
		m_annotation_layers.insert(m_annotation_layers.end(),map_frame.m_annotation_layers.begin(),map_frame.m_annotation_layers.end());

		m_cross_layers.clear();
		m_cross_layers.insert(m_cross_layers.end(),map_frame.m_cross_layers.begin(),map_frame.m_cross_layers.end());
	}

	int MapFrame::getMapLayersNeedRender(int scale,Scope& view_scope,
					vector<int>* layer_global_ids,vector<int>* layer_indexs)
	{
		layer_global_ids->clear();
		layer_indexs->clear();


		int i,sum = m_feature_layers.size();
		//z11::MGLogD("=== MapFrame::getMapLayersNeedRender() sum = %d ===", sum);
		for(i=0;i<sum;i++)
		{
			if(m_feature_layers[i]->m_need_render)
			{
				//获取一个图层的整体scope，求交
// 				Scope* extension = m_map_tool_box->getFeatureLayerExtension(m_feature_layers[i]->m_layer_global_index);
// 				if(!extension->isIntersect(view_scope))
// 					continue;

				layer_indexs->push_back(i);
				layer_global_ids->push_back(m_feature_layers[i]->m_layer_global_index);

				m_feature_layers[i]->initStyleByScale(scale);

				//GRF
				//m_annotation_layers[i]->clearGeoLabelPos();
				if(m_annotation_layers[i]->m_need_render)
					m_annotation_layers[i]->initStyleByScale(scale);
			}

			
		}

		return 0;
	}
	int MapFrame::getMapLayersNeedRender(int scale,Scope& view_scope,vector<int>& fcls_ids,
		vector<int>* layer_global_ids,vector<int>* layer_indexs)
	{
		layer_global_ids->clear();
		layer_indexs->clear();

		int i,sum = m_feature_layers.size();
		for(i=0;i<sum;i++)
		{
			if(m_feature_layers[i]->m_need_render)
			{
// 				Scope* extension = m_map_tool_box->getFeatureLayerExtension(m_feature_layers[i]->m_layer_global_index);
// 				if(!extension->isIntersect(view_scope))
// 					continue;

				int fcls_id = m_feature_layers[i]->m_layer_global_index;
				if (isFclsIdInMap(fcls_ids,fcls_id))
				{		
					layer_indexs->push_back(i);
					layer_global_ids->push_back(fcls_id);

					m_feature_layers[i]->initStyleByScale(scale);

					m_annotation_layers[i]->clearGeoLabelPos();
					if(m_annotation_layers[i]->m_need_render)
						m_annotation_layers[i]->initStyleByScale(scale);
				}
			}
		}
		return 0;
	}
	int MapFrame::getMapLayers(int scale,Scope& view_scope,vector<int>& fcls_ids,
		vector<int>* layer_global_ids,vector<int>* layer_indexs)
	{
		layer_global_ids->clear();
		layer_indexs->clear();

		int i,sum = m_feature_layers.size();
		for(i=0;i<sum;i++)
		{
// 			Scope* extension = m_map_tool_box->getFeatureLayerExtension(m_feature_layers[i]->m_layer_global_index);
// 			if(!extension->isIntersect(view_scope))
// 				continue;

			int fcls_id = m_feature_layers[i]->m_layer_global_index;
			if (isFclsIdInMap(fcls_ids,fcls_id))
			{		
				layer_indexs->push_back(i);
				layer_global_ids->push_back(fcls_id);

 				m_feature_layers[i]->initStyleByScale(scale);
 
				//GRF 12-3-23
 				//m_annotation_layers[i]->clearGeoLabelPos();
 				if(m_annotation_layers[i]->m_need_render)
 					m_annotation_layers[i]->initStyleByScale(scale);
			}
		}
		return 0;
	}

	bool MapFrame::isFclsIdInMap(vector<int>& fcls_ids,int fcls_id)
	{
		if(fcls_ids.empty())
			return false;
		for (uint_32 i=0,len = fcls_ids.size(); i < len; i++)
		{
			if (fcls_ids[i] == fcls_id)
				return true;
		}
		return false;
	}
	const CrossLayerStyle& MapFrame::getCrossLayerStyleByFrc(int frc)const
	{
		assert(frc>=0/* && frc<=9*/);
		assert(frc<m_cross_layers.size());
		return m_cross_layers[frc]->m_layer_style;
	}

	void MapFrame::clear()
	{
		m_feature_layers.clear();
		m_annotation_layers.clear();
	}

	FeatureLayer * MapFrame::initialFeatureLayer( WSLayer& ws_layer )
	{
		FeatureLayer *ret = new FeatureLayer();
		ret->m_layer_id = ws_layer.LAYER_ID;
		ret->m_layer_global_index = 0;
		ret->m_layer_type = (FeatureLayer::FeatureType)ws_layer.LAYER_TYPE;
		ret->m_zoom_min = ws_layer.ZOOM_MIN_GM;
		ret->m_zoom_max = ws_layer.ZOOM_MAX_GM;
		ret->m_need_render = false;
		initialFeatureLayerStyle(ws_layer.LAYER_STYLE,ret);
		return ret;
	}

	AnnotationLayer * MapFrame::initialAnnotationLayer( WSLayer& ws_layer )
	{
		AnnotationLayer * ret = new AnnotationLayer();
		ret->m_layer_id = ws_layer.LAYER_ID;
		ret->m_layer_global_index = 0;
		ret->m_layer_type = (FeatureLayer::FeatureType)ws_layer.LAYER_TYPE;
		ret->m_zoom_max = ws_layer.ZOOM_MAX_LABEL;
		ret->m_zomm_min = ws_layer.ZOOM_MIN_LABEL;
		ret->m_need_render = false;
		initialAnnotationLayerStyle(ws_layer.LAYER_STYLE,ret);
		return ret;
	}
}