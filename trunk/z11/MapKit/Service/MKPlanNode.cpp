/*
 * =====================================================================================
 *
 *       Filename:  MKCircle.cpp
 *
 *    Description:  The MKCircle class is a concrete overlay object 
 *    representing a circular area on a map.
 *
 *        Version:  1.0
 *        Created:  04/12/2012 02:53:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Lucifer (GRF), gurongfang@gmail.com
 *        Company:  ZJU
 *
 * =====================================================================================
 */

#include "MKPlanNode.h"
namespace z11
{


	MKPlanNode::MKPlanNode( const MKLocationCoordinate2D& coord ) :m_coord(coord),m_course(-100000)
	{

	}

	MKPlanNode::MKPlanNode():m_course(-100000)
	{

	}

	MKPlanNode::MKPlanNode( const MKLocationCoordinate2D& coord,double course ) :m_coord(coord),m_course(course)
	{

	}


}