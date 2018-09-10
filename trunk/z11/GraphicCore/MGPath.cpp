/*
 * =====================================================================================
 *
 *       Filename:  MGGraphicPath.cpp
 *
 *    Description:  Path for Canvas
 *
 *        Version:  1.0
 *        Created:  03/20/2012 12:41:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MGPath.h"

namespace z11
{
	z11::MGGraphicPath::MGGraphicPath( const MGPoint pts[],size_t size ) :m_points(pts,pts+size)
	{

	}
	z11::MGGraphicPath::MGGraphicPath( const std::vector<MGPoint> &pts ) :m_points(pts.begin(),pts.end())
	{

	}

	void z11::MGGraphicPath::lineTo( float x,float y )
	{
		m_points.push_back(MGPointMake(x,y));
	}

};
