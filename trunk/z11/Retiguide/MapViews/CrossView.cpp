#include "CrossView.h"
#include "MapKit/Embedded/BasicType/Line.h"
#include "MapKit/Embedded/Application/LayerMgr.h"
#include "GraphicCore/MGCanvas.h"
#include "System/Shared/MGLog.h"
static MGCanvas *cross_view_canvas=NULL;
namespace GIS_Embedded
{
	CrossView::CrossView(int width,int height,int scope_width)
	{
		cross_view_canvas = new MGCanvas(width,height);

		m_scope_meter_width = scope_width;
		m_scope_pix_width = width;
		m_scope_pix_height = height;

		m_headInfo_pixel_height = 40;
		m_rightInfor_pixel_width = 40;

		m_offset_angle = 0;	
		m_auto_rotate = false;

		m_map_frame = NULL;

		std::string file_name = MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/image/");

		m_traffic_light = z11::MGImage::loadImageFromFile(file_name+"traffic_light.png");
		m_right_bar_bg = z11::MGImage::loadImageFromFile(file_name+"crossBg.png");
		for(int i=0,sum=12;i<sum;i++)
		{
			::ostringstream iss;			
			iss << file_name;
			iss << "user/HOST_";
			iss << i;
			iss << ".png";		
			m_host_bitmap[i] = z11::MGImage::loadImageFromFile(iss.str());
		}
		m_center_line_render.setLineColor(0xFFFFFF);//m_line_color = RGB(255,255,255);
		m_center_line_render.m_pixel_width = 4;//GRF  m_center_line_render.m_pixel_width = 2
		m_center_line_render.m_line_style = SOLID;

		m_dash_line_render.setLineColor(0xFFFFFF);//._LINE_COLOR = RGB(255,255,255);;
		m_dash_line_render.m_pixel_width = 1;
		m_dash_line_render.m_line_style = DASH;

		m_line_face_render.setFaceColor(0x646464);// = RGB(100,100,100);
		m_arrow_face_render.setFaceColor(0x0000FF);// = RGB(0,0,255);

		m_scale_line_render.setLineColor(0xffffff);
		m_scale_line_render.m_pixel_width = 2;

		m_scale_poly_render.setFaceColor(0x0000FF);// = RGB(0,0,255);
		m_blackBg_render.setFaceColor(0x0000FF);

		m_big_label_render.setFontSize(12);
		m_big_label_render.setFontWeight(700);
		m_big_label_render.setLabelColor(0xffffff);// = RGB(255,255,255);

		m_small_label_render.setFontSize(10);
		m_small_label_render.setFontWeight(400);
		m_small_label_render.setLabelColor(0xffffff);// = RGB(255,255,255);

		m_cross_info_label = new MILabel();
	}
	
	void CrossView::setMapFrame(const MapFrame* map_frame)
	{
		m_map_frame = map_frame;
		m_arrow_width = map_frame->m_arrow_width;
		m_arrow_face_render.setFaceColor(map_frame->m_arrow_color);
		m_bg_clor = map_frame->m_cross_bg_color;
	}
	
	void CrossView::setCrossCenter(const Pos2DEx& cross_center)
	{
		m_view_scope.m_left = cross_center.d0 - m_scope_meter_width;
		m_view_scope.m_right = cross_center.d0 + m_scope_meter_width;
		double scope_height = m_scope_meter_width*m_scope_pix_height/m_scope_pix_width;
		m_view_scope.m_top = cross_center.d1 + scope_height;
		m_view_scope.m_bottom = cross_center.d1 - scope_height;

		m_scope_adjust.setScope(m_view_scope);
		m_scope_adjust.rotateAdjust(m_auto_rotate,m_scope_pix_width,m_scope_pix_height);
	}
	
	int CrossView::setPathLine(vector<Pos2DEx>& path)
	{
		m_arrow.clear();
		CrossTrans::arrowTrans(path,m_arrow_width,m_view_scope,&m_arrow);
		return 0;
	}

	bool CrossView::getCrossData()
	{
		bool flag;
		Navigation* navi = Navigation::getInstance();
		flag = navi->getCrossData(m_cross_info);
		setCrossCenter(m_cross_info.cross_center);
		return flag;
	}

	int CrossView::transGeotoScreen(int_32 geo_x,int_32 geo_y,long *screen_x,long *screen_y)
	{
		m_transformer.setTansformPara(m_view_scope,m_offset_angle,m_scope_pix_width,m_scope_pix_height);
		return m_transformer.transfromMapToScr(geo_x,geo_y,screen_x,screen_y);
	}

	int CrossView::getRoadsFromTopoData(vector<CrossRoad*>* cross_roads)
	{
		static vector< Embedded::Line*> data;
		data.clear();

		MapPos2D lefLow(m_scope_adjust.m_left, m_scope_adjust.m_bottom), rightTop(m_scope_adjust.m_right, m_scope_adjust.m_top);
		Rect rect(lefLow, rightTop);		

		LayerMgr* mgr = LayerMgr::getInstance();
		mgr->queryLineByRect(rect, data, true);

		int road_sum = CrossTrans::crossRoadsTrans(cross_roads,data,m_scope_adjust,*m_map_frame);
		return road_sum;
	}

	void CrossView::render(int offset_x,int offset_y)
	{
		PolygonStyleRender::fillRect(cross_view_canvas,offset_x,offset_y,m_scope_pix_width-1,m_scope_pix_height-1,m_bg_clor);
		renderCross(0,0);
		if (cross_view_canvas!=NULL)
		{
			cross_view_canvas->draw(true);
		}
		renderStatic();
		renderFloat();
	}

	void CrossView::renderCross(int offset_x,int offset_y)
	{
		//if (m_arrow.size()<2) //lw
		//	return -1;

		Scope view_screen_scope(offset_x,m_scope_pix_height,m_scope_pix_width,offset_y);
	
		PolygonStyleRender::fillRect(cross_view_canvas,offset_x,offset_y,m_scope_pix_width-1,m_scope_pix_height-1,m_bg_clor);
		m_transformer.setTansformPara(m_view_scope,m_offset_angle,m_scope_pix_width,m_scope_pix_height);

		//not delete but overwrite
		//zj
		vector<CrossRoad*> cross_roads;
		//zj
		int road_sum = getRoadsFromTopoData(&cross_roads);

		vector<z11::MGPoint> control_pos(256);
		int control_sum;

		for(int i=0;i<road_sum;i++)
		{
			control_sum = cross_roads[i]->m_external_line.size();
			control_pos.resize(control_sum);
			m_transformer.transformPos2DsToArray(cross_roads[i]->m_external_line,control_sum,&control_pos[0]);
			m_line_face_render.fillPolygonWithFillColor(cross_view_canvas,control_pos);
		}

		for(int i=0;i<road_sum;i++)
		{
			if(cross_roads[i]->m_grade <= 0)
				continue;

			for(int ii=0,iisum=cross_roads[i]->m_compartmentation_lines.size();ii<iisum;ii++)
			{
				control_sum =  cross_roads[i]->m_compartmentation_lines[ii].size();
				control_pos.resize(control_sum);

				m_transformer.transformPos2DsToArray(cross_roads[i]->m_compartmentation_lines[ii],control_sum,&control_pos[0]);
				
				//GRF 
				//m_dash_line_render.drawPolyline(cross_view_canvas,control_pos);
			}

			control_sum = cross_roads[i]->m_center_line.size();

			control_pos.resize(control_sum);

			m_transformer.transformPos2DsToArray(cross_roads[i]->m_center_line,control_sum,&control_pos[0]);

			m_center_line_render.drawPolyline(cross_view_canvas,control_pos);
			
		}
	}

	void CrossView::renderStatic()
	{
 		m_blackBg_render.fillRectWithFillColor(cross_view_canvas,0,0,m_scope_pix_width,m_headInfo_pixel_height);//-m_rightInfor_pixel_width+1
 		m_blackBg_render.fillRectWithFillColor(cross_view_canvas,0,m_scope_pix_height-m_headInfo_pixel_height,m_scope_pix_width,m_headInfo_pixel_height);
//  		m_image_render.drawImageLeftTop(m_right_bar_bg,m_scope_pix_width-m_rightInfor_pixel_width,0);
 
 		if(m_traffic_light != NULL)
 		{
 			long screen_x,screen_y;
 			transGeotoScreen(m_cross_info.cross_center.d0,m_cross_info.cross_center.d1,&screen_x,&screen_y);
 			m_image_render.drawImageCenterXY(m_traffic_light,screen_x,screen_y);//+m_headInfo_pixel_height
 		}	
 
 		//scale ranks
 		int total_length = m_cross_info.path_length_all/100*100 + (m_cross_info.path_length_all/50)%2*100;
 		static int left_x = m_scope_pix_width - m_rightInfor_pixel_width/4 + 2;
 		static int right_x = left_x + 3;
 
 
 		static int top_y = m_headInfo_pixel_height *3/4;
 		static int space_length = m_scope_pix_height - 3*m_headInfo_pixel_height*3/4;
 		//for(int length_index=0,length_sum = total_length/100;length_index<length_sum;length_index++)
 		//	m_scale_line_render.drawLine(left_x,top_y + length_index*space_length/length_sum,right_x,top_y + length_index*space_length/length_sum);
	}

	void CrossView::renderFloat()
	{

 		static int max_height = m_scope_pix_height-m_headInfo_pixel_height*3/2 - m_headInfo_pixel_height*3/4;
 
 		int process_height = (int)(m_cross_info.path_length_left * max_height/m_cross_info.path_length_all);
 		if ( process_height >= max_height-4)
 			process_height =  max_height-5;
 
 		::ostringstream length_left;
 		length_left << m_cross_info.path_length_left;
 		length_left << " M";

		int total_length_i = m_cross_info.path_length_all/100*100 + (m_cross_info.path_length_all/50)%2*100;
 
 		::ostringstream total_length;
 		total_length << total_length_i;
 		total_length << " M";
 
 		long screen_x,screen_y;
 		transGeotoScreen(m_cross_info.host_pos.d0,m_cross_info.host_pos.d1,&screen_x,&screen_y);
 
// 		//for 1/3 height
////  		m_big_label_render.drawStringCenterXY(length_left.str(),(m_scope_pix_width-m_rightInfor_pixel_width)/2,m_headInfo_pixel_height/2);//m_scope_pix_width+25
// 		m_big_label_render.drawStringCenterXY(m_cross_info.path_name,(m_scope_pix_width-m_rightInfor_pixel_width)/2,m_scope_pix_height/2);//+46
// 		m_small_label_render.drawStringCenterXY(total_length.str(),(m_scope_pix_width-m_rightInfor_pixel_width)/2,m_scope_pix_height-m_headInfo_pixel_height/2);// m_scope_pix_width+25- m_headInfo_pixel_height - m_headInfo_pixel_height/5 +50);
 		if(screen_x >=0 && screen_x <= m_scope_pix_width - m_rightInfor_pixel_width && screen_y >= m_headInfo_pixel_height && screen_y <= m_scope_pix_width-m_headInfo_pixel_height)
			m_image_render.drawImageCenterXY(m_host_bitmap[m_cross_info.host_dir_clock],screen_x,screen_y /*+ m_headInfo_pixel_height+50*/);
		
		//MGLogD("m_cross_info.host_dir_clock %d",m_cross_info.host_dir_clock);

		m_big_label_render.drawStringCenterXY(m_cross_info.path_name+" left length: "+length_left.str()+"  total length: "+total_length.str(),
			(m_scope_pix_width-m_rightInfor_pixel_width)/2,10);
		//m_cross_info_label->setText(m_cross_info.path_name+" left length: "+length_left.str()+"  total length: "+total_length.str());
	}

	void CrossView::drawself()
	{
		MGGraphicEngine *engine=MGGraphicEngine::sharedGraphicEngine();
		engine->enableClip(m_bounds);
		MIView::drawself();
		render(0,0);
		engine->disableClip();
	}
	void CrossView::initWithFrame( z11::MGRect frame )
	{
		_instance=this;
		MIView::initWithFrame(frame);
		m_cross_info_label->initWithFrame(MGRectMake(10,10,300,30));
		m_cross_info_label->setFontSizeAndColor(17,RGBAColorRef::white());
		addSubview(m_cross_info_label);
	}

	CrossView::~CrossView()
	{
		delete_and_set_null(cross_view_canvas);
	}
	CrossView *CrossView::_instance=NULL;
}
