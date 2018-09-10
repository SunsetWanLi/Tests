#include "WorkspaceAdapter.h"
#include "DataReader.h"

namespace GIS_Embedded
{

	int WorkSpaceAdapter::_last_context_color = 0xffffff;


	int WorkSpaceAdapter::translate(byte_8 *data, int size, WorkSpace *ws)
	{
		//test(ws);
		//return 0;


		DataReader read;
		read.setStartPointer(data);

		read.getString(ws->m_workspace_id);
		ws->m_bg_color = read.getInt();

		int layer_sum = read.getInt();
		ws->m_ws_layers.resize(layer_sum);
		for(int i=0;i<layer_sum;i++)
			getLayerStyle(read,&ws->m_ws_layers[i]);

		getCrossStyle(read,&ws->m_cross_type);

		return 0;
	}

	void WorkSpaceAdapter::getCrossStyle(DataReader &read,CrossType* cross_type)
	{
		cross_type->m_bg_color = read.getInt();
		cross_type->m_arrow_width = read.getInt();
		cross_type->m_arrow_color = read.getInt();

		int layer_sum = read.getInt();
		cross_type->m_cross_layers.resize(layer_sum);
		for(int i=0;i<layer_sum;i++)
			getCrossLayerStyle(read,&cross_type->m_cross_layers[i]);
	}

	void WorkSpaceAdapter::getCrossLayerStyle(DataReader &read,WSCrossLayer* layer)
	{
		layer->FRC = read.getInt();
		layer->GRADE = read.getInt();
		layer->WIDTH = read.getInt();
	}

	void WorkSpaceAdapter::getLayerStyle(DataReader &read,WSLayer* layer)
	{
		layer->LAYER_ID = read.getString();
		layer->LAYER_TYPE = read.getByte()-'0';
		layer->ZOOM_MAX_GM = read.getInt();
		layer->ZOOM_MIN_GM = read.getInt();
		layer->ZOOM_MAX_LABEL = read.getInt();
		layer->ZOOM_MIN_LABEL = read.getInt();
		int gm_ranks = read.getInt();
		if(layer->LAYER_TYPE ==1)
		{
			layer->LAYER_STYLE.POINT_STYLE.resize(gm_ranks);
			for(int i=0;i<gm_ranks;i++)
				getPointStyle(read,&(layer->LAYER_STYLE.POINT_STYLE[i]));
		}
		else if(layer->LAYER_TYPE == 2)
		{
			layer->LAYER_STYLE.LINE_STYLE.resize(gm_ranks);
			for(int i=0;i<gm_ranks;i++)
				getLineStyle(read,&(layer->LAYER_STYLE.LINE_STYLE[i]));
		}
		else if(layer->LAYER_TYPE == 3)
		{
			layer->LAYER_STYLE.POLYGON_STYLE.resize(gm_ranks);
			for(int i=0;i<gm_ranks;i++)
				getPolygonStyle(read,&(layer->LAYER_STYLE.POLYGON_STYLE[i]));
			if(gm_ranks > 0)
				_last_context_color = layer->LAYER_STYLE.POLYGON_STYLE[0].second.FACE_COLOR;
		}
		else if(layer->LAYER_TYPE == 4)
		{

		}

		int label_ranks = read.getInt();
		layer->LAYER_STYLE.LABEL_STYLE.resize(label_ranks);
		for(int i=0;i<label_ranks;i++)
			getLabelStyle(read,&layer->LAYER_STYLE.LABEL_STYLE[i]);
	}

	void WorkSpaceAdapter::getPointStyle(DataReader& read,pair<int,WSPointStyle>* point_style)
	{
		point_style->first = read.getInt();
		read.getString(point_style->second.PIC_NAME);
	}

	void WorkSpaceAdapter::getLineStyle(DataReader& read,pair<int,WSLineStyle>* line_style)
	{
		line_style->first = read.getInt();

		line_style->second.TYPE = read.getByte()-'0';
		line_style->second.WIDTH = read.getShort();
		line_style->second.CURVE_COLOR = read.getInt();
		line_style->second.FACE_COLOR = read.getInt();
		line_style->second.COLOR_ALPHA = read.getShort();
		line_style->second.FILTER_PARA = read.getShort();
		line_style->second.ANTIALIAS = (bool)(read.getByte()-'0');
	}

	void WorkSpaceAdapter::getPolygonStyle(DataReader& read,pair<int,WSPolygonStyle>* polygon_style)
	{
		polygon_style->first = read.getInt();
		polygon_style->second.FACE_COLOR = read.getInt();
		polygon_style->second.COLOR_ALPHA = read.getShort();
		polygon_style->second.EDGE_COLOR = read.getInt();
		read.getString(polygon_style->second.PATTERN_PIC);
		polygon_style->second.FILTER_PARA = read.getShort();
		polygon_style->second.ANTIALIAS = bool(read.getByte()-'0');
		polygon_style->second.CONTEXT_COLOR = _last_context_color;
	}

	void WorkSpaceAdapter::getLabelStyle(DataReader& read,pair<int,WSLabelStyle>* label_style)
	{
		label_style->first = read.getInt();
		read.getString(label_style->second.FONT_NAME);
		label_style->second.FONT_SIZE = read.getShort();
		label_style->second.FONT_WEIGHT = read.getShort();
		label_style->second.FONT_COLOR = read.getInt();
		label_style->second.FONT_BG_COLOR = read.getInt();

	}

	//int WorkSpaceAdapter::translate(byte_8 *data, int size, WorkSpace **work_space)
	//{
	//	WorkSpace *ret = new WorkSpace();

	//	vector<WSFeatureLayer*> &ws_feature_layers = ret->_ws_feature_layers;
	//	vector<WSAnnotationLayer*> &ws_annotation_layers = ret->_ws_annotation_layers;
	//	Scope &extension = ret->_extension_scope;

	//	read = new DataReader();
	//	read->setStartPointer(data);

	//	int sum,i;

	//	sum = read->getInt();
	//	
	//	_last_color = 0xffffff;

	//	for(i=0;i<sum;i++)
	//	{
	//		WSFeatureLayer* ws_feature_layer = new WSFeatureLayer();
	//		ws_feature_layer->LAYER_ID = read->getString();
	//		getRenderLevelGroup(*read,&(ws_feature_layer->RENDER_LEVEL_GROUP)); //loujt 08-06-04
	//		getFeatureStyleGroup(*read,&(ws_feature_layer->FEATURE_STYLE_GROUP));
 //   		ws_feature_layer->DEFAULT_FLAG = read->getInt();
 //   		ws_feature_layer->EDITABLE_FLAG = read->getInt();
 //   		ws_feature_layer->FLOAT_FLAG = read->getInt();
 //   		ws_feature_layer->VISIBLE_FLAG =read->getInt();
 //   		ws_feature_layer->SHOWABLE_FLAG = read->getInt();
	//		ws_feature_layer->MIN_DISPLAY_RATIO = read->getDouble();
 //   		ws_feature_layer->MAX_DISPLAY_RATIO = read->getDouble();
	//		ws_feature_layers.push_back(ws_feature_layer);

	//	}

	//	sum = read->getInt();
	//	for(i = 0; i < sum; i++)
	//	{
 //   		WSAnnotationLayer* ws_annotation_layer = new WSAnnotationLayer();
 //   		ws_annotation_layer->LAYER_ID = read->getString();
	//		getAnnotationStyleGroup(*read,&ws_annotation_layer->ANNOTATION_STYLE_GROUP);
	//		ws_annotation_layers.push_back(ws_annotation_layer);
	//	}

	//	*work_space = ret;

	//	delete read;

	//	return 0;

	//}

	//void WorkSpaceAdapter::getRenderLevelGroup(DataReader &read,vector<WSRenderLevel*> *feature_render_level_group)
	//{
	//	int level_sum = read.getInt();
	//	WSRenderLevel *ws_render_level = NULL;
	//	if(level_sum == 0)
	//		return;

	//	for(int i=0;i<level_sum;i++)
	//	{
	//		ws_render_level = new WSRenderLevel();

	//		ws_render_level->SCALE = read.getDouble();
	//		getPointStyle(read,&ws_render_level->POINT);
	//		getLineStyle(read,&ws_render_level->LINE);
	//		getPolygonStyle(read,&ws_render_level->POLYGON);

	//		feature_render_level_group->push_back(ws_render_level);

	//	}

	//}


	//void WorkSpaceAdapter::getFeatureStyleGroup(DataReader &read,vector<WSFeatureLayerStyle*> *feature_layer_style_group)
	//{


	//	int style_sum = read.getInt();
	//	WSFeatureLayerStyle *ws_feature_layer_style = NULL;
	//	if(style_sum == 0)
	//		return;

	//	for(int i = 0; i < style_sum; i++)
	//	{
	//		ws_feature_layer_style = new WSFeatureLayerStyle();
	//		//vector oid_vector = new Vector();
	//		//ws_feature_layer_style->MAX_SCALE = read.getDouble(); //loujt 08-03-14 ,临时作为比例尺调整道路宽度参�?
	//		int oid_sum = read.getInt();
	//		for(int j = 0; j < oid_sum; j++)
	//			ws_feature_layer_style->GOID_VECTOR.push_back(read.getInt64());

	//		getPointStyle(read,&ws_feature_layer_style->POINT);
	//		getLineStyle(read,&ws_feature_layer_style->LINE);
	//		getPolygonStyle(read,&ws_feature_layer_style->POLYGON);

 //       	ws_feature_layer_style->POLYGON.CONTEXT_COLOR = _last_color;
	//		feature_layer_style_group->push_back(ws_feature_layer_style);
	//	}

	//	if(ws_feature_layer_style != NULL)
	//		_last_color = ws_feature_layer_style->POLYGON.COLOR;

	//}

	//void WorkSpaceAdapter::getAnnotationStyleGroup(DataReader &read,vector<WSAnnotationLayerStyle*> *annotation_layer_style_group)
	//{

	//	int style_sum = read.getInt();
	//	if(style_sum == 0)
	//		return;

	//	for(int i = 0; i < style_sum; i++)
	//	{
	//		WSAnnotationLayerStyle *ws_annotation_layer_style = new WSAnnotationLayerStyle();

	//		int oid_sum = read.getInt();
	//		for(int j = 0; j < oid_sum; j++)
	//			ws_annotation_layer_style->GOID_VECTOR.push_back(read.getInt64());
 //     
	//		if(read.getInt() == 0)
	//			ws_annotation_layer_style->Label.DW_BK = false;
	//		else
	//			ws_annotation_layer_style->Label.DW_BK = true;
 //       
	//		getPolygonStyle(read,&ws_annotation_layer_style->Label.BACK_STYLE);

	//		ws_annotation_layer_style->Label.FONT_NAME = read.getString();
	//		ws_annotation_layer_style->Label.FONT_COLOR = read.getInt();
	//		ws_annotation_layer_style->Label.FONT_SIZE = read.getInt();

	//		annotation_layer_style_group->push_back(ws_annotation_layer_style);
	//	}

	//}

	//void WorkSpaceAdapter::getPointStyle(DataReader& read,WSPointStyle* point_style)
	//{

 //   	point_style->COLOR = read.getInt();
 //   	point_style->COLOR_ALPHA = read.getInt();
 //   	point_style->POINT_FONT_SIZE = read.getInt();
 //   	point_style->POINT_FONT_NAME = read.getString();
 //   	point_style->POINT_SHAPE = read.getString();
 //   	point_style->POINT_TYPE = read.getInt();
 //   	point_style->PIC_NAME = read.getString();
 //   	point_style->COLOR_GRADE = read.getString();

	//}


	//void WorkSpaceAdapter::getLineStyle(DataReader& read,WSLineStyle* line_style)
	//{

	//	getPointStyle(read,&line_style->POINT);

 //   	line_style->COLOR = read.getInt();
 //   	line_style->COLOR_ALPHA = read.getInt();
 //   	line_style->LINE_WIDTH = read.getInt();
 //   	line_style->LINE_STYLE = read.getInt();

	//}


	//void WorkSpaceAdapter::getPolygonStyle(DataReader& read,WSPolygonStyle* polygon_style)
	//{

	//	getPointStyle(read,&polygon_style->POINT);
	//	getLineStyle(read,&polygon_style->LINE);

	//	polygon_style->COLOR = read.getInt();
	//	polygon_style->COLOR_ALPHA = read.getInt();
	//	polygon_style->POLYGON_HATCH = read.getInt();
	//	polygon_style->POLYGON_PATTERN = read.getInt();
 //   	polygon_style->POLYGON_SIZERATE = read.getInt();
 //   	polygon_style->POLYGON_WIDTH = read.getInt();
 //   	polygon_style->POLYGON_TYPE = read.getInt();

	//}

}