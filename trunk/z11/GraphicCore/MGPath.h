/*
 * =====================================================================================
 *
 *       Filename:  MGGraphicPath.h
 *
 *    Description:  Path for Canvas
 *
 *        Version:  1.0
 *        Created:  03/20/2012 12:41:09 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma once

#include <vector>
#include "../CommonBase/MGStructure.h"
namespace z11
{
	enum MGLineCap
	{
		MGLineCapButt,
		MGLineCapRound,
		MGLineCapSquare
	};

	enum MGLineJoin
	{
		MGLineJoinMiter,
		MGLineJoinRound,
		MGLineJoinBevel
	};

	class MGGraphicPath
	{
	public:
		MGGraphicPath(const MGPoint pts[],size_t size);

		MGGraphicPath(const std::vector<MGPoint> &pts );

		void lineTo(float x,float y);

	private:
		std::vector<MGPoint> m_points;
		friend class MGCanvas;
	};
}
