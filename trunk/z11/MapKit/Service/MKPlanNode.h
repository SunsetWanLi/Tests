/*
 * =====================================================================================
 *
 *       Filename:  MKPlanNode.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/12/2012 02:53:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */
#pragma once
#include "../DataTypes/Base/MKGeometry.h"
#include <string>
namespace z11
{
	class MKPlanNode
	{
	public:
		MKPlanNode();
		MKPlanNode(const MKLocationCoordinate2D& coord);
		MKPlanNode(const MKLocationCoordinate2D& coord,double course);
		std::string name;
		MKLocationCoordinate2D m_coord;
		double m_course;
	};
}
