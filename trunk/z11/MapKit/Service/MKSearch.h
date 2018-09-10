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
#include "MKPlanNode.h"
#include <vector>
#include <deque>
namespace Embedded
{
	class Pathfinding;
}
namespace z11
{
	class RoadId
	{
	public:
        long long m_id;
        MKLocationCoordinate2D m_pos;   //Current Position
        MKLocationCoordinate2D m_begin;
        MKLocationCoordinate2D m_end;
		 RoadId();
	protected:
	private:
	};
	typedef std::vector<MKLocationCoordinate2D> MKLineCoordinate2D;
	class MKSearch
	{
	public:
		double computeShortestRoute(const MKPlanNode&start,const MKPlanNode&end);
		double computeShortestRoute(const MKPlanNode&start,const MKPlanNode&end,std::deque<MKLocationCoordinate2D>&route);
		double getDistance(const MKLocationCoordinate2D &start,const MKLocationCoordinate2D &end);
		RoadId bindPoint(const MKPlanNode &pt);
		RoadId bindPoint(const MKPlanNode &pt,std::vector<MKLocationCoordinate2D>&route);
	private:
		Embedded::Pathfinding* initPathFinding();
	};
}
