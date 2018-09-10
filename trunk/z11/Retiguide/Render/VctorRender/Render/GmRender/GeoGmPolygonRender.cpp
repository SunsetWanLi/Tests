#include "GeoGmPolygonRender.h"
#include "Retiguide/Common/String_Tools.h"
#include "MapKit/Embedded/BasicType/Polygon.h"
#include "Retiguide/SpatialMath/SpatialMath.h"
#include "System/Shared/MGLog.h"
//#include "include/MGFoundation.h"
using namespace Embedded;

namespace GIS_Embedded
{
	vector<Pos2DEx> GeoGmPolygonRender::m_clip_buff = vector<Pos2DEx>(1000);
	int GeoGmPolygonRender::m_clip_buff_size = 1000;

	int GeoGmPolygonRender::render(StyleRender& mg)
	{
  		Embedded::Polygon* gm_polygon =dynamic_cast< Embedded::Polygon*>(m_geo);
  
  		std::vector<Embedded::ScreenPos2D> points;
  		gm_polygon->getPoints(&points);
  
		//vector<Pos2DEx> out_ring_pos2Ds(points.size());//points.size()
		//for (uint_32 k=0; k<points.size(); k++)
		//{
		//	out_ring_pos2Ds[k].d0 = points[k].d0;
		//	out_ring_pos2Ds[k].d1 = points[k].d1;
		//}

		vector<Pos2DEx> out_ring_pos2Ds(points.begin(),points.end());
  

  		int pos2d_sum = out_ring_pos2Ds.size();
 
  
/*   			resizeClipBuff(pos2d_sum);
    			pos2d_sum = SpatialMath::polygon_clip(pos2d_sum, &out_ring_pos2Ds[0], &m_clip_buff[0], 
    							m_scope_adjust->m_left, m_scope_adjust->m_bottom, m_scope_adjust->m_right , m_scope_adjust->m_top);
 */  
   			resizeGmBuff(pos2d_sum+COORDINATE_CLIP_BUFFER);
  			m_buff_valid = m_transformer->transformPos2DsToArray(out_ring_pos2Ds,pos2d_sum, m_gm_buff);//m_clip_buff
  
			//MGLogD("draw fillPolygonWithFillColor times %d\n",pos2d_sum);


  		mg.fillPolygonWithFillColor(m_gm_buff, m_buff_valid); 
//			mg.drawPolygonLine(m_gm_buff, m_buff_valid); 

	//	mg.drawPolygonLine(m_gm_buff,m_buff_valid);//draw polygon line        lw




		return 0;

		//int inner_ring_sum = gm_polygon->_inner_rings.size();

		//for(int i = 0; i < inner_ring_sum; i++)
		//{
		//	GeometryLine &inner_ring = gm_polygon->_inner_rings[i];

		//	pos2d_sum = inner_ring._control_pos2d.size();

		//	vector<Pos2DEx> &inner_ring_pos2Ds = inner_ring._control_pos2d;

		//	resizeClipBuff(pos2d_sum);

		//	m_transformer->transformPos2DsToArray(inner_ring_pos2Ds,pos2d_sum, m_gm_buff);


		//	mg.fillPolygonWithFillColor(m_gm_buff, pos2d_sum);


		//}


	}
}


//using namespace GIS_Embedded;
//
//vector<Pos2DEx> GeoGmPolygonRender::m_buf = vector<Pos2DEx>(1000);
//int GeoGmPolygonRender::BUF_SIZE = 1000;
//
//z11::MGPoint *GeoGmPolygonRender::buff = new z11::MGPoint[1000];
//int GeoGmPolygonRender::buff_size = 1000;
////GeoGmPolygonRender::GeoGmPolygonRender(){
////	m_buf = new Pos2D[BUF_SIZE];
////}
////
////GeoGmPolygonRender::~GeoGmPolygonRender()
////{
////	delete []m_buf;
////}
//
////void GeoGmPolygonRender::setGeo(GeographyObject& geo){
////	
////	_geo = &geo;
////	_gm_polygon = (GeometryPolygon*)geo._geometry;
////
////}
//
//void GeoGmPolygonRender::getClipBuf(uint_32 orig_sum)
//{
//	int ret_size = orig_sum * 2 + 6;
//	if(ret_size > BUF_SIZE)
//	{
//		//delete[] m_buf;
//		//m_buf = new Pos2D[ret_size];
//		m_buf.resize(ret_size);
//		BUF_SIZE = ret_size;
//	}
//	//return m_buf;
//}
//
//void GeoGmPolygonRender::render(StyleRender& mg){
//
//	GeometryLine &out_ring = _gm_polygon->_exterior_ring;
//
//	int pos2d_sum = out_ring._control_pos2d.size();
//
//	vector<Pos2DEx> tmp_out_ring(pos2d_sum + 1);
//
//	vector<Pos2DEx> &out_ring_pos2Ds = out_ring._control_pos2d;	//copy
//	
//	//for(int i = 0; i < pos2d_sum; i ++)
//	//{
//	//	tmp_out_ring[i] = out_ring_pos2Ds[i];
//	//}
//	copy(out_ring_pos2Ds.begin(), out_ring_pos2Ds.end(), tmp_out_ring.begin());
//	tmp_out_ring[pos2d_sum] = out_ring_pos2Ds[0];
//
//	Scope scope;
//	_transformer->getScreenScope(&scope);
//
//	if(_transformer->getScreenOffsetAngle() != 0.0)
//		scope.rotateAdjust(); //防止旋转时产生剪切后空白
//
//	getClipBuf(pos2d_sum + 1);
//	pos2d_sum = SpatialMath::polygon_clip(pos2d_sum + 1, &tmp_out_ring[0], &m_buf[0], scope._left, scope._bottom, scope._right, scope._top);
//
//	//out_ring_pos2Ds.clear();
//
//	//for(int i = 0; i < pos2d_sum; i ++)
//	//{
//	//	out_ring_pos2Ds.push_back(new_pts[i]);
//	//}
//
//	//z11::MGPoint* out_control_points = new z11::MGPoint[new_size];
//	if(pos2d_sum > buff_size)
//	{
//		delete buff;
//		buff = new z11::MGPoint[pos2d_sum];
//		buff_size = pos2d_sum;
//	}
//
//	_transformer->transformPos2DsToArray(m_buf, pos2d_sum, buff);
//
//    mg.fillPolygonWithFillColor(buff, pos2d_sum); 
//
//	int inner_ring_sum = _gm_polygon->_inner_rings.size();
//
//    for(int i = 0; i < inner_ring_sum; i++)
//    {
//		GeometryLine &inner_ring = _gm_polygon->_inner_rings[i];
//
//		pos2d_sum = inner_ring._control_pos2d.size();
//
//		vector<Pos2DEx> &inner_ring_pos2Ds = inner_ring._control_pos2d;
//
//		if(pos2d_sum > buff_size)
//		{
//			delete buff;
//			buff = new z11::MGPoint[pos2d_sum];
//			buff_size = pos2d_sum;
//		}
//		//z11::MGPoint* inner_control_points = new z11::MGPoint[pos2d_sum];
//
//		_transformer->transformPos2DsToArray(inner_ring_pos2Ds,buff);
//
//		//tmp
//		//mg.fillPolygonWithContextColor(buff, pos2d_sum);
//		mg.fillPolygonWithFillColor(buff, pos2d_sum);
//
//		//delete []inner_control_points;
//
//    }
//
//	//delete []out_control_points;
//
//
//}

//long GeoGmPolygonRender::polygon_clip( long src_sum, const Pos2D * src_pts, 
//					Pos2D * dest_pts, int x1, int y1, int x2, int y2 ) const
//{
//	// 区域裁减算法，依次将四条边外面的删掉�?
//	// 首先是左�?
//	static Pos2D * temp_pts = NULL;
//	static uint_32 current_size = 0;
//	if(current_size < src_sum * 2)
//	{
//		current_size = src_sum * 2;
//		delete []temp_pts;
//		temp_pts = new Pos2D[current_size];
//	}
//	
//	long i, k;
//	long size = src_sum;
//	bool inflag;
//	// 始点如果在里面，首先加入
//	if( size > 0 && src_pts[0].d0 >= x1 )
//	{
//		temp_pts[0] = src_pts[0];
//		k = 1;
//		inflag = true;
//	}
//	else
//	{
//		k = 0; 
//		inflag = false;
//	}
//	for( i = 1; i < size; i++ )
//	{
//		if( true == inflag )
//		{
//			// 都在里面，加入终点，
//			if( src_pts[i].d0 >= x1 )
//				temp_pts[k++] = src_pts[i];
//			// 由里到外，加入交界点�?
//			else 
//			{
//				double y;
//				y = src_pts[i - 1].d1 + ( src_pts[i].d1 - src_pts[i - 1].d1 ) 
//						/ ( src_pts[i].d0 - src_pts[i - 1].d0 ) 
//						* (x1 - src_pts[i - 1].d0);
//				temp_pts[k].d0 = x1;
//				temp_pts[k].d1 = y;
//				k++;
//				inflag = false;
//			}
//		}
//		else
//		{
//			// 由外到内，加入交界点和终点�?
//			if( src_pts[i].d0 >= x1 )
//			{
//				double y;
//				y = src_pts[i - 1].d1 + ( src_pts[i].d1 - src_pts[i - 1].d1 ) 
//						/ ( src_pts[i].d0 - src_pts[i - 1].d0 ) 
//						* (x1 - src_pts[i - 1].d0);
//				temp_pts[k].d0 = x1;
//				temp_pts[k].d1 = y;
//				k++;
//				temp_pts[k++] = src_pts[i];
//				inflag = true;
//			}
//			// 都在外边，不�?
//		//	else if( src_pts[i].x < x1 && src_pts[i + 1].x < x1 )
//		//		;
//		}
//	}
//	// 0601；如果原来首尾点重合，新的首尾点不重合，则强制重合，保证面切割的正确性�?
//	if( k >= 2 && src_pts[0].d0 == src_pts[size - 1].d0 
//		&& src_pts[0].d1 == src_pts[size - 1].d1 
//		&& (temp_pts[0].d0 != temp_pts[k - 1].d0 
//			|| temp_pts[0].d1 != temp_pts[k - 1].d1)  )
//	{
//		// 增加一个点�?
//		temp_pts[k++] = temp_pts[0];
//	}
//
//	// 然后是右边，
//	size = k;
//	if( size > 0 && temp_pts[0].d0 <= x2 )
//	{
//		dest_pts[0] = temp_pts[0];
//		k = 1;
//		inflag = true;
//	}
//	else
//	{
//		k = 0;
//		inflag = false;
//	}
//	for( i = 1; i < size; i++ )
//	{
//		if( true == inflag )
//		{
//			// 都在里面，加入终点，
//			if( temp_pts[i].d0 <= x2 )
//				dest_pts[k++] = temp_pts[i];
//			// 由里到外，加入交界点�?
//			else 
//			{
//				double y;
//				y = temp_pts[i - 1].d1 + ( temp_pts[i].d1 - temp_pts[i - 1].d1 ) 
//						/ ( temp_pts[i].d0 - temp_pts[i - 1].d0 ) 
//						* (x2 - temp_pts[i - 1].d0);
//				dest_pts[k].d0 = x2;
//				dest_pts[k].d1 = y;
//				k++;
//				inflag = false;
//			}
//		}
//		else
//		{
//			// 由外到内，加入交界点和终点�?
//			if( temp_pts[i].d0 <= x2 )
//			{
//				double y;
//				y = temp_pts[i - 1].d1 + ( temp_pts[i].d1 - temp_pts[i - 1].d1 ) 
//						/ ( temp_pts[i].d0 - temp_pts[i - 1].d0 ) 
//						* (x2 - temp_pts[i - 1].d0);
//				dest_pts[k].d0 = x2;
//				dest_pts[k].d1 = y;
//				k++;
//				dest_pts[k++] = temp_pts[i];
//				inflag = true;
//			}
//			// 都在外边，不�?
//		}
//	}
//	if( k >= 2 && temp_pts[0].d0 == temp_pts[size - 1].d0 
//		&& temp_pts[0].d1 == temp_pts[size - 1].d1 
//		&& (dest_pts[0].d0 != dest_pts[k - 1].d0 
//			|| dest_pts[0].d1 != dest_pts[k - 1].d1) )
//	{
//		// 增加一个点�?
//		dest_pts[k++] = dest_pts[0];
//	}
//
//	// 首先是下�?
//	size = k;
//	if( k > 0 && dest_pts[0].d1 >= y1 )
//	{
//		temp_pts[0] = dest_pts[0];
//		k = 1;
//		inflag = true;
//	}
//	else
//	{
//		k = 0;
//		inflag = false;
//	}
//	for( i = 1; i < size; i++ )
//	{
//		if( true == inflag )
//		{
//			// 都在里面，加入终点，
//			if( dest_pts[i].d1 >= y1 )
//				temp_pts[k++] = dest_pts[i];
//			// 由里到外，加入交界点�?
//			else 
//			{
//				double x;
//				x = dest_pts[i - 1].d0 + ( dest_pts[i].d0 - dest_pts[i - 1].d0 ) 
//						/ ( dest_pts[i].d1 - dest_pts[i - 1].d1 ) 
//						* (y1 - dest_pts[i - 1].d1);
//				temp_pts[k].d0 = x;
//				temp_pts[k].d1 = y1;
//				k++;
//				inflag = false;
//			}
//		}
//		else
//		{
//			// 由外到内，加入交界点和终点�?
//			if( dest_pts[i].d1 >= y1 )
//			{
//				double x;
//				x = dest_pts[i - 1].d0 + ( dest_pts[i].d0 - dest_pts[i - 1].d0 )
//						/ ( dest_pts[i].d1 - dest_pts[i - 1].d1 ) 
//						* (y1 - dest_pts[i - 1].d1);
//				temp_pts[k].d0 = x;
//				temp_pts[k].d1 = y1;
//				k++;
//				temp_pts[k++] = dest_pts[i];
//				inflag = true;
//			}
//			// 都在外边，不�?
//		}
//	}
//	if( k >= 2 && dest_pts[0].d0 == dest_pts[size - 1].d0 
//		&& dest_pts[0].d1 == dest_pts[size - 1].d1 
//		&& (temp_pts[0].d0 != temp_pts[k - 1].d0 
//			|| temp_pts[0].d1 != temp_pts[k - 1].d1) )
//	{
//		// 增加一个点�?
//		temp_pts[k++] = temp_pts[0];
//	}
//
//	// 最后是上边�?
//	size = k;
//	if( k > 0 && temp_pts[0].d1 <= y2 )
//	{
//		dest_pts[0] = temp_pts[0];
//		k = 1;
//		inflag = true;
//	}
//	else
//	{
//		k = 0;
//		inflag = false;
//	}
//	for( i = 1; i < size; i++ )
//	{
//		if( true == inflag )
//		{
//			// 都在里面，加入终点，
//			if( temp_pts[i].d1 <= y2)
//				dest_pts[k++] = temp_pts[i];
//			// 由里到外，加入交界点�?
//			else 
//			{
//				double x;
//				x = temp_pts[i - 1].d0 + ( temp_pts[i].d0 - temp_pts[i - 1].d0 ) 
//						/ ( temp_pts[i].d1 - temp_pts[i - 1].d1 ) 
//						* (y2 - temp_pts[i - 1].d1);
//				dest_pts[k].d0 = x;
//				dest_pts[k].d1 = y2;
//				k++;
//				inflag = false;
//			}
//		}
//		else
//		{
//			// 由外到内，加入交界点和终点�?
//			if( temp_pts[i].d1 <= y2 )
//			{
//				double x;
//				x = temp_pts[i - 1].d0 + ( temp_pts[i].d0 - temp_pts[i - 1].d0 ) 
//						/ ( temp_pts[i].d1 - temp_pts[i - 1].d1 ) 
//						* (y2 - temp_pts[i - 1].d1);
//				dest_pts[k].d0 = x;
//				dest_pts[k].d1 = y2;
//				k++;
//				dest_pts[k++] = temp_pts[i];
//				inflag = true;
//			}
//			// 都在外边，不�?
//		}
//	}
//	if( k >= 2 && temp_pts[0].d0 == temp_pts[size - 1].d0 
//		&& temp_pts[0].d1 == temp_pts[size - 1].d1 
//		&& (dest_pts[0].d0 != dest_pts[k - 1].d0 
//			|| dest_pts[0].d1 != dest_pts[k - 1].d1) )
//	{
//		// 增加一个点�?
//		dest_pts[k++] = dest_pts[0];
//	}
//
//	//delete[] temp_pts;
//	return k;
//}
//
