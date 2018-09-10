/*
* =====================================================================================
*
*       Filename:  MKStandardDataFetcher.cpp
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

#include "MKStandardDataFetcher.h"
#include "Retiguide/DataFetcher/DataFetcher.h"
#include "Retiguide/Common/Types/Settings.h"
#include "Retiguide/Common/Types/WorkSpaceTypes/TerrCtrl.h"
#include "Retiguide/Common/Types/MapConfigType.h"
#include "Retiguide/Render/VctorRender/Map/RenderFlagAdjuster.h"
#include "MapKit/Embedded/Application/AttributeMgr.h"
#include "System/Shared/MGLog.h"
#include "GraphicCore/MGCanvas.h"
#include "GraphicCore/Canvas/CGCanvas.h"
#include "GraphicCore/MGPPMCodec.h"
#include "GraphicCore/MGPNGCodec.h"
#include "MapKit/Fetcher/Pyramid/MKGlobalMapTiles.h"
#include "Retiguide/Common/CRS/GlobalCrt.h"
#include "MapKit/Embedded/BasicType/Line.h"

#include "MapKit/Views/MapViews/MKMapView.h"
#include "MapKit/Embedded/BasicType/Point.h"
using namespace z11;
using namespace GIS_Embedded;
using namespace Embedded;
#define MKMapViewCellWidth MAP_CELL_SIZE
#define MKMapViewCellHeight MAP_CELL_SIZE
static uint_32 adaptSize(uint_32 size)
{
	return size;
	uint_32 ret=1;
	//size<<=1;
	size--;
	while (size!=0)
	{
		ret<<=1;
		size>>=1;
	}
	ret = MAXVALUE(ret,1024);
	return ret;
}
extern z11::MKMapView *test;
namespace z11
{
	bool fetcher_init=false;
	void MKStandardDataFetcher::init(int width,int height)
	{
		if (fetcher_init)
		{
			return ;
		}
		fetcher_init=true;
		
		width =adaptSize(width);
		height=adaptSize(height);
		m_screen_size.width=width;
		m_screen_size.height=height;
		GIS_Embedded::DataFetcher *data_fetcher = GIS_Embedded::DataFetcher::getInstance();//init some setting of DataFetcher,read m_map_config,m_setting,m_languages,m_current_language
		Settings* setConf;
		data_fetcher->getSettings(&setConf);				//return  DataFetcher->m_setting
		TerrCtrl *terr_ctrl;
		data_fetcher->getTerrsConf(&terr_ctrl);				//reading GIS_Embedded/conf/TerrsConf.tc
		MapConfigType *map_config;
		data_fetcher->getMapConfig(&map_config);			//return DataFetcher->m_map_config


		m_scale_rank = map_config->initial_scale;
		double new_width = m_scale_rank*width/(double)map_config->scale_pixel_width;
		double new_height = new_width*height/width;

        //setConf->map.host_pos_x=13422854;
        //setConf->map.host_pos_y=3478714;
		double view_h_center= setConf->map.host_pos_y;
		double view_w_center= setConf->map.host_pos_x;
		terr_ctrl->initialScopeCenter(setConf->map.host_pos_x,setConf->map.host_pos_y,&view_w_center,&view_h_center);


		//Scope view_scope;
		m_map_scope.setBottom(view_h_center - new_height/2);
		m_map_scope.setTop(view_h_center + new_height/2);
		m_map_scope.setLeft(view_w_center - new_width/2);
		m_map_scope.setRight(view_w_center + new_width/2);

		m_view_scope=MKMapRect(m_map_scope.m_left,m_map_scope.m_bottom,new_width,new_height);

		m_feature_layer_render.setTransformer(&m_transformer);
		m_feature_layer_render.setViewScopeAdjust(&m_map_scope);
		m_feature_layer_render.setIsRasterMap(false);

		m_datasrc_nodes = m_map_frame.getFclsGlobalIndex()->getAllFclsNames();
		int src_size = m_datasrc_nodes.size();

		m_layerMgr = Embedded::LayerMgr::getInstance();
		for (int i=0; i<src_size; i++)
		{
			int fcls_size = m_datasrc_nodes[i].second.size();

			for (int j=0 ;j <fcls_size; j++)
			{
				std::string fcls_name = m_datasrc_nodes[i].second[j];
				m_fcls_names.push_back(fcls_name);
				std::string file_name = MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/data/"+fcls_name+".dat");
				z11::MGWString wstrLayerName = Utility::ustr2wstr(file_name);
				m_layerMgr->loadLayer(wstrLayerName);
			}
		}
		AttributeMgr::getInstance()->loadAttributes(Utility::ustr2wstr(MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/data")));

		m_map_terr_ctrl = new MapTerrCtrl(setConf,*terr_ctrl);

		m_map_terr_ctrl->change(m_map_scope,&m_map_frame);
		RenderFlagAdjuster::adjust(m_map_frame,m_scale_rank);


		m_screen_pix_map.setScreenSize(width,height,10,10);
		
		m_scale_ranks=map_config->scale_ranks;
		//getLayerDataInScope(m_view_scope);

		if(true)
		{
			WSLayer layer;
			layer.LAYER_ID=Utility::astr2wstr("HighSpeed::Traffic_Road");
			{
				WSLineStyle style;
				style.TYPE=3;
				style.WIDTH=20;
				style.CURVE_COLOR=0xffffff;
				style.FACE_COLOR=0xff0000;
				style.COLOR_ALPHA=1;
				style.FILTER_PARA=10;
				style.ANTIALIAS=false;
				pair<int,WSLineStyle> line_style=make_pair(5000,style);
				layer.LAYER_STYLE.LINE_STYLE.push_back(line_style);
			}
			layer.LAYER_TYPE=2;
			layer.ZOOM_MAX_GM=5000;
			layer.ZOOM_MIN_GM=0;
			layer.ZOOM_MAX_LABEL=5000;
			layer.ZOOM_MIN_LABEL=0;
			m_traffic_feature_layer=m_map_frame.initialFeatureLayer(layer);
			m_traffic_feature_layer->m_feature_layer_style.m_current_style=&m_traffic_feature_layer->m_feature_layer_style.m_ceiline_scale_styles[0].second;
			m_traffic_feature_layer->m_is_path_layer=true;
			m_traffic_annotation_layer=m_map_frame.initialAnnotationLayer(layer);
		}

	}

	void MKStandardDataFetcher::getLayerDataInScope( const MKMapRect& _view_scope )
	{
		AUTO_LOCK_THIS;
		adjustScope(_view_scope);

		m_map_frame.getMapLayersNeedRender(m_scale_rank,m_map_scope,&m_layer_global_ids,&m_layer_mapframe_indexs);
		Embedded::MapPos2D leftlow(m_map_scope.m_left,m_map_scope.m_bottom), righttop(m_map_scope.m_right,m_map_scope.m_top);
		Embedded::Rect r(leftlow,righttop);

		std::vector<Embedded::IShape*> shapes;

		clearData();
		int fcls_size = m_layer_global_ids.size();
		for (int i=0; i<fcls_size; i++)
		{
			std::string fcls_name = m_fcls_names[m_layer_global_ids[i]];
			z11::MGWString wFcls_name = Utility::ustr2wstr(fcls_name);
			m_layerMgr->queryShapeByRect(r, shapes, wFcls_name);
			m_shapes.push_back(shapes);
			shapes.clear();
		}
	}
	static MGFont *sharedFont()
	{
		static MGFont*font=MGFont::create(17,RGBAColorRef::black());
		return font;
	}
	void MKStandardDataFetcher::render(MGCanvas *canvas)
	{
		AUTO_LOCK_THIS;
		m_screen_pix_map.clear();

		m_style_render.setCanvas(canvas);
		//m_map_scope.setBottom(m_view_scope.getMinY());
		//m_map_scope.setTop(m_view_scope.getMaxY());
		//m_map_scope.setLeft(m_view_scope.getMinX());
		//m_map_scope.setRight(m_view_scope.getMaxX());

		assert(m_screen_size.width==256);
		m_transformer.setTansformPara(m_map_scope,0,m_screen_size.width,m_screen_size.height);

		int i,sum = m_layer_global_ids.size();
		int start=0;
		//draw polygon & polyline
		for (i=start;i<sum;i++)
		{
			FeatureLayer* f_layer=m_map_frame.m_feature_layers[m_layer_mapframe_indexs[i]];
			AnnotationLayer* a_layer= m_map_frame.m_annotation_layers[m_layer_mapframe_indexs[i]];

			if(a_layer->m_layer_type == FeatureLayer::t_POINT) 
			{
				break;
			}

			a_layer->clearGeoLabelPos();
			m_feature_layer_render.renderMapLayer(m_style_render,*f_layer,m_shapes[i],*a_layer,m_screen_pix_map);
		}
		
		//draw all label
		for (i=start;i<sum;i++)
		{
			AnnotationLayer* a_layer= m_map_frame.m_annotation_layers[m_layer_mapframe_indexs[i]];
			if(a_layer->m_layer_type == FeatureLayer::t_LINE)
			{
				continue;
			}
			for (int j=0;j<a_layer->m_labels.size();j++)
			{
				MGFont *shared_font=sharedFont();
				if (a_layer->m_annogation_layer_style.m_ceiline_scale_styles.empty())
				{
					shared_font->setFaceSize(17);
					shared_font->setColor(RGBAColorRef::black());
				}
				else
				{
					pair<int,LabelStyleRender>& layer_style = a_layer->m_annogation_layer_style.m_ceiline_scale_styles[0];
					shared_font->setFaceSize(layer_style.second.getFontSize());
					shared_font->setColor(layer_style.second.getLabelColor());
				}
				canvas->drawText(a_layer->m_labels[j],MGRectMake(a_layer->m_label_lfTp[j].x,a_layer->m_label_lfTp[j].y,200,100),shared_font);
			}

		}
	}

	MKStandardDataFetcher* MKStandardDataFetcher::fetcher=NULL;

	MGMutex mutex;
	MKStandardDataFetcher* MKStandardDataFetcher::getInstance()
	{
		if (fetcher==NULL)
		{
			AUTO_LOCK(&mutex);
			if (fetcher==NULL)
			{
				fetcher=new MKStandardDataFetcher();
				fetcher->init(MKMapViewCellWidth,MKMapViewCellHeight);
			}
		}

		return fetcher;
	}

	MGImage * MKStandardDataFetcher::getImage( const MKGlobalTileIndex &index,MKMapType type )
	{
        AUTO_LOCK_THIS;
		string path=getCacheFolderPath(index,type);
		MGImage *ret=NULL;
		if (!Utility::isFileORDirExist(path))
		{
			MKMapRect map_rect=getViewScope(index);
			if (type==MKMapTypeCustom)
			{
				{
					//MGTimeLog log(" MKStandardDataFetcher::get data time %d,%d,%d",index.x,index.y,index.z);
					getLayerDataInScope(map_rect);
				}
				static MGCanvas canvas(MKMapViewCellWidth,MKMapViewCellHeight,16);
				{
					//MGTimeLog log(" MKStandardDataFetcher::init time %d,%d,%d",index.x,index.y,index.z);
					canvas.clear();
					canvas.RectFill(MGRectMake(0,0,MKMapViewCellWidth,MKMapViewCellHeight),RGBAColorRef::lightGray());
				}
				{
					//MGTimeLog log(" MKStandardDataFetcher::render time %d,%d,%d",index.x,index.y,index.z);
					render(&canvas);
				}
				{
					//MGTimeLog log(" MKStandardDataFetcher::encode time %d,%d,%d",index.x,index.y,index.z);
					canvas.writeToFile(path,true);
				}
			}
			else if(type==MKMapTypeCustomTraffic)
			{
				adjustScope(map_rect);
				static MGCanvas canvas(MKMapViewCellWidth,MKMapViewCellHeight,32);
				//canvas.RectFill(MGRectMake(0,0,MKMapViewCellWidth,MKMapViewCellHeight),RGBAColorRef::lightGray());
				renderTrafficRoad(&canvas);
				canvas.writeToFile(path,true);
			}else
                return ret;
		}
		{
			//MGTimeLog log(" MKStandardDataFetcher::loading time %d,%d,%d",index.x,index.y,index.z);
			ret = MGImage::imageWithContentsOfFile(path);
		}
		return ret;
	}

	MKStandardDataFetcher::MKStandardDataFetcher()
	{
		m_canvas=new MGCanvas();
		m_traffic_feature_layer=NULL;
		m_traffic_annotation_layer=NULL;
	}

	void MKStandardDataFetcher::adjustScope( const MKMapRect& _view_scope )
	{
		m_view_scope=_view_scope;
		m_map_scope.setLeft(m_view_scope.getMinX());
		m_map_scope.setTop(m_view_scope.getMaxY());
		m_map_scope.setRight(m_view_scope.getMaxX());
		m_map_scope.setBottom(m_view_scope.getMinY());

		m_map_terr_ctrl->change(m_map_scope,&m_map_frame);
		RenderFlagAdjuster::adjust(m_map_frame,m_scale_rank);
	}

	void MKStandardDataFetcher::clearData()
	{
		for(int i = 0; i < m_shapes.size(); ++i)
			for (int j=0; j<m_shapes[i].size(); ++j)
				delete m_shapes[i][j];

		m_shapes.clear();
	}

	std::string MKStandardDataFetcher::getCacheFolderPath( const MKGlobalTileIndex &index,MKMapType type )
	{
		int x=index.x;
		int y=index.y;
		int z=index.z;
		char filename[256];
		GlobalMercator mercator;
		//MGIntPoint g= mercator.GoogleTile(index);
		//std::string quad=mercator.QuadTree(x,y,z);
		std::string tmp_path;
		if (type==MKMapTypeCustom)
		{
			tmp_path=MGBundle::mainBundle()->tmpPathWithFileName("draw_caches/");
		}
		else if(type==MKMapTypeCustomTraffic)
		{
			tmp_path=MGBundle::mainBundle()->tmpPathWithFileName("draw_traffic_caches/");
		}
		
		sprintf(filename,"%d/%d/",z,x);
		tmp_path+=filename;
		if (!Utility::isFileORDirExist(tmp_path))
			Utility::createDirOnPath(tmp_path);
		sprintf(filename,"%d.jpg",y);
		tmp_path+=filename;
		return tmp_path;
	}

	z11::MKMapRect MKStandardDataFetcher::getViewScope( const MKGlobalTileIndex &index )
	{
        AUTO_LOCK_THIS;
		GlobalMercator mercator;
		fetcher->m_scale_rank=(1<<(21-index.z));
		MGDoubleRect meter_rect=mercator.TileBounds(index.x,index.y,index.z);
		MKMapRect map_rect(meter_rect.x,meter_rect.y,meter_rect.width,meter_rect.height);
		{
			MGDoublePoint lefttop=mercator.MetersToLatLon(meter_rect.x,meter_rect.y);
			MGDoublePoint rightbottom=mercator.MetersToLatLon(meter_rect.x+meter_rect.width,meter_rect.y+meter_rect.width);
			GlobalCrt crt;

			MGIntPoint lefttop2;
			crt.geoToPrj(lefttop.y,lefttop.x,&lefttop2.x,&lefttop2.y);
			MGIntPoint rightbottom2;
			crt.geoToPrj(rightbottom.y,rightbottom.x,&rightbottom2.x,&rightbottom2.y);

			map_rect.origin.x=lefttop2.x;
			map_rect.origin.y=lefttop2.y;
			map_rect.size.width=rightbottom2.x-lefttop2.x;
			map_rect.size.height=rightbottom2.y-lefttop2.y;
		}
		return map_rect;
	}

	void MKStandardDataFetcher::renderTrafficRoad( MGCanvas *canvas )
	{
		AUTO_LOCK_THIS;
		if (m_scale_rank>m_traffic_annotation_layer->m_zoom_max||m_scale_rank<m_traffic_annotation_layer->m_zomm_min)
		{
			return ;
		}
		
		std::vector<Embedded::IShape*> shape;
		m_screen_pix_map.clear();
		m_style_render.setCanvas(canvas);

		assert(m_screen_size.width==256);
		m_transformer.setTansformPara(m_map_scope,0,m_screen_size.width,m_screen_size.height);

		clearData();
		double geo[]={120.2223969,30.28583375,120.2230706,30.28581815,120.2234928,30.28584934,120.2237713,30.28588832,120.2240857,30.28588052,120.2243821,30.28580256,120.2246516,30.28565443,120.2248133,30.28550631,120.224975,30.28528022,120.2251367,30.2849138,120.2252355,30.2845084,120.2252804,30.28390029,120.2252804,30.28390029,120.2253254,30.28297253,120.2253523,30.28240339,120.2253703,30.28223187,120.2253793,30.28209154,120.2253972,30.28192002,120.2254691,30.28135087,120.2255589,30.28093766,120.2256937,30.28043089,120.2257655,30.28018919,120.226071,30.27942513,120.2263045,30.27894173,120.2264393,30.27870004,120.2266998,30.27827122,120.2269962,30.2778346,120.227122,30.27766307,120.2272747,30.27747595,120.2273915,30.27733561,120.2274454,30.27727323,120.2276879,30.27698475,120.2280652,30.27658711,120.2283976,30.27628303,120.2289186,30.2758542,120.2294037,30.27550334,120.2297091,30.27531621,120.2299517,30.27516807,120.2301044,30.27507451,120.2305536,30.27480161,120.2309219,30.27459889,120.2312632,30.27439617,120.2317034,30.27414666,120.2321166,30.27390495,120.2323232,30.273788,120.2325478,30.27365545,120.2329341,30.27342933,120.2332305,30.27325779,120.2338773,30.27289133,120.2343894,30.27260283,120.2348295,30.27234553,120.2353056,30.27208042,120.2354045,30.27202584,120.235629,30.27190108,120.2358985,30.27174514,120.2365453,30.27137867,120.2369675,30.27113695,120.2374886,30.27083286,120.2385126,30.27025585,120.2388809,30.27005312,120.2398421,30.2695073,120.240992,30.26884452,120.2421778,30.26816614,120.2430671,30.2676593,120.2439295,30.26716805,120.2442529,30.2669809,120.2447739,30.2666768,120.2456812,30.26616215,120.2462022,30.26587363,120.2465436,30.26570208,120.2471634,30.26539017,120.2476665,30.26514065,120.2483043,30.26483653,120.2488522,30.2645792,120.2489511,30.26454022,120.2496428,30.2642361,120.2500021,30.26408794,120.2501638,30.26401776,120.250595,30.26383841,120.2511699,30.26361227,120.2512238,30.26359667,120.2559759,30.26167837,120.2566856,30.26138984,120.256982,30.26126507,120.266558,30.25739714,120.2666658,30.25735035,120.2683726,30.25665629,120.2704657,30.25581406,120.2720737,30.25517458,120.2730529,30.25476905,120.2746698,30.25409837,120.2763317,30.25336529,120.276727,30.25318592,120.2771671,30.25299095,120.277248,30.25295196,120.2784966,30.25237485,120.2795926,30.25186012,120.2808412,30.25125181,120.2812455,30.25103344,120.2821528,30.25057331,120.2824762,30.25039393,120.282575,30.25033934,120.2829074,30.25015996,120.2830242,30.25009757,120.2832128,30.25000398,120.2833026,30.24995719,120.2834913,30.2498558,120.2834913,30.2498558,120.2837159,30.24973882,120.2840393,30.24956724,120.2845782,30.24926307,120.2849106,30.2490759,120.2853238,30.24884972,120.2856742,30.24866255,120.2860245,30.24846757,120.2863659,30.24827259,120.2866084,30.2481478,120.2871834,30.24782804,120.2875606,30.24762526,120.2875606,30.24762526,120.2877942,30.24749267,120.2882434,30.2472431,120.2887464,30.24697013,120.2891417,30.24675175,120.2895998,30.24650217,120.29004,30.24626039,120.2905341,30.24597962,120.2911898,30.24562085,120.2917468,30.24531667,120.2921061,30.24511389,120.292169,30.24508269,120.2922588,30.2450359,120.2925104,30.24489551,120.2930044,30.24462253,120.2934805,30.24435735,120.2938489,30.24415456,120.2944238,30.24384258,120.2949628,30.2435462,120.2954299,30.24328882,120.2961575,30.24289884,120.2965707,30.24268045,120.2969211,30.24250106,120.2972984,30.24230607,120.2978823,30.24200969,120.2983494,30.2417835,120.2988345,30.24154171,120.2992747,30.24133112,120.299652,30.24115172,120.3002179,30.24088653,120.3006132,30.24070714,120.3011072,30.24048095,120.3014756,30.24031715,120.301772,30.24019236,120.3021223,30.24003636,120.3024996,30.23987256,120.3030117,30.23966197,120.3030566,30.23963857,120.3036495,30.23938897,120.3039639,30.23926418,120.3045478,30.23903018,120.3051137,30.23880398,120.3055898,30.23861679,120.306039,30.23845299,120.3064343,30.23829699,120.3070092,30.23807859,120.3075032,30.23790699,120.3079255,30.23775099,120.3084016,30.23758719,120.3089226,30.23740779,120.3094526,30.23722839,120.3099377,30.23705679,120.3104946,30.23686959,120.3108809,30.23673699,120.3113121,30.23658879,120.3117972,30.23642499,120.3122643,30.23626119,120.3124979,30.23617539,120.3126686,30.23612079,120.3131716,30.23594918,120.3137286,30.23575418,120.315615,30.23511457,120.3174117,30.23447495,120.3199269,30.23362472,120.3228195,30.23263408,120.3244904,30.23208805,120.3270865,30.231269,120.3295928,30.23052015,120.3323147,30.22975569,120.3345336,30.22917064,120.3347132,30.22912384,120.3354139,30.22893662,120.3382616,30.22825016,120.3386748,30.22814875,120.3393036,30.22800834,120.3398965,30.22787573,120.3404265,30.22775091,120.3410014,30.2276261,120.3417021,30.22747789,120.3426903,30.22725946,120.3429598,30.22719706,120.3437503,30.22704104,120.3437593,30.22703324,120.3443432,30.22690843,120.344972,30.22678361,120.3467327,30.22642477,120.3475951,30.22625315,120.3489874,30.22595672,120.3499756,30.22575389,120.3506224,30.22561348,120.3507571,30.22558227,120.3522483,30.22524683,120.3528322,30.22512202,120.3531287,30.22505961,120.3531287,30.22505961,120.3539911,30.22487239,120.3547816,30.22470076,120.3560123,30.22441212,120.3573957,30.22407668,120.3584287,30.22383485,120.3589228,30.22371003,120.3592013,30.22363982,120.3592013,30.22363982,120.3589857,30.22385825,120.3589857,30.22385825,120.3584737,30.22398307,120.3574496,30.2242249,120.3560572,30.22456034,120.3548265,30.22484898,120.354036,30.2250206,120.3522932,30.22539505,120.3522932,30.22539505,120.3507571,30.22569929,120.3506134,30.22573829,120.3500205,30.22586311,120.3490144,30.22607373,120.347622,30.22637017,120.3467686,30.22654179,120.344972,30.22690843,120.3443791,30.22702544,120.3437952,30.22715025,120.3437503,30.22715805,120.3429957,30.22731407,120.3427262,30.22737648,120.341738,30.2275871,120.3410374,30.22774311,120.3404624,30.22786793,120.3399324,30.22798494,120.3393395,30.22812535,120.3387107,30.22826576,120.3382975,30.22835937,120.3374531,30.22856219,120.3351803,30.22912384,120.3350007,30.22917064,120.3323596,30.2298649,120.3296377,30.23062936,120.3271314,30.23137821,120.3245353,30.23219725,120.3228644,30.23274328,120.3199719,30.23373392,120.3174566,30.23458415,120.3156599,30.23522377,120.3137286,30.23587898,120.3132165,30.23605839,120.3127225,30.23622999,120.3124979,30.23630799,120.3123182,30.23637039,120.3118511,30.23653419,120.311357,30.23669799,120.3109348,30.23684619,120.3105485,30.23697879,120.3099916,30.23716599,120.3094975,30.23733759,120.3089765,30.23751699,120.3084555,30.23769639,120.3079794,30.23786019,120.3075571,30.23801619,120.3070631,30.23818779,120.3064882,30.23840619,120.3060929,30.23855439,120.3056527,30.23872599,120.3051766,30.23891318,120.3046017,30.23913158,120.3040268,30.23936558,120.3037034,30.23949817,120.3031105,30.23974777,120.3030656,30.23976337,120.3025625,30.23997396,120.3021852,30.24014556,120.3018259,30.24029375,120.3015384,30.24042635,120.3011701,30.24059014,120.3006761,30.24080854,120.3002808,30.24099573,120.2997149,30.24125312,120.2993376,30.24143251,120.2989064,30.2416431,120.2984123,30.24188489,120.2979542,30.24211108,120.2973613,30.24239967,120.296984,30.24260246,120.2966336,30.24278185,120.2962294,30.24300023,120.2955018,30.24339021,120.2950346,30.2436476,120.2944956,30.24394398,120.2939207,30.24425595,120.2935524,30.24445874,120.2930763,30.24472392,120.2925822,30.2449969,120.2923307,30.24512949,120.2922409,30.24518409,120.292178,30.24521528,120.2918187,30.24541027,120.2912617,30.24572224,120.2906059,30.24608101,120.2901119,30.24635398,120.2896717,30.24659576,120.2892135,30.24685314,120.2888183,30.24707152,120.2883152,30.24734449,120.2878661,30.24759406,120.2874349,30.24783584,120.2872552,30.24792943,120.2872552,30.24792943,120.2866803,30.24824919,120.2864378,30.24837398,120.2860964,30.24856896,120.2857461,30.24875613,120.2853957,30.24895111,120.2849825,30.24917728,120.2846501,30.24936446,120.2841111,30.24966083,120.2837877,30.2498402,120.2837248,30.2498714,120.2835721,30.24995719,120.2835721,30.24995719,120.2834913,30.25000398,120.2832038,30.25016776,120.2829882,30.25028474,120.2828625,30.25034714,120.2826828,30.25044072,120.2824762,30.25054211,120.2822247,30.25067469,120.2812455,30.25117382,120.2809041,30.2513532,120.2796645,30.25196931,120.2785595,30.25248403,120.2773109,30.25306114,120.2771761,30.25312353,120.2767359,30.2533263,120.2763946,30.25347447,120.2747327,30.25420755,120.2731157,30.25487823,120.2721366,30.25528376,120.2705196,30.25593103,120.2684355,30.25676547,120.2667197,30.25746733,120.2666119,30.25750632,120.2570359,30.26138204,120.2569281,30.26142883,120.2562184,30.26170956,120.2560028,30.26179534,120.2512238,30.26372144,120.2506579,30.26394758,120.2502267,30.26412693,120.2500021,30.2642205,120.2496517,30.26436087,120.2495439,30.26440765,120.2488433,30.26471177,120.2483672,30.2649379,120.2477293,30.26524982,120.2472353,30.26549155,120.2466424,30.26579566,120.2462651,30.26595941,120.2457531,30.26625572,120.2448458,30.26677037,120.2443247,30.26707448,120.2440013,30.26725382,120.24313,30.26775287,120.2422496,30.26825971,120.2410638,30.26893809,120.239905,30.26959308,120.2389528,30.27013889,120.2385845,30.27034942,120.237803,30.27078607,120.2377221,30.27083286,120.2370394,30.27122272,120.2366082,30.27146444,120.2359704,30.27183091,120.2356919,30.27199465,120.2354763,30.27211161,120.2353685,30.27217399,120.2349014,30.27243909,120.2344522,30.2726886,120.2339492,30.2729771,120.2333024,30.27335136,120.232997,30.2735229,120.2326197,30.27374901,120.2323951,30.27387377,120.2321795,30.27399852,120.2317753,30.27423243,120.2313351,30.27448973,120.2309848,30.27469245,120.2306254,30.27489518,120.2301763,30.27516027,120.2300236,30.27525384,120.2294846,30.27558911,120.2289995,30.27593217,120.2284785,30.276361,120.228164,30.27665728,120.2277957,30.27706272,120.2275532,30.2773434,120.2274903,30.27741357,120.2274005,30.27753053,120.2272388,30.27772544,120.227104,30.27789697,120.2268166,30.27832579,120.2265561,30.27874682,120.2264213,30.27898851,120.2261967,30.27946411,120.2259003,30.28022818,120.2258194,30.28045428,120.2256847,30.28096105,120.2256038,30.28136647,120.225532,30.28193561,120.225523,30.28199018,120.225514,30.28213052,120.2254871,30.28251254,120.2254871,30.28251254,120.2254601,30.28298032,120.2254152,30.28390029,120.2253523,30.28570901,120.2253523,30.28579476,120.2253523,30.28579476,120.2253164,30.28683944,120.2252625,30.28763463,120.2251816,30.28954463,120.2251727,30.2899656,120.2251457,30.29102582,120.2251457,30.29128308,120.2251367,30.29161829,120.2251367,30.29166507,120.2251367,30.29168845,120.2251367,30.2919613,120.2251637,30.29246801,120.2251727,30.29262393,120.2251906,30.29285779,120.2251906,30.29289677,120.2252086,30.293193,120.2253074,30.29384782,120.2254242,30.29460398,120.225514,30.29497037,120.2255949,30.29528998,120.2256488,30.29552384,120.2257476,30.2959136,120.2259901,30.29667754,120.2261608,30.29716084,120.2265291,30.29807288,120.2268974,30.298829,120.2273645,30.29970205,120.2275532,30.30001385,120.2278856,30.30055949,120.2283527,30.30126883,120.2293139,30.30270308,120.2296193,30.30317076,120.2297091,30.30330327,120.2297091,30.30330327,120.2300056,30.30373978,120.2303469,30.30426981,120.2307602,30.30485441,120.2310836,30.30536106,120.2315687,30.30607815,120.2324939,30.30748894,120.2326556,30.30774616,120.2330149,30.30827617,120.2333114,30.30872045,120.2335629,30.3090556,120.2337785,30.30938296,120.2341648,30.30995194,120.2345331,30.31051312,120.235108,30.31138606,120.2355392,30.31201737,120.2358357,30.31247722,120.2360692,30.31281236,120.2360692,30.31281236,120.2360692,30.31281236,120.236204,30.31300721,120.2370574,30.31430879,120.2376053,30.31511155,120.2377491,30.31532977,120.2384587,30.3164053,120.2390426,30.3172704,120.2395367,30.3179952,120.239896,30.31853295,120.2400937,30.31880572,120.2405159,30.31939023,120.2413244,30.32045792,120.2418813,30.32116712,120.2422047,30.32156457,120.2427257,30.32220362,120.2429054,30.32241403,120.2433366,30.32292059,120.2438756,30.32351286,120.244172,30.32384797,120.2443786,30.32408176,120.2454656,30.32521954,120.2462112,30.32596766,120.246867,30.32661447,120.2474149,30.32712879,120.2484749,30.32810289,120.2487085,30.32832108,120.2491846,30.3287341,120.2498853,30.32932634,120.2512687,30.33044068,120.2520323,30.33104849,120.2526791,30.331555,120.2531372,30.33192125,120.2534696,30.33217839,120.2540176,30.33261477,120.2544487,30.33295763,120.2548979,30.33330829,120.2568832,30.33488232,120.2574401,30.33531868,120.2577725,30.33558361,120.2586708,30.33629269,120.2594074,30.3368693,120.2596859,30.33709527,120.2600991,30.33742254,120.2605842,30.33781213,120.2610424,30.33816277,120.2613658,30.3384199,120.2619048,30.33884846,120.2624617,30.33930817,120.2632792,30.3399471,120.2635846,30.34019644,120.263917,30.34046136,120.2644739,30.34089769,120.2652555,30.34151323,120.2663514,30.34237031,120.2668275,30.34274431,120.2673216,30.34312609,120.2673575,30.34315726};
		uint_32 pos[]={12,95,12,94,8,2,7,93,12,93,5,33,18,58};
		RGBAColorRef color[]={RGBAColorMake(153,0,0),RGBAColorMake(255,204,0),RGBAColorMake(51,177,0)};
		uint_32 color_size=sizeof(color)/sizeof(RGBAColorRef);
		double *p=geo;
		int len=sizeof(pos)/sizeof(uint_32);
		for (int i=0;i<len;i++)
		{
			Line *line=new Line(-1);
			line->m_posNum=pos[i] ;
			line->m_pCoords = new double[Line::DIMENSION * line->m_posNum];
			GIS_Embedded::GlobalCrt crt;
			for (uint_32 ii=0;ii<line->m_posNum;ii++)
			{
				int x,y;
				crt.geoToPrj(p[ii*2],p[2*ii+1],&x,&y);
				line->m_pCoords[ii*2]=x;
				line->m_pCoords[ii*2+1]=y;
			}
			shape.push_back(line);
			p+=pos[i]*Line::DIMENSION;
			m_traffic_annotation_layer->clearGeoLabelPos();
			m_feature_layer_render.renderMapLayer(m_style_render,*m_traffic_feature_layer,shape,*m_traffic_annotation_layer,m_screen_pix_map);
			m_traffic_feature_layer->m_feature_layer_style.m_current_style->m_line_render->setFaceColor(color[i%color_size].convertToARGB());
			shape.clear();
		}
		

	}

	vector<MKPOIAnnotation *> MKStandardDataFetcher::getPOIAnnotations()
	{
		AUTO_LOCK_THIS;
		vector<MKPOIAnnotation *> ret;
		getLayerDataInScope(MKMapRect(0,0,1<<30,1<<30));
		int sum = m_layer_global_ids.size();
		for (int i=0;i<sum;i++)
		{
			FeatureLayer* f_layer=m_map_frame.m_feature_layers[m_layer_mapframe_indexs[i]];
			AnnotationLayer* a_layer= m_map_frame.m_annotation_layers[m_layer_mapframe_indexs[i]];

            //if(false)
			if(a_layer->m_layer_type == FeatureLayer::t_POINT)
			{
				for (int j=0;j<m_shapes[i].size();j++)
				{
					Embedded::Point* gm_point = dynamic_cast< Embedded::Point*>(m_shapes[i][j]);
					MKPOIAnnotation *annotation = new MKPOIAnnotation();
					{
						MapPos2D pos;
						gm_point->getPoint(&pos);

						GIS_Embedded::GlobalCrt crt;
						GIS_Embedded::Pos2D geo;
						crt.prjToGeo(pos,geo);
						annotation->setCoordinate(MKLocationCoordinate2D(geo.d1,geo.d0));
                        //Utility::wstr2ustr(gm_point->getName());
						annotation->m_name=Utility::wstr2ustr(gm_point->getName());
						annotation->m_image_path = f_layer->m_feature_layer_style.m_current_style->m_point_render->m_filename;
						annotation->m_maximum_scale=f_layer->m_zoom_max;
						annotation->m_minimum_scale=f_layer->m_zoom_min;
						ret.push_back(annotation);
					}
                    delete_and_set_null(m_shapes[i][j]);
				}
				
			}

		}
        clearData();
		return ret;
	}


}
