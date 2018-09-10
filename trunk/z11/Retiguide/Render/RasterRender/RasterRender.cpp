#include "RasterRender.h"
#include "Retiguide/RenderEngine/ImageRender/ImageStyleRender.h"
#include "Retiguide/Common/CRS/GlobalCrt.h"
#include "Retiguide/Common/Util/Transformer.h"

#include "System/Device/MGDevice.h"
#include "System/io/MGBundle.h"

#include <math.h>

using namespace z11;

namespace GIS_Embedded
{
	RasterData::RasterData(int current_floor_num = 0)
	{
		m_current_floor_num = current_floor_num;
		m_center_prj.d0 = 0;
		m_center_prj.d1 = 0;
		//rasterDataInit();
	}

	RasterData::~RasterData()
	{
		if(m_ip)
			delete m_ip;
// 		if(m_gx_bmp_DC)
// 			GXDestroyDC(m_gx_bmp_DC);
// 		if(m_gx_mem_DC)
// 			GXDestroyDC(m_gx_mem_DC);

// 		GIS_Embedded::DataFetcher* fetcher = GIS_Embedded::DataFetcher::getInstance();

// 		fetcher->releaseBitmapMyBITMAP(m_host_bmp_file_name.c_str());///m_host_bmp 娌℃湁鏋愭瀯鎺?闇�瑕侀噴鏀?
	}


	int RasterData::rasterDataInit(const string& raster_map_name, const string& raster_map_root_path)
	{
		m_ip = new GIS_Embedded::ImagePyramid();


		if("" == raster_map_name || "" == raster_map_root_path)
			return -1;

		m_ip->init(raster_map_name.c_str(), raster_map_root_path.c_str());

		m_resolution_width = z11::MGDevice::getScreenWidth();
		m_resolution_height = z11::MGDevice::getScreenHeight();

		m_delta_x = m_delta_y = 0;
		m_zoom = false;

		m_crs_config = m_ip->getImagePyramidConfig();
// 		m_crt_affine = m_ip->getCRTAffine();

		m_total_floor = m_crs_config->m_floor_num;

		m_rect.left_low.d0 = m_crs_config->m_left;
		m_rect.left_low.d1 = m_crs_config->m_bottom;

		setRect(m_current_floor_num);

		m_host_prj_x = m_host_prj_y = 0;
		m_host_bmp_file_name = "host";

		return 0;
	}

	void RasterData::setRect(int floor_num)
	{
		if(m_center_prj.d0 == 0 || m_center_prj.d1 == 0)
		{
			m_rect.right_top.d0 = m_rect.left_low.d0 + m_resolution_width * pow(2.0,floor_num) / m_crt_affine->a ;
			m_rect.right_top.d1 = m_rect.left_low.d1 + m_resolution_height  * pow(2.0,floor_num) / m_crt_affine->e;

			m_center_prj.d0 = m_rect.left_low.d0 + m_resolution_width  * pow(2.0,floor_num - 1) / m_crt_affine->a;
			m_center_prj.d1 = m_rect.left_low.d1 + m_resolution_height  * pow(2.0,floor_num - 1) / m_crt_affine->e;
		}
		else
		{
			m_rect.left_low.d0 = m_center_prj.d0 - m_resolution_width  * pow(2.0,floor_num - 1) / m_crt_affine->a;
			m_rect.left_low.d1 = m_center_prj.d1 - m_resolution_height * pow(2.0,floor_num - 1) / m_crt_affine->e;
			m_rect.right_top.d0 = m_center_prj.d0 + m_resolution_width * pow(2.0,floor_num - 1) / m_crt_affine->a;
			m_rect.right_top.d1 = m_center_prj.d1 + m_resolution_height * pow(2.0,floor_num - 1) / m_crt_affine->e;
		}

	}

	void RasterData::adjustRect(double prj_move_x, double prj_move_y)
	{
		m_rect.left_low.d0 -= prj_move_x ;
		m_rect.left_low.d1 += prj_move_y ;
		m_rect.right_top.d0 -= prj_move_x ;
		m_rect.right_top.d1 += prj_move_y ;

		m_center_prj.d0 -= prj_move_x;
		m_center_prj.d1 +=prj_move_y;
	}

	void RasterData::resizer(int cur_floor_num)
	{
		if (cur_floor_num == m_total_floor-1 || cur_floor_num == 0)
			return;

		m_current_floor_num = cur_floor_num;
		m_delta_x = 0;
		m_delta_y = 0;

		m_zoom = true;

		setRect(m_current_floor_num);

	}
	void RasterData::zoomOut()
	{
		if(m_current_floor_num == m_total_floor - 1)//宸茬粡鏄渶灏忎簡
			return;
		m_current_floor_num ++;
		m_delta_x = 0;
		m_delta_y = 0;

		m_zoom = true;

		setRect(m_current_floor_num);
	}


	void RasterData::zoomIn()
	{
		if(m_current_floor_num == 0)//宸茬粡鏄渶澶т簡
			return;
		m_current_floor_num --;
		m_delta_x = 0;
		m_delta_y = 0;

		m_zoom = true;

		setRect(m_current_floor_num);
	}

	void RasterData::move(int delta_x, int delta_y)
	{
// 		m_delta_x = delta_x;
// 		m_delta_y = delta_y;
// 		m_zoom = false;
		//lw
 		double prj_move_x = 0, prj_move_y = 0,start_x = 0,start_y =0;
		double longitude = 0,latitude = 0;
		scrPosToPrjPos(0,0,start_x,start_y);
		scrPosToPrjPos(delta_x,delta_y,prj_move_x,prj_move_y);
// 		prjPosToGeoPos(prj_move_x,prj_move_y,longitude,latitude);
		adjustRect(prj_move_x-start_x,prj_move_y-start_y);
//  
//  		double PrjToScr_resolution = m_ip->getProToScr_rate();
//  		prj_move_x = m_delta_x * PrjToScr_resolution;
//  		prj_move_y = m_delta_y * PrjToScr_resolution;
// 
//  		adjustRect(prj_move_x, prj_move_y);

/* 		adjustRect(m_delta_x, m_delta_y);*/

		//prjPosToScrPos(m_host_prj_x, m_host_prj_y, m_host_scr_x, m_host_scr_y);
		//renderHostPosition(m_gx_mem_DC, m_host_scr_x, m_host_scr_y);
	}

	int RasterData::render(/*MyDC myDC*/)
	{
		double prj_move_x = 0, prj_move_y = 0;
		SPosition left_top;
		unsigned int width = 0, height = 0;

		double PrjToScr_resolution = m_ip->getProToScr_rate();
		prj_move_x = m_delta_x * PrjToScr_resolution;
		prj_move_y = m_delta_y * PrjToScr_resolution;

		adjustRect((int)prj_move_x, (int)prj_move_y);

		int ret = m_ip->getImage(m_rect, m_resolution_width, m_resolution_height,&left_top,&width,&height,m_zoom);

 		if(ret >= 0)
 		{
 			if(m_ip->neetNotFetchTiles())
 			{
//  				int_32 lBlit_xs = 0, lBlit_xe = 0, lBlit_ys = 0, lBlit_ye = 0;
//  				m_ip->getBlitRegion(lBlit_xs, lBlit_xe, lBlit_ys, lBlit_ye);
// 
//  				GXRectFill(m_gx_mem_DC, 0, 0, m_resolution_width, m_resolution_height);
// 
//  				GXBlitEx(m_gx_mem_DC, left_top.x, left_top.y,m_gx_bmp_DC, lBlit_xs, lBlit_ys, lBlit_xe, lBlit_ye);
//  				GXBlit(gx_DC, m_gx_mem_DC);
 			}
 			else
 			{
				while (!m_ip->BMPNameEmpty())
 				{
 					MGImage* src_bitmap;
 					string name;
 					m_ip->getBMPName(name);

 					GIS_Embedded::LeftTop* lt = m_ip->getBMPLeftTop();

					src_bitmap = z11::MGImage::loadImageFromFile(MGBundle::mainBundle()->pathWithFileName("GIS_Embedded/image"+name+".bmp"));
					ImageStyleRender::drawImageCenterXY(src_bitmap,lt->left,lt->top);
//left top 会有很大的范围，所以是不是要新建一个view或者什么其他办法？
 					delete lt;
 				}
 			}
 		}


 		long lBlit_xs = 0, lBlit_xe = 0, lBlit_ys = 0, lBlit_ye = 0;
 		m_ip->getBlitRegion(lBlit_xs, lBlit_xe, lBlit_ys, lBlit_ye);

//  		GXBlitEx( eft_top.x, left_top.y, lBlit_xs, lBlit_ys, lBlit_xe, lBlit_ye);

//  		if(0 != m_host_prj_x && 0 != m_host_prj_y)//画起始点图标
//  		{
//  			int host_scr_x = 0, host_scr_y = 0;
//  			prjPosToScrPos(m_host_prj_x, m_host_prj_y, host_scr_x, host_scr_y);
//  			renderHostPosition( host_scr_x, host_scr_y);
//  		}

		return 0;
	}

	int RasterData::getCurrentFloorNumber()
	{
		return m_current_floor_num;
	}

	int RasterData::getTotalFloorNumber()
	{
		return m_total_floor;
	}

	int RasterData::scrPosToPrjPos(int scr_x, int scr_y, double& prj_x, double& prj_y) const
	{
		prj_x = 0;
		prj_y = 0;

		prj_x = (double)scr_x * (m_rect.right_top.d0 - m_rect.left_low.d0) / (double)m_resolution_width + m_rect.left_low.d0;
		prj_y = m_rect.right_top.d1 - (double)scr_y * (m_rect.right_top.d1 - m_rect.left_low.d1) / (double)m_resolution_height;

//		GIS_Embedded::Transformer trans;
//		GIS_Embedded::Scope map_scope;

//		map_scope.m_left = m_rect.left_low.d0;
//		map_scope.m_right = m_rect.right_top.d0;
//		map_scope.m_bottom = m_rect.left_low.d1;
//		map_scope.m_top = m_rect.right_top.d1;

		//map_scope.m_left = (int)(m_ip->getImagePyramidConfig()->m_left);
		//map_scope.m_bottom = (int)(m_ip->getImagePyramidConfig()->m_bottom);
		//map_scope.m_right = (int)(m_ip->getImagePyramidConfig()->m_right);
		//map_scope.m_top = (int)(m_ip->getImagePyramidConfig()->m_top);

		//trans.setTansformPara(map_scope,0,m_resolution_width,m_resolution_height,0);

		//	int_32 temp_prj_x = 0, temp_prj_y = 0;
		//	trans.translateFromScrToMap(scr_x, scr_y, &temp_prj_x, &temp_prj_y);

		//	prj_x = (double) temp_prj_x;
		//	prj_y = (double) temp_prj_y;

		if(isLegalPrjPostion(prj_x, prj_y))
			return 0;
		else
			return -1;
	}

	int RasterData::prjPosToScrPos(double prj_x, double prj_y, int& scr_x, int& scr_y) const
	{
		scr_x = 0;
		scr_y = 0;

		scr_x = (int) ( m_resolution_width * (prj_x - m_rect.left_low.d0) / (m_rect.right_top.d0 - m_rect.left_low.d0) );
		scr_y = (int) ( m_resolution_height * (m_rect.right_top.d1 - prj_y) / (m_rect.right_top.d1 - m_rect.left_low.d1) );

//		GIS_Embedded::Transformer trans;
//		GIS_Embedded::Scope map_scope;

//		map_scope.m_left = m_rect.left_low.d0;
//		map_scope.m_right = m_rect.right_top.d0;
//		map_scope.m_bottom = m_rect.left_low.d1;
//		map_scope.m_top = m_rect.right_top.d1;


		//map_scope.m_left = (int)(m_ip->getImagePyramidConfig()->m_left);
		//map_scope.m_bottom = (int)(m_ip->getImagePyramidConfig()->m_bottom);
		//map_scope.m_right = (int)(m_ip->getImagePyramidConfig()->m_right);
		//map_scope.m_top = (int)(m_ip->getImagePyramidConfig()->m_top);

		//trans.setTansformPara(map_scope,0,m_resolution_width,m_resolution_height,0);

		//int_32 temp_scr_x = 0, temp_scr_y = 0;
		//trans.transfromMapToScr((int_32) prj_x, (int_32) prj_y, &temp_scr_x, &temp_scr_y);

		//scr_x = (int) temp_scr_x;
		//scr_y = (int) temp_scr_y;

		if(isLegalScrPostion(scr_x, scr_y))
			return 0;
		else
			return -1;
	}

	bool RasterData::isLegalPrjPostion(const long& prj_x, const long& prj_y) const
	{
		return (prj_x >= m_crs_config->m_left) && (prj_x <= m_crs_config->m_right) &&
			(prj_y >= m_crs_config->m_bottom) && (prj_y <= m_crs_config->m_top);
	}

	bool RasterData::isLegalScrPostion(const int& scr_x, const int& scr_y) const
	{
		return (scr_x >= 0) && (scr_x <m_resolution_width) &&
			(scr_y >= 0) && (scr_y < m_resolution_height);
	}

	int RasterData::geoPosToPrjPos(double longitude, double latitude, long& prj_x, long& prj_y) const
	{
		GIS_Embedded::GlobalCrt crt;
		long temp_prj_x = 0, temp_prj_y = 0;
		crt.geoToPrj(longitude, latitude, &temp_prj_x, &temp_prj_y);
		prj_x = temp_prj_x;
		prj_y = temp_prj_y;
		return 0;
	}

	int RasterData::prjPosToGeoPos(long prj_x, long prj_y, double& longitude, double& latitude) const
	{
		GIS_Embedded::GlobalCrt crt;
		long temp_prj_x = prj_x, temp_prj_y = prj_y;
		longitude = 0; latitude = 0;
		crt.prjToGeo(temp_prj_x, temp_prj_y, &longitude, &latitude);
		return 0;
	}

	void RasterData::setHostPosition(int host_x, int host_y)
	{
		m_delta_x = m_delta_y = 0;
		scrPosToPrjPos(host_x, host_y, m_host_prj_x, m_host_prj_y);
	}

	void RasterData::setDestPosition(int dest_x,int dest_y)
	{
		m_delta_x = m_delta_y = 0;
		scrPosToPrjPos(dest_x, dest_y, m_dest_prj_x, m_dest_prj_y);
	}
	void RasterData::renderHostPosition(int host_x, int host_y)
	{
// 		GXDC gx_dc = (GXDC) myDC;
// 		rendBMPFromScrPosition(gx_dc, m_host_bmp_file_name, host_x, host_y);
		//GXBlit((GXDC)myDC, m_gx_mem_DC);
	}

	void RasterData::renderDestPostion(int dest_x, int dest_y)
	{
		rendBMPFromScrPosition(m_dest_bmp_file_name, dest_x, dest_y);
	}

	void RasterData::changeHostPosition(long new_host_prj_x, long new_host_prj_y)
	{
		m_host_prj_x = new_host_prj_x;
		m_host_prj_y = new_host_prj_y;
		long delt_prj_x = 0, delt_prj_y = 0;
		delt_prj_x = m_host_prj_x - m_center_prj.d0;
		delt_prj_y = m_host_prj_y - m_center_prj.d1;

		m_rect.left_low.d0 += delt_prj_x;
		m_rect.left_low.d1 += delt_prj_y;
		m_rect.right_top.d0 += delt_prj_x;
		m_rect.right_top.d1 += delt_prj_y;

		m_center_prj.d0 = m_host_prj_x;
		m_center_prj.d1 = m_host_prj_y;

		m_delta_x = m_delta_y = 0;
	}

	void RasterData::locater(long center_x, long center_y)
	{
		double longitude=0,latitude=0;
		prjPosToGeoPos(center_x,center_y,longitude,latitude);
		m_center_prj.d0 = longitude;
		m_center_prj.d1 = latitude;
		setRect(m_current_floor_num);
	}


	void RasterData::getCurrentPrjScope(long* prj_x_start, long* prj_y_start, long* prj_x_end, long* prj_y_end) const
	{
		if(!prj_x_start || !prj_x_end || !prj_y_start || !prj_y_end)
			return;
		geoPosToPrjPos(m_rect.left_low.d0, m_rect.left_low.d1, *prj_x_start, *prj_y_start);
		geoPosToPrjPos(m_rect.right_top.d0, m_rect.right_top.d1, *prj_x_end, *prj_y_end);
		
		//*prj_x_start = m_rect.left_low.d0;
		//*prj_y_start = m_rect.left_low.d1;
		//*prj_x_end = m_rect.right_top.d0;
		//*prj_y_end = m_rect.right_top.d1;

	}

	void RasterData::rendBMPFromPrjPosition(string bmp_name, long prj_x, long prj_y) const
	{
		int scr_x = 0, scr_y = 0;
		prjPosToScrPos(prj_x, prj_y, scr_x, scr_y);
		rendBMPFromScrPosition(bmp_name, scr_x, scr_y);
	}
	void RasterData::rendBMPFromScrPosition(string bmp_name, int scr_x, int scr_y) const
	{
// 		GIS_Embedded::DataFetcher* fetcher = GIS_Embedded::DataFetcher::getInstance();
// 
// 		MyBITMAP bitmap;
// 		fetcher->getBitmap(bmp_name.c_str(), &bitmap); 
// 
// 		long width = 0, height = 0;
// 		GIS_Embedded::ImageStyleRender::getImageSize(bitmap, &width, &height);
// 
// 		GIS_Embedded::ImageStyleRender::drawTransparenceImageLeftTop(myDC, bitmap, scr_x - width/2 , scr_y - height/2);
	}
	void RasterData::setRect(long left,long top,long right,long bottom)
	{
		double tmp_left=0,tmp_top=0,tmp_right=0,tmp_bottom=0;
		prjPosToGeoPos(left,top,tmp_left,tmp_top);
		prjPosToGeoPos(right,bottom,tmp_right,tmp_bottom);
		m_rect.left_low.d0 = tmp_left;
		m_rect.left_low.d1 = tmp_bottom;
		m_rect.right_top.d0 = tmp_right;
		m_rect.right_top.d1 = tmp_top;
		m_center_prj.d0 = (tmp_left+tmp_right)/2;
		m_center_prj.d1 = (tmp_top+tmp_bottom)/2;

	}
}

